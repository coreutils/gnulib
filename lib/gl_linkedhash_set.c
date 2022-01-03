/* Set data type implemented by a hash table with a linked list.
   Copyright (C) 2006, 2008-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2018.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "gl_linkedhash_set.h"

#include <stdint.h> /* for uintptr_t, SIZE_MAX */
#include <stdlib.h>

#include "xsize.h"

/* --------------------------- gl_set_t Data Type --------------------------- */

#include "gl_anyhash1.h"

/* Concrete list node implementation, valid for this file only.  */
struct gl_list_node_impl
{
  struct gl_hash_entry h;           /* hash table entry fields; must be first */
  struct gl_list_node_impl *next;
  struct gl_list_node_impl *prev;
  const void *value;
};
typedef struct gl_list_node_impl * gl_list_node_t;

/* Concrete gl_set_impl type, valid for this file only.  */
struct gl_set_impl
{
  struct gl_set_impl_base base;
  gl_setelement_hashcode_fn hashcode_fn;
  /* A hash table: managed as an array of collision lists.  */
  struct gl_hash_entry **table;
  size_t table_size;
  /* A circular list anchored at root.
     The first node is = root.next, the last node is = root.prev.
     The root's value is unused.  */
  struct gl_list_node_impl root;
  /* Number of list nodes, excluding the root.  */
  size_t count;
};

#define CONTAINER_T gl_set_t
#define CONTAINER_COUNT(set) (set)->count
#include "gl_anyhash2.h"

/* If the symbol SIGNAL_SAFE_SET is defined, the code is compiled in such
   a way that a gl_set_t data structure may be used from within a signal
   handler.  The operations allowed in the signal handler are:
     gl_set_iterator, gl_set_iterator_next, gl_set_iterator_free.
   The set and node fields that are therefore accessed from the signal handler
   are:
     set->root, node->next, node->value.
   We are careful to make modifications to these fields only in an order
   that maintains the consistency of the list data structure at any moment,
   and we use 'volatile' assignments to prevent the compiler from reordering
   such assignments.  */
#ifdef SIGNAL_SAFE_SET
# define ASYNCSAFE(type) *(type volatile *)&
#else
# define ASYNCSAFE(type)
#endif

/* --------------------------- gl_set_t Data Type --------------------------- */

static gl_set_t
gl_linkedhash_nx_create_empty (gl_set_implementation_t implementation,
                               gl_setelement_equals_fn equals_fn,
                               gl_setelement_hashcode_fn hashcode_fn,
                               gl_setelement_dispose_fn dispose_fn)
{
  struct gl_set_impl *set =
    (struct gl_set_impl *) malloc (sizeof (struct gl_set_impl));

  if (set == NULL)
    return NULL;

  set->base.vtable = implementation;
  set->base.equals_fn = equals_fn;
  set->base.dispose_fn = dispose_fn;
  set->hashcode_fn = hashcode_fn;
  set->table_size = 11;
  set->table =
    (gl_hash_entry_t *) calloc (set->table_size, sizeof (gl_hash_entry_t));
  if (set->table == NULL)
    goto fail;
  set->root.next = &set->root;
  set->root.prev = &set->root;
  set->count = 0;

  return set;

 fail:
  free (set);
  return NULL;
}

static size_t _GL_ATTRIBUTE_PURE
gl_linkedhash_size (gl_set_t set)
{
  return set->count;
}

static bool _GL_ATTRIBUTE_PURE
gl_linkedhash_search (gl_set_t set, const void *elt)
{
  size_t hashcode =
    (set->hashcode_fn != NULL
     ? set->hashcode_fn (elt)
     : (size_t)(uintptr_t) elt);
  size_t bucket = hashcode % set->table_size;
  gl_setelement_equals_fn equals = set->base.equals_fn;

  /* Look for a match in the hash bucket.  */
  gl_list_node_t node;

  for (node = (gl_list_node_t) set->table[bucket];
       node != NULL;
       node = (gl_list_node_t) node->h.hash_next)
    if (node->h.hashcode == hashcode
        && (equals != NULL
            ? equals (elt, node->value)
            : elt == node->value))
      return true;
  return false;
}

static int
gl_linkedhash_nx_add (gl_set_t set, const void *elt)
{
  size_t hashcode =
    (set->hashcode_fn != NULL
     ? set->hashcode_fn (elt)
     : (size_t)(uintptr_t) elt);
  size_t bucket = hashcode % set->table_size;
  gl_setelement_equals_fn equals = set->base.equals_fn;

  /* Look for a match in the hash bucket.  */
  {
    gl_list_node_t node;

    for (node = (gl_list_node_t) set->table[bucket];
         node != NULL;
         node = (gl_list_node_t) node->h.hash_next)
      if (node->h.hashcode == hashcode
          && (equals != NULL
              ? equals (elt, node->value)
              : elt == node->value))
        return 0;
  }

  /* Allocate a new node.  */
  gl_list_node_t node =
    (struct gl_list_node_impl *) malloc (sizeof (struct gl_list_node_impl));

  if (node == NULL)
    return -1;

  ASYNCSAFE(const void *) node->value = elt;
  node->h.hashcode = hashcode;

  /* Add node to the hash table.  */
  node->h.hash_next = set->table[bucket];
  set->table[bucket] = &node->h;

  /* Add node to the set.  */
  ASYNCSAFE(gl_list_node_t) node->next = &set->root;
  node->prev = set->root.prev;
  ASYNCSAFE(gl_list_node_t) node->prev->next = node;
  set->root.prev = node;
  set->count++;

  hash_resize_after_add (set);

  return 1;
}

static bool
gl_linkedhash_remove (gl_set_t set, const void *elt)
{
  size_t hashcode =
    (set->hashcode_fn != NULL
     ? set->hashcode_fn (elt)
     : (size_t)(uintptr_t) elt);
  size_t bucket = hashcode % set->table_size;
  gl_setelement_equals_fn equals = set->base.equals_fn;

  /* Look for the first match in the hash bucket.  */
  gl_list_node_t *nodep;

  for (nodep = (gl_list_node_t *) &set->table[bucket];
       *nodep != NULL;
       nodep = (gl_list_node_t *) &(*nodep)->h.hash_next)
    {
      gl_list_node_t node = *nodep;
      if (node->h.hashcode == hashcode
          && (equals != NULL
              ? equals (elt, node->value)
              : elt == node->value))
        {
          /* Remove node from the hash table.  */
          *nodep = (gl_list_node_t) node->h.hash_next;

          /* Remove node from the list.  */
          {
            gl_list_node_t prev = node->prev;
            gl_list_node_t next = node->next;

            ASYNCSAFE(gl_list_node_t) prev->next = next;
            next->prev = prev;
          }
          set->count--;

          if (set->base.dispose_fn != NULL)
            set->base.dispose_fn (node->value);
          free (node);
          return true;
        }
    }

  return false;
}

static void
gl_linkedhash_free (gl_set_t set)
{
  gl_setelement_dispose_fn dispose = set->base.dispose_fn;
  gl_list_node_t node;

  for (node = set->root.next; node != &set->root; )
    {
      gl_list_node_t next = node->next;
      if (dispose != NULL)
        dispose (node->value);
      free (node);
      node = next;
    }
  free (set->table);
  free (set);
}

/* ---------------------- gl_set_iterator_t Data Type ---------------------- */

/* Iterate through the list, not through the hash buckets, so that the order
   in which the elements are returned is predictable.  */

static gl_set_iterator_t
gl_linkedhash_iterator (gl_set_t set)
{
  gl_set_iterator_t result;

  result.vtable = set->base.vtable;
  result.set = set;
  result.p = set->root.next;
  result.q = &set->root;
#if defined GCC_LINT || defined lint
  result.i = 0;
  result.j = 0;
  result.count = 0;
#endif

  return result;
}

static bool
gl_linkedhash_iterator_next (gl_set_iterator_t *iterator, const void **eltp)
{
  if (iterator->p != iterator->q)
    {
      gl_list_node_t node = (gl_list_node_t) iterator->p;
      *eltp = node->value;
      iterator->p = node->next;
      return true;
    }
  else
    return false;
}

static void
gl_linkedhash_iterator_free (gl_set_iterator_t *iterator)
{
}


const struct gl_set_implementation gl_linkedhash_set_implementation =
  {
    gl_linkedhash_nx_create_empty,
    gl_linkedhash_size,
    gl_linkedhash_search,
    gl_linkedhash_nx_add,
    gl_linkedhash_remove,
    gl_linkedhash_free,
    gl_linkedhash_iterator,
    gl_linkedhash_iterator_next,
    gl_linkedhash_iterator_free
  };
