/* Abstract ordered map data type.
   Copyright (C) 2006-2007, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2018.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _GL_OMAP_H
#define _GL_OMAP_H

#include <stdbool.h>
#include <stddef.h>

#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
#endif
_GL_INLINE_HEADER_BEGIN
#ifndef GL_OMAP_INLINE
# define GL_OMAP_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* gl_omap is an abstract ordered map data type.  It can contain any number
   of (key, value) pairs, where
     - keys and values are objects ('void *' or 'const void *' pointers),
     - The (key, value) pairs are ordered by the key, in the order of a given
       comparator function.
     - There are no (key, value1) and (key, value2) pairs with the same key
       (in the sense of the comparator function).

   There are several implementations of this ordered map datatype, optimized
   for different operations or for memory.  You can start using the simplest
   ordered map implementation, GL_ARRAY_OMAP, and switch to a different
   implementation later, when you realize which operations are performed
   the most frequently.  The API of the different implementations is exactly
   the same; when switching to a different implementation, you only have to
   change the gl_omap_create call.

   The implementations are:
     GL_ARRAY_OMAP        a growable array
     GL_AVLTREE_OMAP      a binary tree (AVL tree)
     GL_RBTREE_OMAP       a binary tree (red-black tree)

   The memory consumption is asymptotically the same: O(1) for every pair
   in the map.  When looking more closely at the average memory consumed
   for an pair, GL_ARRAY_OMAP is the most compact representation, and
   GL_AVLTREE_OMAP, GL_RBTREE_OMAP need more memory.

   The guaranteed average performance of the operations is, for a map of
   n pairs:

   Operation                  ARRAY     TREE

   gl_omap_size                O(1)     O(1)
   gl_omap_get               O(log n) O(log n)
   gl_omap_put                 O(n)   O(log n)
   gl_omap_remove              O(n)   O(log n)
   gl_omap_search            O(log n) O(log n)
   gl_omap_search_atleast    O(log n) O(log n)
   gl_omap_iterator            O(1)   O(log n)
   gl_omap_iterator_next       O(1)   O(log n)
 */

/* -------------------------- gl_omap_t Data Type -------------------------- */

/* Type of function used to compare two keys.  Same as for qsort().
   NULL denotes pointer comparison.  */
typedef int (*gl_mapkey_compar_fn) (const void *key1, const void *key2);

#ifndef _GL_MAP_DISPOSE_FNS_DEFINED

/* Type of function used to dispose a key once a (key, value) pair is removed
   from a map.  NULL denotes a no-op.  */
typedef void (*gl_mapkey_dispose_fn) (const void *key);

/* Type of function used to dispose a value once a (key, value) pair is removed
   from a map.  NULL denotes a no-op.  */
typedef void (*gl_mapvalue_dispose_fn) (const void *value);

# define _GL_MAP_DISPOSE_FNS_DEFINED 1
#endif

/* Type of function used to compare a key with a threshold.
   Returns true if the key is greater or equal than the threshold.  */
typedef bool (*gl_mapkey_threshold_fn) (const void *key, const void *threshold);

struct gl_omap_impl;
/* Type representing an entire ordered map.  */
typedef struct gl_omap_impl * gl_omap_t;

struct gl_omap_implementation;
/* Type representing a ordered map datatype implementation.  */
typedef const struct gl_omap_implementation * gl_omap_implementation_t;

#if 0 /* Unless otherwise specified, these are defined inline below.  */

/* Creates an empty map.
   IMPLEMENTATION is one of GL_ARRAY_OMAP, GL_AVLTREE_OMAP, GL_RBTREE_OMAP.
   COMPAR_FN is a key comparison function or NULL.
   KDISPOSE_FN is a key disposal function or NULL.
   VDISPOSE_FN is a value disposal function or NULL.  */
/* declared in gl_xomap.h */
extern gl_omap_t gl_omap_create_empty (gl_omap_implementation_t implementation,
                                       gl_mapkey_compar_fn compar_fn,
                                       gl_mapkey_dispose_fn kdispose_fn,
                                       gl_mapvalue_dispose_fn vdispose_fn)
  /*_GL_ATTRIBUTE_DEALLOC (gl_omap_free, 1)*/
  _GL_ATTRIBUTE_RETURNS_NONNULL;
/* Likewise.  Returns NULL upon out-of-memory.  */
extern gl_omap_t gl_omap_nx_create_empty (gl_omap_implementation_t implementation,
                                          gl_mapkey_compar_fn compar_fn,
                                          gl_mapkey_dispose_fn kdispose_fn,
                                          gl_mapvalue_dispose_fn vdispose_fn)
  /*_GL_ATTRIBUTE_DEALLOC (gl_omap_free, 1)*/;

/* Returns the current number of pairs in an ordered map.  */
extern size_t gl_omap_size (gl_omap_t map);

/* Searches whether a pair with the given key is already in the ordered map.
   Returns the value if found, or NULL if not present in the map.  */
extern const void * gl_omap_get (gl_omap_t map, const void *key);

/* Searches whether a pair with the given key is already in the ordered map.
   Returns true and sets *VALUEP to the value if found.
   Returns false if not present in the map.  */
extern bool gl_omap_search (gl_omap_t map, const void *key,
                            const void **valuep);

/* Searches the pair with the least key in the ordered map that compares
   greater or equal to the given THRESHOLD.  The representation of the
   THRESHOLD is defined by the THRESHOLD_FN.
   Returns true and stores the found pair in *KEYP and *VALUEP if found.
   Otherwise returns false.  */
extern bool gl_omap_search_atleast (gl_omap_t map,
                                    gl_mapkey_threshold_fn threshold_fn,
                                    const void *threshold,
                                    const void **keyp, const void **valuep);

/* Adds a pair to an ordered map.
   Returns true if a pair with the given key was not already in the map and so
   this pair was added.
   Returns false if a pair with the given key was already in the map and only
   its value was replaced.  */
/* declared in gl_xomap.h */
extern bool gl_omap_put (gl_omap_t map, const void *key, const void *value);
/* Likewise.  Returns -1 upon out-of-memory.  */
_GL_ATTRIBUTE_NODISCARD
extern int gl_omap_nx_put (gl_omap_t map, const void *key, const void *value);

/* Adds a pair to an ordered map and retrieves the previous value.
   Returns true if a pair with the given key was not already in the map and so
   this pair was added.
   Returns false and sets *OLDVALUEP to the previous value, if a pair with the
   given key was already in the map and only its value was replaced.  */
/* declared in gl_xomap.h */
extern bool gl_omap_getput (gl_omap_t map, const void *key, const void *value,
                            const void **oldvaluep);
/* Likewise.  Returns -1 upon out-of-memory.  */
_GL_ATTRIBUTE_NODISCARD
extern int gl_omap_nx_getput (gl_omap_t map, const void *key, const void *value,
                              const void **oldvaluep);

/* Removes a pair from an ordered map.
   Returns true if the key was found and its pair removed.
   Returns false otherwise.  */
extern bool gl_omap_remove (gl_omap_t map, const void *key);

/* Removes a pair from an ordered map and retrieves the previous value.
   Returns true and sets *OLDVALUEP to the previous value, if the key was found
   and its pair removed.
   Returns false otherwise.  */
extern bool gl_omap_getremove (gl_omap_t map, const void *key,
                               const void **oldvaluep);

/* Frees an entire ordered map.
   (But this call does not free the keys and values of the pairs in the map.
   It only invokes the KDISPOSE_FN on each key and the VDISPOSE_FN on each value
   of the pairs in the map.)  */
extern void gl_omap_free (gl_omap_t map);

#endif /* End of inline and gl_xomap.h-defined functions.  */

/* ---------------------- gl_omap_iterator_t Data Type ---------------------- */

/* Functions for iterating through an ordered map.  */

/* Type of an iterator that traverses an ordered map.
   This is a fixed-size struct, so that creation of an iterator doesn't need
   memory allocation on the heap.  */
typedef struct
{
  /* For fast dispatch of gl_omap_iterator_next.  */
  const struct gl_omap_implementation *vtable;
  /* For detecting whether the last returned pair was removed.  */
  gl_omap_t map;
  size_t count;
  /* Other, implementation-private fields.  */
  void *p; void *q;
  size_t i; size_t j;
} gl_omap_iterator_t;

#if 0 /* These are defined inline below.  */

/* Creates an iterator traversing an ordered map.
   The map's contents must not be modified while the iterator is in use,
   except for modifying the value of the last returned key or removing the
   last returned pair.  */
extern gl_omap_iterator_t gl_omap_iterator (gl_omap_t map);

/* If there is a next pair, stores the next pair in *KEYP and *VALUEP, advances
   the iterator, and returns true.  Otherwise, returns false.  */
extern bool gl_omap_iterator_next (gl_omap_iterator_t *iterator,
                                   const void **keyp, const void **valuep);

/* Frees an iterator.  */
extern void gl_omap_iterator_free (gl_omap_iterator_t *iterator);

#endif /* End of inline functions.  */

/* ------------------------- Implementation Details ------------------------- */

struct gl_omap_implementation
{
  /* gl_omap_t functions.  */
  gl_omap_t (*nx_create_empty) (gl_omap_implementation_t implementation,
                                gl_mapkey_compar_fn compar_fn,
                                gl_mapkey_dispose_fn kdispose_fn,
                                gl_mapvalue_dispose_fn vdispose_fn);
  size_t (*size) (gl_omap_t map);
  bool (*search) (gl_omap_t map, const void *key, const void **valuep);
  bool (*search_atleast) (gl_omap_t map,
                          gl_mapkey_threshold_fn threshold_fn,
                          const void *threshold,
                          const void **keyp, const void **valuep);
  int (*nx_getput) (gl_omap_t map, const void *key, const void *value,
                    const void **oldvaluep);
  bool (*getremove) (gl_omap_t map, const void *key, const void **oldvaluep);
  void (*omap_free) (gl_omap_t map);
  /* gl_omap_iterator_t functions.  */
  gl_omap_iterator_t (*iterator) (gl_omap_t map);
  bool (*iterator_next) (gl_omap_iterator_t *iterator,
                         const void **keyp, const void **valuep);
  void (*iterator_free) (gl_omap_iterator_t *iterator);
};

struct gl_omap_impl_base
{
  const struct gl_omap_implementation *vtable;
  gl_mapkey_compar_fn compar_fn;
  gl_mapkey_dispose_fn kdispose_fn;
  gl_mapvalue_dispose_fn vdispose_fn;
};

/* Define most functions of this file as accesses to the
   struct gl_omap_implementation.  */

GL_OMAP_INLINE
/*_GL_ATTRIBUTE_DEALLOC (gl_omap_free, 1)*/
gl_omap_t
gl_omap_nx_create_empty (gl_omap_implementation_t implementation,
                         gl_mapkey_compar_fn compar_fn,
                         gl_mapkey_dispose_fn kdispose_fn,
                         gl_mapvalue_dispose_fn vdispose_fn)
{
  return implementation->nx_create_empty (implementation, compar_fn,
                                          kdispose_fn, vdispose_fn);
}

GL_OMAP_INLINE size_t
gl_omap_size (gl_omap_t map)
{
  return ((const struct gl_omap_impl_base *) map)->vtable->size (map);
}

GL_OMAP_INLINE bool
gl_omap_search (gl_omap_t map, const void *key, const void **valuep)
{
  return ((const struct gl_omap_impl_base *) map)->vtable
         ->search (map, key, valuep);
}

GL_OMAP_INLINE bool
gl_omap_search_atleast (gl_omap_t map,
                        gl_mapkey_threshold_fn threshold_fn,
                        const void *threshold,
                        const void **keyp, const void **valuep)
{
  return ((const struct gl_omap_impl_base *) map)->vtable
         ->search_atleast (map, threshold_fn, threshold, keyp, valuep);
}

_GL_ATTRIBUTE_NODISCARD GL_OMAP_INLINE int
gl_omap_nx_getput (gl_omap_t map, const void *key, const void *value,
                   const void **oldvaluep)
{
  return ((const struct gl_omap_impl_base *) map)->vtable
         ->nx_getput (map, key, value, oldvaluep);
}

GL_OMAP_INLINE bool
gl_omap_getremove (gl_omap_t map, const void *key, const void **oldvaluep)
{
  return ((const struct gl_omap_impl_base *) map)->vtable
         ->getremove (map, key, oldvaluep);
}

GL_OMAP_INLINE void
gl_omap_free (gl_omap_t map)
{
  ((const struct gl_omap_impl_base *) map)->vtable->omap_free (map);
}

GL_OMAP_INLINE gl_omap_iterator_t
gl_omap_iterator (gl_omap_t map)
{
  return ((const struct gl_omap_impl_base *) map)->vtable->iterator (map);
}

GL_OMAP_INLINE bool
gl_omap_iterator_next (gl_omap_iterator_t *iterator,
                       const void **keyp, const void **valuep)
{
  return iterator->vtable->iterator_next (iterator, keyp, valuep);
}

GL_OMAP_INLINE void
gl_omap_iterator_free (gl_omap_iterator_t *iterator)
{
  iterator->vtable->iterator_free (iterator);
}

/* Define the convenience functions, that is, the functions that are independent
   of the implementation.  */

GL_OMAP_INLINE const void *
gl_omap_get (gl_omap_t map, const void *key)
{
  const void *value = NULL;
  gl_omap_search (map, key, &value);
  return value;
}

_GL_ATTRIBUTE_NODISCARD GL_OMAP_INLINE int
gl_omap_nx_put (gl_omap_t map, const void *key, const void *value)
{
  const void *oldvalue;
  int result = gl_omap_nx_getput (map, key, value, &oldvalue);
  if (result == 0)
    {
      gl_mapvalue_dispose_fn vdispose_fn =
        ((const struct gl_omap_impl_base *) map)->vdispose_fn;
      if (vdispose_fn != NULL)
        vdispose_fn (oldvalue);
    }
  return result;
}

GL_OMAP_INLINE bool
gl_omap_remove (gl_omap_t map, const void *key)
{
  const void *oldvalue;
  bool result = gl_omap_getremove (map, key, &oldvalue);
  if (result)
    {
      gl_mapvalue_dispose_fn vdispose_fn =
        ((const struct gl_omap_impl_base *) map)->vdispose_fn;
      if (vdispose_fn != NULL)
        vdispose_fn (oldvalue);
    }
  return result;
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _GL_OMAP_H */
