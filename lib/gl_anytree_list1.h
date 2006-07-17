/* Sequential list data type implemented by a binary tree.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Common code of gl_avltree_list.c, gl_rbtree_list.c,
                  gl_avltreehash_list.c, gl_rbtreehash_list.c.  */

/* An item on the stack used for iterating across the elements.  */
typedef struct
{
  gl_list_node_t node;
  bool rightp;
} iterstack_item_t;

/* A stack used for iterating across the elements.  */
typedef iterstack_item_t iterstack_t[MAXHEIGHT];
