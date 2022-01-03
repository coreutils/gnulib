/* Sequential list data type backed by another list.
   Copyright (C) 2006-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

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
#include "gl_sublist.h"

#include <stdint.h>
#include <stdlib.h>

/* -------------------------- gl_list_t Data Type -------------------------- */

/* Concrete gl_list_impl type, valid for this file only.  */
struct gl_list_impl
{
  struct gl_list_impl_base base;
  /* Reference to the whole list.  */
  gl_list_t whole;
  /* Limits of the index range.  */
  size_t start;
  size_t end;
};

/* struct gl_list_node_impl doesn't exist here.  The pointers are actually
   indices + 1.  (We don't use the whole list's gl_list_node_t implementation,
   because gl_sublist_next_node and gl_sublist_previous_node would not be easy
   to implement with this choice.)  */
#define INDEX_TO_NODE(index) (gl_list_node_t)(uintptr_t)(size_t)((index) + 1)
#define NODE_TO_INDEX(node) ((uintptr_t)(node) - 1)

static gl_list_t
gl_sublist_nx_create_empty (gl_list_implementation_t implementation,
                            gl_listelement_equals_fn equals_fn,
                            gl_listelement_hashcode_fn hashcode_fn,
                            gl_listelement_dispose_fn dispose_fn,
                            bool allow_duplicates)
{
  /* Shouldn't be called.  */
  abort ();
}

static gl_list_t
gl_sublist_nx_create_fill (gl_list_implementation_t implementation,
                           gl_listelement_equals_fn equals_fn,
                           gl_listelement_hashcode_fn hashcode_fn,
                           gl_listelement_dispose_fn dispose_fn,
                           bool allow_duplicates,
                           size_t count, const void **contents)
{
  /* Shouldn't be called.  */
  abort ();
}

static size_t
gl_sublist_size (gl_list_t list)
{
  return list->end - list->start;
}

static const void *
gl_sublist_node_value (gl_list_t list, gl_list_node_t node)
{
  uintptr_t index = NODE_TO_INDEX (node);
  if (!(index < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  return gl_list_get_at (list->whole, list->start + index);
}

static int
gl_sublist_node_nx_set_value (gl_list_t list, gl_list_node_t node, const void *elt)
{
  uintptr_t index = NODE_TO_INDEX (node);
  if (!(index < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  if (gl_list_nx_set_at (list->whole, list->start + index, elt) == NULL)
    return -1;
  return 0;
}

static gl_list_node_t
gl_sublist_next_node (gl_list_t list, gl_list_node_t node)
{
  uintptr_t index = NODE_TO_INDEX (node);
  size_t count = list->end - list->start;
  if (!(index < count))
    /* Invalid argument.  */
    abort ();
  index++;
  if (index < count)
    return INDEX_TO_NODE (index);
  else
    return NULL;
}

static gl_list_node_t
gl_sublist_previous_node (gl_list_t list, gl_list_node_t node)
{
  uintptr_t index = NODE_TO_INDEX (node);
  if (!(index < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  if (index > 0)
    return INDEX_TO_NODE (index - 1);
  else
    return NULL;
}

static gl_list_node_t
gl_sublist_first_node (gl_list_t list)
{
  if (list->end > list->start)
    return INDEX_TO_NODE (0);
  else
    return NULL;
}

static gl_list_node_t
gl_sublist_last_node (gl_list_t list)
{
  size_t count = list->end - list->start;
  if (count > 0)
    return INDEX_TO_NODE (count - 1);
  else
    return NULL;
}

static const void *
gl_sublist_get_at (gl_list_t list, size_t position)
{
  if (!(position < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  return gl_list_get_at (list->whole, list->start + position);
}

static gl_list_node_t
gl_sublist_nx_set_at (gl_list_t list, size_t position, const void *elt)
{
  if (!(position < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  if (gl_list_nx_set_at (list->whole, list->start + position, elt) == NULL)
    return NULL;
  return INDEX_TO_NODE (position);
}

static gl_list_node_t
gl_sublist_search_from_to (gl_list_t list, size_t start_index, size_t end_index,
                           const void *elt)
{
  if (!(start_index <= end_index && end_index <= list->end - list->start))
    /* Invalid arguments.  */
    abort ();
  {
    size_t index =
      gl_list_indexof_from_to (list->whole,
                               list->start + start_index,
                               list->start + end_index,
                               elt);
    if (index != (size_t)(-1))
      return INDEX_TO_NODE (index - list->start);
    else
      return NULL;
  }
}

static size_t
gl_sublist_indexof_from_to (gl_list_t list,
                            size_t start_index, size_t end_index,
                            const void *elt)
{
  if (!(start_index <= end_index && end_index <= list->end - list->start))
    /* Invalid arguments.  */
    abort ();
  {
    size_t index =
      gl_list_indexof_from_to (list->whole,
                               list->start + start_index,
                               list->start + end_index,
                               elt);
    if (index != (size_t)(-1))
      index -= list->start;
    return index;
  }
}

static gl_list_node_t
gl_sublist_nx_add_first (gl_list_t list, const void *elt)
{
  if (gl_list_nx_add_at (list->whole, list->start, elt) == NULL)
    return NULL;
  list->end++;
  return INDEX_TO_NODE (0);
}

static gl_list_node_t
gl_sublist_nx_add_last (gl_list_t list, const void *elt)
{
  if (gl_list_nx_add_at (list->whole, list->end, elt) == NULL)
    return NULL;
  list->end++;
  return INDEX_TO_NODE (list->end - list->start - 1);
}

static gl_list_node_t
gl_sublist_nx_add_before (gl_list_t list, gl_list_node_t node, const void *elt)
{
  size_t position = NODE_TO_INDEX (node);
  if (!(position < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  if (gl_list_nx_add_at (list->whole, list->start + position, elt) == NULL)
    return NULL;
  list->end++;
  return INDEX_TO_NODE (position);
}

static gl_list_node_t
gl_sublist_nx_add_after (gl_list_t list, gl_list_node_t node, const void *elt)
{
  size_t position = NODE_TO_INDEX (node);
  if (!(position < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  position++;
  if (gl_list_nx_add_at (list->whole, list->start + position, elt) == NULL)
    return NULL;
  list->end++;
  return INDEX_TO_NODE (position);
}

static gl_list_node_t
gl_sublist_nx_add_at (gl_list_t list, size_t position, const void *elt)
{
  if (!(position <= list->end - list->start))
    /* Invalid argument.  */
    abort ();
  if (gl_list_nx_add_at (list->whole, list->start + position, elt) == NULL)
    return NULL;
  list->end++;
  return INDEX_TO_NODE (position);
}

static bool
gl_sublist_remove_node (gl_list_t list, gl_list_node_t node)
{
  uintptr_t index = NODE_TO_INDEX (node);
  if (!(index < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  return gl_list_remove_at (list->whole, list->start + index);
}

static bool
gl_sublist_remove_at (gl_list_t list, size_t position)
{
  if (!(position < list->end - list->start))
    /* Invalid argument.  */
    abort ();
  return gl_list_remove_at (list->whole, list->start + position);
}

static bool
gl_sublist_remove (gl_list_t list, const void *elt)
{
  size_t position =
    gl_list_indexof_from_to (list->whole, list->start, list->end, elt);
  if (position == (size_t)(-1))
    return false;
  else
    return gl_list_remove_at (list->whole, position);
}

static void
gl_sublist_list_free (gl_list_t list)
{
  free (list);
}

/* --------------------- gl_list_iterator_t Data Type --------------------- */

static gl_list_iterator_t
gl_sublist_iterator (gl_list_t list)
{
  return gl_list_iterator_from_to (list->whole, list->start, list->end);
}

static gl_list_iterator_t
gl_sublist_iterator_from_to (gl_list_t list,
                             size_t start_index, size_t end_index)
{
  if (!(start_index <= end_index && end_index <= list->end - list->start))
    /* Invalid arguments.  */
    abort ();
  return gl_list_iterator_from_to (list->whole,
                                   list->start + start_index,
                                   list->start + end_index);
}

static bool
gl_sublist_iterator_next (gl_list_iterator_t *iterator,
                          const void **eltp, gl_list_node_t *nodep)
{
  /* Shouldn't be called.  */
  abort ();
}

static void
gl_sublist_iterator_free (gl_list_iterator_t *iterator)
{
  /* Shouldn't be called.  */
  abort ();
}

/* ---------------------- Sorted gl_list_t Data Type ---------------------- */

static gl_list_node_t
gl_sublist_sortedlist_search (gl_list_t list,
                              gl_listelement_compar_fn compar,
                              const void *elt)
{
  size_t index =
    gl_sortedlist_indexof_from_to (list->whole, compar,
                                   list->start, list->end, elt);
  if (index != (size_t)(-1))
    return INDEX_TO_NODE (index - list->start);
  else
    return NULL;
}

static gl_list_node_t
gl_sublist_sortedlist_search_from_to (gl_list_t list,
                                      gl_listelement_compar_fn compar,
                                      size_t low, size_t high,
                                      const void *elt)
{
  size_t index;

  if (!(low <= high && high <= list->end - list->start))
    /* Invalid arguments.  */
    abort ();

  index =
    gl_sortedlist_indexof_from_to (list->whole, compar,
                                   list->start + low, list->start + high, elt);
  if (index != (size_t)(-1))
    return INDEX_TO_NODE (index - list->start);
  else
    return NULL;
}

static size_t
gl_sublist_sortedlist_indexof (gl_list_t list,
                               gl_listelement_compar_fn compar,
                               const void *elt)
{
  size_t index =
    gl_sortedlist_indexof_from_to (list->whole, compar, list->start, list->end,
                                   elt);
  if (index != (size_t)(-1))
    index -= list->start;
  return index;
}

static size_t
gl_sublist_sortedlist_indexof_from_to (gl_list_t list,
                                       gl_listelement_compar_fn compar,
                                       size_t low, size_t high,
                                       const void *elt)
{
  size_t index;

  if (!(low <= high && high <= list->end - list->start))
    /* Invalid arguments.  */
    abort ();

  index = gl_sortedlist_indexof_from_to (list->whole, compar,
                                         list->start + low, list->start + high,
                                         elt);
  if (index != (size_t)(-1))
    index -= list->start;
  return index;
}

static gl_list_node_t
gl_sublist_sortedlist_nx_add (gl_list_t list,
                              gl_listelement_compar_fn compar,
                              const void *elt)
{
  /* It's impossible to implement this method without risking to put the
     whole list into unsorted order (namely, when the given ELT is smaller
     or larger than all elements of the sublist).  */
  abort ();
}

static bool
gl_sublist_sortedlist_remove (gl_list_t list,
                              gl_listelement_compar_fn compar,
                              const void *elt)
{
  size_t index = gl_sublist_sortedlist_indexof (list, compar, elt);
  if (index == (size_t)(-1))
    return false;
  else
    return gl_sublist_remove_at (list, index);
}


static const struct gl_list_implementation gl_sublist_list_implementation =
  {
    gl_sublist_nx_create_empty,
    gl_sublist_nx_create_fill,
    gl_sublist_size,
    gl_sublist_node_value,
    gl_sublist_node_nx_set_value,
    gl_sublist_next_node,
    gl_sublist_previous_node,
    gl_sublist_first_node,
    gl_sublist_last_node,
    gl_sublist_get_at,
    gl_sublist_nx_set_at,
    gl_sublist_search_from_to,
    gl_sublist_indexof_from_to,
    gl_sublist_nx_add_first,
    gl_sublist_nx_add_last,
    gl_sublist_nx_add_before,
    gl_sublist_nx_add_after,
    gl_sublist_nx_add_at,
    gl_sublist_remove_node,
    gl_sublist_remove_at,
    gl_sublist_remove,
    gl_sublist_list_free,
    gl_sublist_iterator,
    gl_sublist_iterator_from_to,
    gl_sublist_iterator_next,
    gl_sublist_iterator_free,
    gl_sublist_sortedlist_search,
    gl_sublist_sortedlist_search_from_to,
    gl_sublist_sortedlist_indexof,
    gl_sublist_sortedlist_indexof_from_to,
    gl_sublist_sortedlist_nx_add,
    gl_sublist_sortedlist_remove
  };

gl_list_t
gl_sublist_nx_create (gl_list_t whole_list, size_t start_index, size_t end_index)
{
  if (!(start_index <= end_index && end_index <= gl_list_size (whole_list)))
    /* Invalid arguments.  */
    abort ();
  {
    struct gl_list_impl *list =
      (struct gl_list_impl *) malloc (sizeof (struct gl_list_impl));

    if (list == NULL)
      return NULL;

    list->base.vtable = &gl_sublist_list_implementation;
    list->base.equals_fn = whole_list->base.equals_fn; /* actually unused */
    list->base.hashcode_fn = whole_list->base.hashcode_fn; /* actually unused */
    list->base.dispose_fn = whole_list->base.dispose_fn; /* actually unused */
    list->base.allow_duplicates = whole_list->base.allow_duplicates; /* unused */
    if (whole_list->base.vtable == &gl_sublist_list_implementation)
      {
        /* Optimization of a sublist of a sublist: Collapse the two
           indirections into a single indirection.  */
        list->whole = whole_list->whole;
        list->start = whole_list->start + start_index;
        list->end = whole_list->start + end_index;
      }
    else
      {
        list->whole = whole_list;
        list->start = start_index;
        list->end = end_index;
      }

    return list;
  }
}
