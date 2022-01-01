/* Ordered map data type implemented by an array.
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

#include <config.h>

/* Specification.  */
#include "gl_array_omap.h"

#include <stdlib.h>

/* Checked size_t computations.  */
#include "xsize.h"

/* -------------------------- gl_omap_t Data Type -------------------------- */

struct pair
{
  const void *key;
  const void *value;
};

/* Concrete gl_omap_impl type, valid for this file only.  */
struct gl_omap_impl
{
  struct gl_omap_impl_base base;
  /* An array of ALLOCATED pairs, of which the first COUNT are used.
     0 <= COUNT <= ALLOCATED.  */
  struct pair *pairs;
  size_t count;
  size_t allocated;
};

static gl_omap_t
gl_array_nx_create_empty (gl_omap_implementation_t implementation,
                          gl_mapkey_compar_fn compar_fn,
                          gl_mapkey_dispose_fn kdispose_fn,
                          gl_mapvalue_dispose_fn vdispose_fn)
{
  struct gl_omap_impl *map =
    (struct gl_omap_impl *) malloc (sizeof (struct gl_omap_impl));

  if (map == NULL)
    return NULL;

  map->base.vtable = implementation;
  map->base.compar_fn = compar_fn;
  map->base.kdispose_fn = kdispose_fn;
  map->base.vdispose_fn = vdispose_fn;
  map->pairs = NULL;
  map->count = 0;
  map->allocated = 0;

  return map;
}

static size_t _GL_ATTRIBUTE_PURE
gl_array_size (gl_omap_t map)
{
  return map->count;
}

static size_t _GL_ATTRIBUTE_PURE
gl_array_indexof (gl_omap_t map, const void *key)
{
  size_t count = map->count;

  if (count > 0)
    {
      gl_mapkey_compar_fn compar = map->base.compar_fn;
      size_t low = 0;
      size_t high = count;

      /* At each loop iteration, low < high; for indices < low the values
         are smaller than KEY; for indices >= high the values are greater
         than KEY.  So, if the key occurs in the map, it is at
         low <= position < high.  */
      do
        {
          size_t mid = low + (high - low) / 2; /* low <= mid < high */
          int cmp = (compar != NULL
                     ? compar (map->pairs[mid].key, key)
                     : (map->pairs[mid].key > key ? 1 :
                        map->pairs[mid].key < key ? -1 : 0));

          if (cmp < 0)
            low = mid + 1;
          else if (cmp > 0)
            high = mid;
          else /* cmp == 0 */
            /* We have a key equal to KEY at index MID.  */
            return mid;
        }
      while (low < high);
    }
  return (size_t)(-1);
}

static bool _GL_ATTRIBUTE_PURE
gl_array_search (gl_omap_t map, const void *key, const void **valuep)
{
  size_t index = gl_array_indexof (map, key);
  if (index != (size_t)(-1))
    {
      *valuep = map->pairs[index].value;
      return true;
    }
  else
    return false;
}

static bool _GL_ATTRIBUTE_PURE
gl_array_search_atleast (gl_omap_t map,
                         gl_mapkey_threshold_fn threshold_fn,
                         const void *threshold,
                         const void **keyp, const void **valuep)
{
  size_t count = map->count;

  if (count > 0)
    {
      size_t low = 0;
      size_t high = count;

      /* At each loop iteration, low < high; for indices < low the values are
         smaller than THRESHOLD; for indices >= high the values are nonexistent.
         So, if a key >= THRESHOLD occurs in the map, it is at
         low <= position < high.  */
      do
        {
          size_t mid = low + (high - low) / 2; /* low <= mid < high */

          if (! threshold_fn (map->pairs[mid].key, threshold))
            low = mid + 1;
          else
            {
              /* We have a key >= THRESHOLD at index MID.  But we need the
                 minimal such index.  */
              high = mid;
              /* At each loop iteration, low <= high and
                   compar (map->pairs[high].key, value) >= 0,
                 and we know that the first occurrence of the key is at
                 low <= position <= high.  */
              while (low < high)
                {
                  size_t mid2 = low + (high - low) / 2; /* low <= mid2 < high */

                  if (! threshold_fn (map->pairs[mid2].key, threshold))
                    low = mid2 + 1;
                  else
                    high = mid2;
                }
              *keyp = map->pairs[low].key;
              *valuep = map->pairs[low].value;
              return true;
            }
        }
      while (low < high);
    }
  return false;
}

/* Ensure that map->allocated > map->count.
   Return 0 upon success, -1 upon out-of-memory.  */
static int
grow (gl_omap_t map)
{
  size_t new_allocated;
  size_t memory_size;
  struct pair *memory;

  new_allocated = xtimes (map->allocated, 2);
  new_allocated = xsum (new_allocated, 1);
  memory_size = xtimes (new_allocated, sizeof (struct pair));
  if (size_overflow_p (memory_size))
    /* Overflow, would lead to out of memory.  */
    return -1;
  memory = (struct pair *) realloc (map->pairs, memory_size);
  if (memory == NULL)
    /* Out of memory.  */
    return -1;
  map->pairs = memory;
  map->allocated = new_allocated;
  return 0;
}

/* Add the given pair (KEY, VALUE) at the given position,
   0 <= position <= gl_omap_size (map).
   Return 1 upon success, -1 upon out-of-memory.  */
static int
gl_array_nx_add_at (gl_omap_t map, size_t position,
                    const void *key, const void *value)
{
  size_t count = map->count;
  struct pair *pairs;
  size_t i;

  if (count == map->allocated)
    if (grow (map) < 0)
      return -1;
  pairs = map->pairs;
  for (i = count; i > position; i--)
    pairs[i] = pairs[i - 1];
  pairs[position].key = key;
  pairs[position].value = value;
  map->count = count + 1;
  return 1;
}

static int
gl_array_nx_getput (gl_omap_t map, const void *key, const void *value,
                    const void **oldvaluep)
{
  size_t count = map->count;
  size_t low = 0;

  if (count > 0)
    {
      gl_mapkey_compar_fn compar = map->base.compar_fn;
      size_t high = count;

      /* At each loop iteration, low < high; for indices < low the keys
         are smaller than KEY; for indices >= high the keys are greater
         than KEY.  So, if the key occurs in the map, it is at
         low <= position < high.  */
      do
        {
          size_t mid = low + (high - low) / 2; /* low <= mid < high */
          int cmp = (compar != NULL
                     ? compar (map->pairs[mid].key, key)
                     : (map->pairs[mid].key > key ? 1 :
                        map->pairs[mid].key < key ? -1 : 0));

          if (cmp < 0)
            low = mid + 1;
          else if (cmp > 0)
            high = mid;
          else /* cmp == 0 */
            {
              *oldvaluep = map->pairs[mid].value;
              map->pairs[mid].value = value;
              return 0;
            }
        }
      while (low < high);
    }
  return gl_array_nx_add_at (map, low, key, value);
}

/* Remove the pair at the given position,
   0 <= position < gl_omap_size (map).  */
static void
gl_array_remove_at (gl_omap_t map, size_t position)
{
  size_t count = map->count;
  struct pair *pairs;
  size_t i;

  pairs = map->pairs;
  if (map->base.kdispose_fn != NULL)
    map->base.kdispose_fn (pairs[position].key);
  for (i = position + 1; i < count; i++)
    pairs[i - 1] = pairs[i];
  map->count = count - 1;
}

static bool
gl_array_getremove (gl_omap_t map, const void *key, const void **oldvaluep)
{
  size_t index = gl_array_indexof (map, key);
  if (index != (size_t)(-1))
    {
      *oldvaluep = map->pairs[index].value;
      gl_array_remove_at (map, index);
      return true;
    }
  else
    return false;
}

static void
gl_array_free (gl_omap_t map)
{
  if (map->pairs != NULL)
    {
      if (map->base.kdispose_fn != NULL || map->base.vdispose_fn != NULL)
        {
          size_t count = map->count;

          if (count > 0)
            {
              gl_mapkey_dispose_fn kdispose = map->base.kdispose_fn;
              gl_mapvalue_dispose_fn vdispose = map->base.vdispose_fn;
              struct pair *pairs = map->pairs;

              do
                {
                  if (vdispose)
                    vdispose (pairs->value);
                  if (kdispose)
                    kdispose (pairs->key);
                  pairs++;
                }
              while (--count > 0);
            }
        }
      free (map->pairs);
    }
  free (map);
}

/* ---------------------- gl_omap_iterator_t Data Type ---------------------- */

static gl_omap_iterator_t _GL_ATTRIBUTE_PURE
gl_array_iterator (gl_omap_t map)
{
  gl_omap_iterator_t result;

  result.vtable = map->base.vtable;
  result.map = map;
  result.count = map->count;
  result.p = map->pairs + 0;
  result.q = map->pairs + map->count;
#if defined GCC_LINT || defined lint
  result.i = 0;
  result.j = 0;
#endif

  return result;
}

static bool
gl_array_iterator_next (gl_omap_iterator_t *iterator,
                        const void **keyp, const void **valuep)
{
  gl_omap_t map = iterator->map;
  if (iterator->count != map->count)
    {
      if (iterator->count != map->count + 1)
        /* Concurrent modifications were done on the map.  */
        abort ();
      /* The last returned pair was removed.  */
      iterator->count--;
      iterator->p = (struct pair *) iterator->p - 1;
      iterator->q = (struct pair *) iterator->q - 1;
    }
  if (iterator->p < iterator->q)
    {
      struct pair *p = (struct pair *) iterator->p;
      *keyp = p->key;
      *valuep = p->value;
      iterator->p = p + 1;
      return true;
    }
  else
    return false;
}

static void
gl_array_iterator_free (_GL_ATTRIBUTE_MAYBE_UNUSED gl_omap_iterator_t *iterator)
{
}


const struct gl_omap_implementation gl_array_omap_implementation =
  {
    gl_array_nx_create_empty,
    gl_array_size,
    gl_array_search,
    gl_array_search_atleast,
    gl_array_nx_getput,
    gl_array_getremove,
    gl_array_free,
    gl_array_iterator,
    gl_array_iterator_next,
    gl_array_iterator_free
  };
