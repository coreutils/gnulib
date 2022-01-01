/* Sequential list data type implemented by a binary tree.
   Copyright (C) 2006, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

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

/* Common code of gl_avltree_list.c and gl_avltreehash_list.c.  */

/* An AVL tree is a binary tree where
   1. The height of each node is calculated as
        heightof(node) = 1 + max (heightof(node.left), heightof(node.right)).
   2. The heights of the subtrees of each node differ by at most 1:
        | heightof(right) - heightof(left) | <= 1.
   3. The index of the elements in the node.left subtree are smaller than
      the index of node.
      The index of the elements in the node.right subtree are larger than
      the index of node.
 */

/* -------------------------- gl_list_t Data Type -------------------------- */

/* Concrete list node implementation, valid for this file only.  */
struct gl_list_node_impl
{
#if WITH_HASHTABLE
  struct gl_hash_entry h;           /* hash table entry fields; must be first */
#endif
  struct gl_list_node_impl *left;   /* left branch, or NULL */
  struct gl_list_node_impl *right;  /* right branch, or NULL */
  /* Parent pointer, or NULL. The parent pointer is not needed for most
     operations.  It is needed so that a gl_list_node_t can be returned
     without memory allocation, on which the functions gl_list_remove_node,
     gl_list_add_before, gl_list_add_after can be implemented.  */
  struct gl_list_node_impl *parent;
  int balance;                      /* heightof(right) - heightof(left),
                                       always = -1 or 0 or 1 */
  size_t branch_size;               /* number of nodes in this branch,
                                       = branchsize(left)+branchsize(right)+1 */
  const void *value;
};

/* Concrete gl_list_impl type, valid for this file only.  */
struct gl_list_impl
{
  struct gl_list_impl_base base;
#if WITH_HASHTABLE
  /* A hash table: managed as an array of collision lists.  */
  struct gl_hash_entry **table;
  size_t table_size;
#endif
  struct gl_list_node_impl *root;   /* root node or NULL */
};

/* An AVL tree of height h has at least F_(h+2) - 1 [Fibonacci number] and at
   most 2^h - 1 elements.  So, h <= 84 (because a tree of height h >= 85 would
   have at least F_87 - 1 elements, and because even on 64-bit machines,
     sizeof (gl_list_node_impl) * (F_87 - 1) > 2^64
   this would exceed the address space of the machine.  */
#define MAXHEIGHT 83
