/* (Persistent) hash array mapped tries.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

/* Written by Marc Nieper-Wißkirchen <marc@nieper-wisskirchen.de>, 2021.  */

#include <config.h>
#define _GL_HAMT_INLINE _GL_EXTERN_INLINE
#include "hamt.h"

#include <flexmember.h>
#include <inttypes.h>
#include <stdlib.h>
#include "count-one-bits.h"
#include "verify.h"
#include "xalloc.h"

/* Reference counters can be shared between different threads if the
   entry they belong to is shared between different threads.
   Operations on them therefore have to be atomic so that no invalid
   state is observable.

   A thread must not modify an entry or its children (!) if its
   reference count implies that the entry is shared by at least two
   hamts.  */
typedef
#if GL_HAMT_THREAD_SAFE
_Atomic
#endif
size_t ref_counter;

/***************/
/* Entry Types */
/***************/

/* Leaf nodes are of type element.  Non-leaf nodes are either subtries
   or, if at maximal depth, buckets.  The entry type is stored in the
   lower two bits of the reference counter, whereas reference counters
   for entries are incremented and decremented in multiples of 4.  */
enum entry_type
{
  element_entry = 0,
  subtrie_entry = 1,
  bucket_entry = 2
};

/* Return the type an entry.  */
static enum entry_type
entry_type (const Hamt_entry *entry)
{
  return entry->ref_count & 3;
}

/********************/
/* Reference Counts */
/********************/

/* Initialize the reference counter, storing its type.  */
static void
init_ref_counter (ref_counter *counter, enum entry_type type)
{
  *counter = 4 + type;
}

/* Increase the reference counter of an entry.  */
static void
inc_ref_counter (ref_counter *counter)
{
  *counter += 4;
}

/* Decrease the entry reference counter.  Return false if the entry
   can be deleted.  */
static bool
dec_ref_counter (ref_counter *counter)
{
  *counter -= 4;
  return *counter >= 4;
}

/**************/
/* Structures */
/**************/

/* Different generations of a hamt share a function table.  */
struct function_table
{
  Hamt_hasher *hasher;
  Hamt_comparator *comparator;
  Hamt_freer *freer;
  ref_counter ref_count;
};

/* Different generations of a hamt share subtries.  A singleton
   subtrie is modelled as a single element.  */
struct subtrie
{
  ref_counter ref_count;
  /* Nodes carry labels from 0 to 31.  The i-th bit in MAP is set if
     the node labelled i is present.  */
  uint32_t map;
  /* The length of the NODES array is the population count of MAP.
     The order of the nodes corresponds to the order of the 1-bits in
     MAP.  */
  Hamt_entry *nodes[FLEXIBLE_ARRAY_MEMBER];
};

/* Buckets are used when different elements have the same hash values.  */
struct bucket
{
  ref_counter ref_counter;
  size_t elt_count;
  Hamt_entry *elts[FLEXIBLE_ARRAY_MEMBER];
};

/* A hamt consists of its function table and the root entry.  */
struct hamt
{
  struct function_table *functions;
  /* The root entry is NULL for an empty HAMT.  */
  Hamt_entry *root;
};

/*******************/
/* Function Tables */
/*******************/

/* Allocate and initialize a function table.  */
static struct function_table *
create_function_table (Hamt_hasher *hasher, Hamt_comparator *comparator,
                       Hamt_freer *freer)
{
  struct function_table *functions = XMALLOC (struct function_table);
  functions->hasher = hasher;
  functions->comparator = comparator;
  functions->freer = freer;
  functions->ref_count = 1;
  return functions;
}

/* Increment the reference count and return the function table. */
static struct function_table *
copy_function_table (struct function_table *function_table)
{
  ++function_table->ref_count;
  return function_table;
}

/* Decrease the reference count and free the function table if the
   reference count drops to zero.  */
static void
free_function_table (struct function_table *function_table)
{
  if (--function_table->ref_count)
    return;
  free (function_table);
}

/************/
/* Elements */
/************/

/* Return an element's hash.  */
static size_t
hash_element (const struct function_table *functions, const Hamt_entry *elt)
{
  return functions->hasher (elt);
}

/* Compare two elements.  */
static bool
compare_elements (const struct function_table *functions,
                  const Hamt_entry *elt1, const Hamt_entry *elt2)
{
  return functions->comparator (elt1, elt2);
}

/* Free an element.  */
static void
free_element (const struct function_table *functions, Hamt_entry *elt)
{
  if (dec_ref_counter (&elt->ref_count))
    return;
  functions->freer (elt);
}

/* Return the initialized element.  */
static Hamt_entry *
init_element (Hamt_entry *elt)
{
  init_ref_counter (&elt->ref_count, element_entry);
  return elt;
}

/***********/
/* Buckets */
/***********/

/* Allocate a partially initialized bucket with a given number of elements.  */
static struct bucket *
alloc_bucket (size_t elt_count)
{
  struct bucket *bucket
    = xmalloc (FLEXSIZEOF (struct bucket, elts,
                           sizeof (Hamt_entry) * elt_count));
  init_ref_counter (&bucket->ref_counter, bucket_entry);
  bucket->elt_count = elt_count;
  return bucket;
}

/***********/
/* Entries */
/***********/

/* Return true if the entry is shared between two or more hamts.
   Otherwise, return false.

   This procedure is used for destructive updates.  If an entry and
   all its parents are not shared, it can be updated destructively
   without effecting other hamts.  */
static bool
is_shared (const Hamt_entry *entry)
{
  return entry->ref_count >= 8;
}

/* Calculate and return the number of nodes in a subtrie.  */
static int
trienode_count (const struct subtrie *subtrie)
{
  return count_one_bits (subtrie->map); /* In Gnulib, we assume that
                                           an integer has at least 32
                                           bits. */
}

/* Allocate a partially initialized subtrie with a given number of nodes.  */
static struct subtrie *
alloc_subtrie (int node_count)
{
  struct subtrie *subtrie
    = xmalloc (FLEXSIZEOF (struct subtrie, nodes,
                           sizeof (Hamt_entry) * node_count));
  init_ref_counter (&subtrie->ref_count, subtrie_entry);
  return subtrie;
}

/* Return a conceptually copy of an entry.  */
static Hamt_entry *
copy_entry (Hamt_entry *entry)
{
  inc_ref_counter (&entry->ref_count);
  return entry;
}

/* Return a new bucket that has the j-th element replaced.  */
static struct bucket *
replace_bucket_element (struct bucket *bucket, int j, Hamt_entry *elt)
{
  int n = bucket->elt_count;
  struct bucket *new_bucket = alloc_bucket (n);
  for (int k = 0; k < n; ++k)
    if (k == j)
      new_bucket->elts[k] = elt;
    else
      new_bucket->elts[k] = copy_entry (bucket->elts[k]);
  return new_bucket;
}

/* Return a new subtrie that has the j-th node replaced.  */
static struct subtrie *
replace_entry (struct subtrie *subtrie, int j, Hamt_entry *entry)
{
  int n = trienode_count (subtrie);
  struct subtrie *new_subtrie = alloc_subtrie (n);
  new_subtrie->map = subtrie->map;
  for (int k = 0; k < n; ++k)
    if (k == j)
      new_subtrie->nodes[k] = entry;
    else
      new_subtrie->nodes[k] = copy_entry (subtrie->nodes[k]);
  return new_subtrie;
}

/* Return a new subtrie that has an entry labelled i inserted at
   the j-th position.  */
static struct subtrie *
insert_entry (struct subtrie *subtrie, int i, int j, Hamt_entry *entry)
{
  int n = trienode_count (subtrie) + 1;
  struct subtrie *new_subtrie = alloc_subtrie (n);
  new_subtrie->map = subtrie->map | (1 << i);
  for (int k = 0; k < n; ++k)
    {
      if (k < j)
        new_subtrie->nodes[k] = copy_entry (subtrie->nodes[k]);
      else if (k > j)
        new_subtrie->nodes[k] = copy_entry (subtrie->nodes[k - 1]);
      else
        new_subtrie->nodes[k] = entry;
    }
  return new_subtrie;
}

/* Return a new entry that has the entry labelled i removed from
   position j.  */
static Hamt_entry *
remove_subtrie_entry (struct subtrie *subtrie, int i, int j)
{
  int n = trienode_count (subtrie) - 1;
  if (n == 1)
    {
      if (j == 0)
        return copy_entry (subtrie->nodes[1]);
      return copy_entry (subtrie->nodes[0]);
    }
  struct subtrie *new_subtrie = alloc_subtrie (n);
  new_subtrie->map = subtrie->map & ~(1 << i);
  for (int k = 0; k < n; ++k)
    {
      if (k < j)
        new_subtrie->nodes[k] = copy_entry (subtrie->nodes[k]);
      else if (k >= j)
        new_subtrie->nodes[k] = copy_entry (subtrie->nodes[k + 1]);
    }
  return (Hamt_entry *) new_subtrie;
}

/* Return a new entry that has the entry at position j removed.  */
static Hamt_entry *
remove_bucket_entry (struct bucket *bucket, int j)
{
  int n = bucket->elt_count - 1;
  if (n == 1)
    {
      if (j == 0)
        return copy_entry (bucket->elts[1]);
      return copy_entry (bucket->elts[0]);
    }
  struct bucket *new_bucket = alloc_bucket (n);
  for (int k = 0; k < n; ++k)
    {
      if (k < j)
        new_bucket->elts[k] = copy_entry (bucket->elts[k]);
      else if (k >= j)
        new_bucket->elts[k] = copy_entry (bucket->elts[k + 1]);
    }
  return (Hamt_entry *) new_bucket;
}

/****************************/
/* Creation and Destruction */
/****************************/

/* Create a new, empty hash array mapped trie.  */
Hamt *
hamt_create (Hamt_hasher *hasher, Hamt_comparator *comparator,
             Hamt_freer *freer)
{
  struct function_table *functions
    = create_function_table (hasher, comparator, freer);
  Hamt *hamt = XMALLOC (Hamt);
  hamt->functions = functions;
  hamt->root = NULL;
  return hamt;
}

/* Return a copy of the hamt.  */
Hamt *
hamt_copy (Hamt *hamt)
{
  Hamt *new_hamt = XMALLOC (Hamt);
  new_hamt->functions = copy_function_table (hamt->functions);
  new_hamt->root = hamt->root == NULL ? NULL : copy_entry (hamt->root);
  return new_hamt;
}

/* Free a bucket.  */
static void
free_bucket (struct function_table const *functions, struct bucket *bucket)
{
  if (dec_ref_counter (&bucket->ref_counter))
    return;
  size_t elt_count = bucket->elt_count;
  Hamt_entry *const *elts = bucket->elts;
  for (size_t i = 0; i < elt_count; ++i)
    free_element (functions, elts[i]);
  free (bucket);
}

/* Forward declaration.  */
static void free_subtrie (struct function_table const *functions,
                          struct subtrie *subtrie);

/* Free an entry.  */
static void
free_entry (struct function_table const *functions, Hamt_entry *entry)
{
  switch (entry_type (entry))
    {
    case element_entry:
      free_element (functions, entry);
      break;
    case subtrie_entry:
      free_subtrie (functions, (struct subtrie *) entry);
      break;
    case bucket_entry:
      free_bucket (functions, (struct bucket *) entry);
      break;
    default:
      assume (0);
    }
}

/* Free a trie recursively.  */
static void
free_subtrie (struct function_table const *functions, struct subtrie *subtrie)
{
  if (dec_ref_counter (&subtrie->ref_count))
    return;
  int n = trienode_count (subtrie);
  Hamt_entry **node_ptr = subtrie->nodes;
  for (int j = 0; j < n; ++j)
    free_entry (functions, *node_ptr++);
  free (subtrie);
}

/* Free a hamt.  */
void
hamt_free (Hamt *hamt)
{
  if (hamt->root != NULL)
    free_entry (hamt->functions, hamt->root);
  free_function_table (hamt->functions);
  free (hamt);
}

/**********/
/* Lookup */
/**********/

/* Lookup an element in a bucket.  */
static Hamt_entry *
bucket_lookup (const struct function_table *functions,
               const struct bucket *bucket, const void *elt)
{
  size_t elt_count = bucket->elt_count;
  Hamt_entry *const *elts = bucket->elts;
  for (size_t i = 0; i < elt_count; ++i)
    {
      if (compare_elements (functions, elt, elts[i]))
        return *elts;
    }
  return NULL;
}

/* Forward declaration.  */
static Hamt_entry *entry_lookup (const struct function_table *functions,
                                 Hamt_entry *entry,
                                 const void *elt, size_t hash);

/* Lookup an element in a bucket.  */
static Hamt_entry *
subtrie_lookup (const struct function_table *functions,
                const struct subtrie *subtrie, const void *elt,
                size_t hash)
{
  uint32_t map = subtrie->map;
  int i = hash & 31;

  if (! (map & (1 << i)))
    return NULL;

  int j = i == 0 ? 0 : count_one_bits (map << (32 - i));
  return entry_lookup (functions, subtrie->nodes[j], elt, hash >> 5);
}

/* Lookup an element in an entry.  */
static Hamt_entry *
entry_lookup (const struct function_table *functions, Hamt_entry *entry,
              const void *elt, size_t hash)
{
  switch (entry_type (entry))
    {
    case element_entry:
      if (compare_elements (functions, elt, entry))
        return entry;
      return NULL;
    case subtrie_entry:
      return subtrie_lookup (functions, (struct subtrie *) entry, elt, hash);
    case bucket_entry:
      return bucket_lookup (functions, (struct bucket *) entry, elt);
    default:
      assume (0);
    }
}

/* If ELT matches an entry in HAMT, return this entry.  Otherwise,
   return NULL.  */
Hamt_entry *
hamt_lookup (const Hamt *hamt, const void *elt)
{
  if (hamt->root == NULL)
    return NULL;

  return entry_lookup (hamt->functions, hamt->root, elt,
                       hash_element (hamt->functions, elt));
}

/**************************/
/* Insertion and Deletion */
/**************************/

/* Create a bucket populated with two elements.  */
static struct bucket *
create_populated_bucket (Hamt_entry *elt1, Hamt_entry *elt2)
{
  struct bucket *bucket = alloc_bucket (2);
  bucket->elts[0] = elt1;
  bucket->elts[1] = elt2;
  return bucket;
}

/* Create a chain of subtrie nodes so that the resulting trie is
   populated with exactly two elements.  */
static Hamt_entry *
create_populated_subtrie (Hamt_entry *elt1, Hamt_entry *elt2, size_t hash1,
                          size_t hash2, int depth)
{
  if (depth >= _GL_HAMT_MAX_DEPTH)
    return (Hamt_entry *) create_populated_bucket (elt1, elt2);

  struct subtrie *subtrie;
  int i1 = hash1 & 31;
  int i2 = hash2 & 31;
  if (i1 != i2)
    {
      subtrie = alloc_subtrie (2);
      subtrie->map = (1 << i1) | (1 << i2);
      if (i1 < i2)
        {
          subtrie->nodes[0] = elt1;
          subtrie->nodes[1] = elt2;
        }
      else
        {
          subtrie->nodes[0] = elt2;
          subtrie->nodes[1] = elt1;
        }
    }
  else
    {
      subtrie = alloc_subtrie (1);
      subtrie->map = 1 << i1;
      subtrie->nodes[0]
        = create_populated_subtrie (elt1, elt2, hash1 >> 5, hash2 >> 5,
                                    depth + 1);
    }
  return (Hamt_entry *) subtrie;
}

/* Insert or replace an element in a bucket.  */
static struct bucket *
bucket_insert (const struct function_table *functions, struct bucket *bucket,
               Hamt_entry **elt_ptr, bool replace, bool shared)
{
  size_t elt_count = bucket->elt_count;
  Hamt_entry **elts = bucket->elts;
  for (size_t i = 0; i < elt_count; ++i)
    {
      if (compare_elements (functions, *elt_ptr, elts[i]))
        {
          if (replace)
            {
              if (shared)
                {
                  struct bucket *new_bucket
                    = replace_bucket_element (bucket, i,
                                              copy_entry (*elt_ptr));
                  *elt_ptr = elts[i];
                  return new_bucket;
                }
              free_element (functions, elts[i]);
              elts[i] = copy_entry (*elt_ptr);
              return bucket;
            }
          *elt_ptr = *elt_ptr == elts[i] ? NULL : elts[i];
          return bucket;
        }
    }
  struct bucket *new_bucket = alloc_bucket (elt_count + 1);
  new_bucket->elts[0] = copy_entry (*elt_ptr);
  for (size_t i = 0; i < elt_count; ++i)
    {
      new_bucket->elts[i + 1] = copy_entry (bucket->elts[i]);
    }
  if (replace)
    *elt_ptr = NULL;
  return new_bucket;
}

/* Forward declaration.  */
static Hamt_entry *entry_insert (const struct function_table *functions,
                                 Hamt_entry *subtrie, Hamt_entry **elt_ptr,
                                 size_t hash, int depth, bool replace,
                                 bool shared);

/* Insert or replace an element in a subtrie.  */
static struct subtrie *
subtrie_insert (const struct function_table *functions, struct subtrie *subtrie,
                Hamt_entry **elt_ptr, size_t hash, int depth, bool replace,
                bool shared)
{
  uint32_t map = subtrie->map;
  int i = hash & 31;
  int j = i == 0 ? 0 : count_one_bits (map << (32 - i));
  if (map & (1 << i))
    {
      Hamt_entry *entry = subtrie->nodes[j];
      Hamt_entry *new_entry
        = entry_insert (functions, entry, elt_ptr, hash >> 5, depth + 1,
                        replace, shared);
      if (new_entry != entry)
        {
          if (shared)
            return replace_entry (subtrie, j, new_entry);
          free_entry (functions, entry);
          subtrie->nodes[j] = new_entry;
        }
      return subtrie;
    }
  Hamt_entry *entry = copy_entry (*elt_ptr);
  if (replace)
    *elt_ptr = NULL;
  return insert_entry (subtrie, i, j, entry);
}

/* Insert or replace an element in an entry.

   REPLACE is true if we want replace instead of insert semantics.
   SHARED is false if a destructive update has been requested and none
   of the parent nodes are shared.  If an entry cannot be inserted
   because the same entry with respect to pointer equality is already
   present, *ELT_PTR is set to NULL to mark this special case.  */
static Hamt_entry *
entry_insert (const struct function_table *functions, Hamt_entry *entry,
              Hamt_entry **elt_ptr, size_t hash, int depth, bool replace,
              bool shared)
{
  shared |= is_shared (entry);
  switch (entry_type (entry))
    {
    case element_entry:
      if (compare_elements (functions, *elt_ptr, entry))
        {
          if (replace)
            {
              if (shared)
                {
                  Hamt_entry *new_entry = copy_entry (*elt_ptr);
                  *elt_ptr = entry;
                  return new_entry;
                }
              return copy_entry (*elt_ptr);
            }
          *elt_ptr = *elt_ptr == entry ? NULL : entry;
          return entry;
        }
      Hamt_entry *new_entry = copy_entry (*elt_ptr);
      if (replace)
        *elt_ptr = NULL;
      /* We have to take this shortcut as shifting an integer of N
        bits by N or more bits triggers undefined behavior.
        See: https://lists.gnu.org/archive/html/bug-gnulib/2022-04/msg00023.html.  */
      if (depth >= _GL_HAMT_MAX_DEPTH)
       return (Hamt_entry *) create_populated_bucket (new_entry, copy_entry (entry));
      return create_populated_subtrie (new_entry, copy_entry (entry), hash,
                                       (hash_element (functions, entry)
                                        >> (5 * depth)), depth);
    case subtrie_entry:
      return (Hamt_entry *)
        subtrie_insert (functions, (struct subtrie *) entry, elt_ptr, hash,
                        depth, replace, shared);
    case bucket_entry:
      return (Hamt_entry *)
        bucket_insert (functions, (struct bucket *) entry, elt_ptr, replace,
                       shared);
    default:
      assume (0);
    }
}

/* Insert or replace an element in the root.  */
static Hamt_entry *
root_insert (const struct function_table *functions, Hamt_entry *root,
             Hamt_entry **elt_ptr, bool replace, bool shared)
{
  if (root == NULL)
    return copy_entry (*elt_ptr);

 return entry_insert (functions, root, elt_ptr,
                      hash_element (functions, *elt_ptr), 0, replace, shared);
}

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
   element from the table and return HAMT.  Otherwise, insert *ELT_PTR
   into a copy of the HAMT and return the copy.  */
Hamt *
hamt_insert (Hamt *hamt, Hamt_entry **elt_ptr)
{
  Hamt_entry *elt = *elt_ptr;
  Hamt_entry *new_entry = root_insert (hamt->functions, hamt->root,
                                       elt_ptr, false, true);
  if (*elt_ptr == NULL)
    *elt_ptr = elt;

  if (new_entry == hamt->root)
    return hamt;

  Hamt *new_hamt = XMALLOC (Hamt);
  new_hamt->functions = copy_function_table (hamt->functions);
  new_hamt->root = new_entry;
  return new_hamt;
}

/* Insert *ELT_PTR into a copy of HAMT and return the copy.  If an
   existing element was replaced, set *ELT_PTR to this element, and to
   NULL otherwise. */
Hamt *
hamt_replace (Hamt *hamt, Hamt_entry **elt_ptr)
{
  Hamt *new_hamt = XMALLOC (Hamt);
  new_hamt->functions = copy_function_table (hamt->functions);
  new_hamt->root = root_insert (hamt->functions, hamt->root, elt_ptr, true,
                                true);
  return new_hamt;
}

/* Remove an element in a bucket if found.  */
static Hamt_entry *
bucket_remove (const struct function_table *functions, struct bucket *bucket,
               Hamt_entry **elt_ptr)
{
  size_t elt_count = bucket->elt_count;
  Hamt_entry *const *elts = bucket->elts;
  for (size_t i = 0; i < elt_count; ++i)
    {
      if (compare_elements (functions, *elt_ptr, elts[i]))
        {
          *elt_ptr = elts[i];
          return remove_bucket_entry (bucket, i);
        }
    }
  *elt_ptr = NULL;
  return (Hamt_entry *) bucket;
}

/* Forward declaration.  */
static Hamt_entry *entry_remove (const struct function_table *functions,
                                 Hamt_entry *entry, Hamt_entry **elt_ptr,
                                 size_t hash, int depth, bool shared);

/* Remove an element in a subtrie if found.  */
static Hamt_entry *
subtrie_remove (const struct function_table *functions, struct subtrie *subtrie,
                Hamt_entry **elt_ptr, size_t hash, int depth, bool shared)
{
  uint32_t map = subtrie->map;
  int i = hash & 31;
  int j = i == 0 ? 0 : count_one_bits (map << (32 - i));
  if (map & (1 << i))
    {
      Hamt_entry *entry = subtrie->nodes[j];
      Hamt_entry *new_entry
        = entry_remove (functions, entry, elt_ptr, hash >> 5, depth + 1,
                        shared);
      if (new_entry == NULL)
        return remove_subtrie_entry (subtrie, i, j);
      if (new_entry != entry)
        {
          if (shared)
            return (Hamt_entry *) replace_entry (subtrie, j, new_entry);
          free_entry (functions, entry);
          subtrie->nodes[j] = new_entry;
        }
      return (Hamt_entry *) subtrie;
    }
  *elt_ptr = NULL;
  return (Hamt_entry *) subtrie;
}

/* Remove an element in an entry if found.

   SHARED is false if a destructive update has been requested and none
   of the parent nodes are shared.  If an entry cannot be
   removed, *ELT_PTR is set to NULL.  */
static Hamt_entry *
entry_remove (const struct function_table *functions, Hamt_entry *entry,
              Hamt_entry **elt_ptr, size_t hash, int depth, bool shared)
{
  shared |= is_shared (entry);
  switch (entry_type (entry))
    {
    case element_entry:
      if (compare_elements (functions, *elt_ptr, entry))
        {
          *elt_ptr = entry;
          return NULL;
        }
      *elt_ptr = NULL;
      return entry;
    case subtrie_entry:
      return subtrie_remove (functions, (struct subtrie *) entry, elt_ptr, hash,
                             depth, shared);
    case bucket_entry:
      return bucket_remove (functions, (struct bucket *) entry, elt_ptr);
    default:
      assume (0);
    }
}

/* Remove an element in the root.  */
static Hamt_entry *
root_remove (const struct function_table *functions, Hamt_entry *root,
             Hamt_entry **elt_ptr, bool shared)
{
  if (root == NULL)
    return NULL;

  return entry_remove (functions, root, elt_ptr,
                       hash_element (functions, *elt_ptr), 0, shared);
}

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
element from the table, remove the element from a copy of the hamt and
return the copy.  Otherwise, return HAMT.  */
Hamt *
hamt_remove (Hamt *hamt, Hamt_entry **elt_ptr)
{
  Hamt_entry *elt = *elt_ptr;
  Hamt_entry *new_entry = root_remove (hamt->functions, hamt->root, elt_ptr,
                                       true);
  if (*elt_ptr == NULL)
    *elt_ptr = elt;

  if (new_entry == hamt->root)
    return hamt;

  Hamt *new_hamt = XMALLOC (Hamt);
  new_hamt->functions = copy_function_table (hamt->functions);
  new_hamt->root = new_entry;
  return new_hamt;
}

/*************/
/* Iteration */
/*************/

/* Walk a bucket.  */
static size_t
bucket_do_while (const struct bucket *bucket, Hamt_processor *proc, void *data,
                 bool *success)
{
  size_t cnt = 0;
  size_t elt_count = bucket->elt_count;
  Hamt_entry *const *elts = bucket->elts;
  for (size_t i = 0; i < elt_count; ++i)
    {
      *success = proc (elts[i], data);
      if (*success == false)
        return cnt;
      ++cnt;
    }
  return cnt;
}

/* Forward declaration.  */
static size_t entry_do_while (Hamt_entry *entry, Hamt_processor *proc,
                              void *data, bool *success);

/* Walk a subtrie.  */
static size_t subtrie_do_while (const struct subtrie *subtrie,
                                Hamt_processor *proc, void *data, bool *success)
{
  size_t cnt = 0;
  int n = trienode_count (subtrie);
  Hamt_entry *const *node_ptr = subtrie->nodes;
  for (int j = 0; j < n; ++j)
    {
      cnt += entry_do_while (*node_ptr++, proc, data, success);
      if (!success)
        return cnt;
    }
  return cnt;
}

/* Walk an entry.  */
static size_t
entry_do_while (Hamt_entry *entry, Hamt_processor *proc, void *data,
                bool *success)
{
  switch (entry_type (entry))
    {
    case element_entry:
      *success = proc (entry, data);
      return *success ? 1 : 0;
    case subtrie_entry:
      return subtrie_do_while ((struct subtrie *) entry, proc, data, success);
    case bucket_entry:
      return bucket_do_while ((struct bucket *) entry, proc, data, success);
    default:
      assume (0);
    }
}

/* Call PROC for every entry of the hamt until it returns false.  The
   first argument of PROC is the entry, the second argument is the value
   of DATA as received.  Return the number of calls that returned
   true.  */
size_t
hamt_do_while (const Hamt *hamt, Hamt_processor *proc, void *data)
{
  if (hamt->root == NULL)
    return 0;

  bool success = true;
  return entry_do_while (hamt->root, proc, data, &success);
}

/* Create an iterator with a copy of the hamt.

   For a valid iterator state the following is true: If DEPTH is
   negative, the iterator is exhausted.  Otherwise, ENTRY[DEPTH] is
   either the element that is produced next, or a bucket such that the
   element at POSITION of the bucket is produced next.
*/
Hamt_iterator
hamt_iterator (Hamt *hamt)
{
  Hamt_iterator iter;
  iter.hamt = hamt_copy (hamt);
  Hamt_entry *entry = hamt->root;
  iter.path = 0;
  iter.position = 0;
  if (entry == NULL)
    {
      iter.depth = -1;
      return iter;
    }
  iter.depth = 0;
  while (iter.entry[iter.depth] = entry, entry_type (entry) == subtrie_entry)
    {
      const struct subtrie *subtrie = (const struct subtrie *) entry;
      ++iter.depth;
      entry = subtrie->nodes[0];
    }
  return iter;
}

/* Free the iterator and the associated hamt copy.  */
void
hamt_iterator_free (Hamt_iterator *iter)
{
  hamt_free (iter->hamt);
}

/* Create a copy of the complete iterator state, including the
   hamt.  */
Hamt_iterator
hamt_iterator_copy (Hamt_iterator *iter)
{
  Hamt_iterator new_iter = *iter;
  new_iter.hamt = hamt_copy (iter->hamt);
  return new_iter;
}

/* Return the number of significant bits at DEPTH.  */
static int
bit_width (int depth)
{
  if (depth < _GL_HAMT_MAX_DEPTH - 1)
    return 5;
  return SIZE_WIDTH - 5 * (_GL_HAMT_MAX_DEPTH - 1);
}

/* The actual iteration.  */
bool
hamt_iterator_next (Hamt_iterator *iter, Hamt_entry **elt_ptr)
{
  int depth = iter->depth;
  if (depth < 0)
    return false;

  Hamt_entry *entry = iter->entry[depth];
  if (entry_type (entry) == bucket_entry)
    {
      struct bucket *bucket = (struct bucket *) entry;
      *elt_ptr = bucket->elts[iter->position];
      if (++iter->position < bucket->elt_count)
        return true;
    }
  else
    *elt_ptr = entry;

  struct subtrie *subtrie;
  while (iter->depth-- > 0)
    {
      int width = bit_width (iter->depth);
      int j = (iter->path & ((1 << width) - 1)) + 1;
      subtrie = (struct subtrie *) iter->entry[iter->depth];
      if (j < trienode_count (subtrie))
        {
          ++iter->path;
          while (iter->entry[++iter->depth] = subtrie->nodes[j],
                 entry_type (iter->entry[iter->depth]) == subtrie_entry)
            {
              width = bit_width (iter->depth);
              iter->path <<= width;
              j = 0;
              subtrie = (struct subtrie *) iter->entry[iter->depth];
            }
          iter->position = 0;
          break;
        }
      iter->path >>= width;
    }

  return true;
}

/***********************/
/* Destructive Updates */
/***********************/

/* If *ELT_PTR matches an element already in HAMT, set *ELT_PTR to the
   element from the table and return false.  Otherwise, insert *ELT_PTR
   destructively into the hamt and return true.  */
bool
hamt_insert_x (Hamt *hamt, Hamt_entry **elt_ptr)
{
  Hamt_entry *elt = *elt_ptr;
  Hamt_entry *old_root = hamt->root;
  hamt->root = root_insert (hamt->functions, old_root, elt_ptr, false, false);
  if (old_root != hamt->root && old_root != NULL)
    free_entry (hamt->functions, old_root);
  if (*elt_ptr == NULL)
    {
      *elt_ptr = elt;
      return false;
    }
  return *elt_ptr == elt;
}

/* Insert ELT destructively into HAMT.  If an existing element was
   replaced, return true.  Otherwise, return false.  */
bool
hamt_replace_x (Hamt *hamt, Hamt_entry *elt)
{
  Hamt_entry *old_root = hamt->root;
  hamt->root = root_insert (hamt->functions, old_root, &elt, true, false);
  if (old_root != hamt->root && old_root != NULL)
    free_entry (hamt->functions, old_root);
  return elt != NULL;
}

/* If ELT matches an element already in HAMT, remove the element
   destructively from the hamt and return true.  Otherwise, return
   false.  */
bool
hamt_remove_x (Hamt *hamt, Hamt_entry *elt)
{
  Hamt_entry *old_root = hamt->root;
  hamt->root = root_remove (hamt->functions, old_root, &elt, false);
  if (old_root != hamt->root)
    free_entry (hamt->functions, old_root);
  return elt != NULL;
}
