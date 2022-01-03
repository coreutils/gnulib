/* Sequential list data type backed by another list.
   Copyright (C) 2006, 2009-2022 Free Software Foundation, Inc.
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

#ifndef _GL_SUBLIST_H
#define _GL_SUBLIST_H

#include "gl_list.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Creates a sublist of a given list.
   This is the list of elements with indices i, start_index <= i < end_index.
   The sublist is backed by the given list, which means:
     - Modifications to the sublist affect the whole list.
     - Modifications to the whole list are immediately visible in the sublist.
     - The sublist is only valid as long as the whole list is valid.
     - The sublist must not be passed to the gl_list_sortedlist_add() function.
 */
#if 0 /* declared in gl_xsublist.h */
extern gl_list_t gl_sublist_create (gl_list_t whole_list,
                                    size_t start_index, size_t end_index)
  /*_GL_ATTRIBUTE_DEALLOC (gl_list_free, 1)*/
  _GL_ATTRIBUTE_RETURNS_NONNULL;
#endif
/* Likewise.  Returns NULL upon out-of-memory.  */
extern gl_list_t gl_sublist_nx_create (gl_list_t whole_list,
                                       size_t start_index, size_t end_index)
  /*_GL_ATTRIBUTE_DEALLOC (gl_list_free, 1)*/;


#ifdef __cplusplus
}
#endif

#endif /* _GL_SUBLIST_H */
