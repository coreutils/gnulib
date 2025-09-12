/* Memory allocation aligned to system page boundaries.

   Copyright (C) 2005, 2008, 2010-2025 Free Software Foundation, Inc.

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

#ifndef _PAGEALIGN_ALLOC_H
#define _PAGEALIGN_ALLOC_H

/* This file uses _GL_ATTRIBUTE_ALLOC_SIZE, _GL_ATTRIBUTE_DEALLOC,
   _GL_ATTRIBUTE_MALLOC, _GL_ATTRIBUTE_NONNULL,
   _GL_ATTRIBUTE_RETURNS_NONNULL.  */
#if !_GL_CONFIG_H_INCLUDED
# error "Please include config.h first."
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Note: The functions declared in this file are NOT multithread-safe.  */


/* Free a memory block.
   PTR must be a non-NULL pointer returned by pagealign_alloc or
   pagealign_xalloc.  */
extern void pagealign_free (void *ptr)
  _GL_ATTRIBUTE_NONNULL ((1));

/* Allocate a block of memory of SIZE bytes, aligned on a system page
   boundary.
   If SIZE is not a multiple of the system page size, it will be rounded up
   to the next multiple.
   Return a pointer to the start of the memory block. Upon allocation failure,
   return NULL and set errno.  */
extern void *pagealign_alloc (size_t size)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC (pagealign_free, 1)
  _GL_ATTRIBUTE_ALLOC_SIZE ((1));

/* Like pagealign_alloc, except it exits the program if the allocation
   fails.  */
extern void *pagealign_xalloc (size_t size)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC (pagealign_free, 1)
  _GL_ATTRIBUTE_ALLOC_SIZE ((1)) _GL_ATTRIBUTE_RETURNS_NONNULL;


/* Selects the implementation of pagealign_alloc.
   If you set pagealign_impl, it must be before the first invocation of
   pagealign_alloc or pagealign_xalloc.  */
typedef enum
{
  PA_IMPL_DEFAULT        = 0, /* platform-dependent default */
  PA_IMPL_MALLOC         = 1, /* malloc */
  PA_IMPL_MMAP           = 2, /* mmap (if available) */
  PA_IMPL_POSIX_MEMALIGN = 3, /* posix_memalign (if available) */
  PA_IMPL_ALIGNED_MALLOC = 4, /* _aligned_malloc (if available) */
  PA_IMPL_VIRTUAL_ALLOC  = 5  /* VirtualAlloc (if available) */
} pagealign_impl_t;
extern pagealign_impl_t pagealign_impl;


#ifdef __cplusplus
}
#endif

#endif /* _PAGEALIGN_ALLOC_H */
