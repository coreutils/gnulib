/* Memory allocation aligned to system page boundaries.

   Copyright (C) 2005, 2008, 2010-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _PAGEALIGN_ALLOC_H
# define _PAGEALIGN_ALLOC_H

# include <stddef.h>

#if __GNUC__ >= 3
# define _GL_ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
#else
# define _GL_ATTRIBUTE_MALLOC
#endif

#if ! defined __clang__ && \
    (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
# define _GL_ATTRIBUTE_ALLOC_SIZE(args) __attribute__ ((__alloc_size__ args))
#else
# define _GL_ATTRIBUTE_ALLOC_SIZE(args)
#endif

/* Allocate a block of memory of SIZE bytes, aligned on a system page
   boundary.
   If SIZE is not a multiple of the system page size, it will be rounded up
   to the next multiple.
   Return a pointer to the start of the memory block. Upon allocation failure,
   return NULL and set errno.  */
extern void *pagealign_alloc (size_t size)
     _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_ALLOC_SIZE ((1));

/* Like pagealign_alloc, except it exits the program if the allocation
   fails.  */
extern void *pagealign_xalloc (size_t size)
     _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_ALLOC_SIZE ((1));

/* Free a memory block.
   PTR must be a non-NULL pointer returned by pagealign_alloc or
   pagealign_xalloc.  */
extern void pagealign_free (void *ptr);

#endif /* _PAGEALIGN_ALLOC_H */
