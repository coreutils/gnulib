/* Ordered map data type implemented by a binary tree.
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

/* Common code of gl_avltree_omap.c and gl_rbtree_omap.c.  */

/* An item on the stack used for iterating across the elements.  */
typedef struct
{
  gl_omap_node_t node;
  bool rightp;
} iterstack_item_t;

/* A stack used for iterating across the elements.  */
typedef iterstack_item_t iterstack_t[MAXHEIGHT];

static gl_omap_t
gl_tree_nx_create_empty (gl_omap_implementation_t implementation,
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
  map->root = NULL;
  map->count = 0;

  return map;
}

static size_t _GL_ATTRIBUTE_PURE
gl_tree_size (gl_omap_t map)
{
  return map->count;
}

static bool
gl_tree_search (gl_omap_t map, const void *key, const void **valuep)
{
  gl_mapkey_compar_fn compar = map->base.compar_fn;
  gl_omap_node_t node;

  for (node = map->root; node != NULL; )
    {
      int cmp = (compar != NULL
                 ? compar (node->key, key)
                 : (node->key > key ? 1 :
                    node->key < key ? -1 : 0));

      if (cmp < 0)
        node = node->right;
      else if (cmp > 0)
        node = node->left;
      else /* cmp == 0 */
        {
          /* We have a key equal to KEY.  */
          *valuep = node->value;
          return true;
        }
    }
  return false;
}

static bool
gl_tree_search_atleast (gl_omap_t map,
                        gl_mapkey_threshold_fn threshold_fn,
                        const void *threshold,
                        const void **keyp, const void **valuep)
{
  gl_omap_node_t node;

  for (node = map->root; node != NULL; )
    {
      if (! threshold_fn (node->key, threshold))
        node = node->right;
      else
        {
          /* We have a key >= THRESHOLD.  But we need the leftmost such
             element.  */
          gl_omap_node_t found = node;
          node = node->left;
          for (; node != NULL; )
            {
              if (! threshold_fn (node->key, threshold))
                node = node->right;
              else
                {
                  found = node;
                  node = node->left;
                }
            }
          *keyp = found->key;
          *valuep = found->value;
          return true;
        }
    }
  return false;
}

static int
gl_tree_nx_getput (gl_omap_t map, const void *key, const void *value,
                   const void **oldvaluep)
{
  gl_mapkey_compar_fn compar;
  gl_omap_node_t node = map->root;

  if (node == NULL)
    {
      if (gl_tree_nx_add_first (map, key, value) == NULL)
        return -1;
      return 1;
    }

  compar = map->base.compar_fn;

  for (;;)
    {
      int cmp = (compar != NULL
                 ? compar (node->key, key)
                 : (node->key > key ? 1 :
                    node->key < key ? -1 : 0));

      if (cmp < 0)
        {
          if (node->right == NULL)
            {
              if (gl_tree_nx_add_after (map, node, key, value) == NULL)
                return -1;
              return 1;
            }
          node = node->right;
        }
      else if (cmp > 0)
        {
          if (node->left == NULL)
            {
              if (gl_tree_nx_add_before (map, node, key, value) == NULL)
                return -1;
              return 1;
            }
          node = node->left;
        }
      else /* cmp == 0 */
        {
          *oldvaluep = node->value;
          node->value = value;
          return 0;
        }
    }
}

static bool
gl_tree_getremove (gl_omap_t map, const void *key, const void **oldvaluep)
{
  gl_mapkey_compar_fn compar = map->base.compar_fn;
  gl_omap_node_t node;

  for (node = map->root; node != NULL; )
    {
      int cmp = (compar != NULL
                 ? compar (node->key, key)
                 : (node->key > key ? 1 :
                    node->key < key ? -1 : 0));

      if (cmp < 0)
        node = node->right;
      else if (cmp > 0)
        node = node->left;
      else /* cmp == 0 */
        {
          /* We have a key equal to KEY.  */
          *oldvaluep = node->value;
          gl_tree_remove_node (map, node);
          return true;
        }
    }
  return false;
}

static void
gl_tree_omap_free (gl_omap_t map)
{
  /* Iterate across all elements in post-order.  */
  gl_omap_node_t node = map->root;
  iterstack_t stack;
  iterstack_item_t *stack_ptr = &stack[0];

  for (;;)
    {
      /* Descend on left branch.  */
      for (;;)
        {
          if (node == NULL)
            break;
          stack_ptr->node = node;
          stack_ptr->rightp = false;
          node = node->left;
          stack_ptr++;
        }
      /* Climb up again.  */
      for (;;)
        {
          if (stack_ptr == &stack[0])
            goto done_iterate;
          stack_ptr--;
          node = stack_ptr->node;
          if (!stack_ptr->rightp)
            break;
          /* Free the current node.  */
          if (map->base.vdispose_fn != NULL)
            map->base.vdispose_fn (node->value);
          if (map->base.kdispose_fn != NULL)
            map->base.kdispose_fn (node->key);
          free (node);
        }
      /* Descend on right branch.  */
      stack_ptr->rightp = true;
      node = node->right;
      stack_ptr++;
    }
 done_iterate:
  free (map);
}

/* --------------------- gl_omap_iterator_t Data Type --------------------- */

static gl_omap_iterator_t _GL_ATTRIBUTE_PURE
gl_tree_iterator (gl_omap_t map)
{
  gl_omap_iterator_t result;
  gl_omap_node_t node;

  result.vtable = map->base.vtable;
  result.map = map;
  /* Start node is the leftmost node.  */
  node = map->root;
  if (node != NULL)
    while (node->left != NULL)
      node = node->left;
  result.p = node;
  /* End point is past the rightmost node.  */
  result.q = NULL;
#if defined GCC_LINT || defined lint
  result.i = 0;
  result.j = 0;
  result.count = 0;
#endif

  return result;
}

static bool
gl_tree_iterator_next (gl_omap_iterator_t *iterator,
                       const void **keyp, const void **valuep)
{
  if (iterator->p != iterator->q)
    {
      gl_omap_node_t node = (gl_omap_node_t) iterator->p;
      *keyp = node->key;
      *valuep = node->value;
      /* Advance to the next node.  */
      if (node->right != NULL)
        {
          node = node->right;
          while (node->left != NULL)
            node = node->left;
        }
      else
        {
          while (node->parent != NULL && node->parent->right == node)
            node = node->parent;
          node = node->parent;
        }
      iterator->p = node;
      return true;
    }
  else
    return false;
}

static void
gl_tree_iterator_free (_GL_ATTRIBUTE_MAYBE_UNUSED gl_omap_iterator_t *iterator)
{
}
