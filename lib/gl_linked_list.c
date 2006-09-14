/* Sequential list data type implemented by a linked list.
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

#include <config.h>

/* Specification.  */
#include "gl_linked_list.h"

#include <stdlib.h>

#include "xalloc.h"

/* -------------------------- gl_list_t Data Type -------------------------- */

/* Generic linked list code.  */
#include "gl_anylinked_list1.h"
#include "gl_anylinked_list2.h"


const struct gl_list_implementation gl_linked_list_implementation =
  {
    gl_linked_create_empty,
    gl_linked_create,
    gl_linked_size,
    gl_linked_node_value,
    gl_linked_next_node,
    gl_linked_previous_node,
    gl_linked_get_at,
    gl_linked_set_at,
    gl_linked_search,
    gl_linked_indexof,
    gl_linked_add_first,
    gl_linked_add_last,
    gl_linked_add_before,
    gl_linked_add_after,
    gl_linked_add_at,
    gl_linked_remove_node,
    gl_linked_remove_at,
    gl_linked_remove,
    gl_linked_list_free,
    gl_linked_iterator,
    gl_linked_iterator_from_to,
    gl_linked_iterator_next,
    gl_linked_iterator_free,
    gl_linked_sortedlist_search,
    gl_linked_sortedlist_indexof,
    gl_linked_sortedlist_add,
    gl_linked_sortedlist_remove
  };
