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

#include <config.h>

/* Specification.  */
#include "gl_avltree_omap.h"

#include <stdlib.h>

/* -------------------------- gl_omap_t Data Type -------------------------- */

/* Parameterization of gl_avltree_ordered.h.  */
#define CONTAINER_T gl_omap_t
#define CONTAINER_IMPL gl_omap_impl
#define CONTAINER_IMPL_BASE gl_omap_impl_base
#define NODE_IMPL gl_omap_node_impl
#define NODE_T gl_omap_node_t
#define NODE_PAYLOAD_FIELDS \
  const void *key; \
  const void *value;
#define NODE_PAYLOAD_PARAMS \
  const void *key, const void *value
#define NODE_PAYLOAD_ASSIGN(node) \
  node->key = key; \
  node->value = value;
#define NODE_PAYLOAD_DISPOSE(container, node) \
  if (container->base.kdispose_fn != NULL) \
    container->base.kdispose_fn (node->key);

#include "gl_avltree_ordered.h"

/* Generic binary tree code.  */
#include "gl_anytree_omap.h"

/* For debugging.  */
extern void gl_avltree_omap_check_invariants (gl_omap_t);
void
gl_avltree_omap_check_invariants (gl_omap_t map)
{
  size_t counter = 0;
  if (map->root != NULL)
    check_invariants (map->root, NULL, &counter);
  if (!(map->count == counter))
    abort ();
}

const struct gl_omap_implementation gl_avltree_omap_implementation =
  {
    gl_tree_nx_create_empty,
    gl_tree_size,
    gl_tree_search,
    gl_tree_search_atleast,
    gl_tree_nx_getput,
    gl_tree_getremove,
    gl_tree_omap_free,
    gl_tree_iterator,
    gl_tree_iterator_next,
    gl_tree_iterator_free
  };
