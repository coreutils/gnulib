/* Determine the protection of a virtual memory area.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#ifndef _VMA_PROT_H
#define _VMA_PROT_H

/* Get size_t.  */
#include <stddef.h>

/* Get VMA_PROT_READ, VMA_PROT_WRITE, VMA_PROT_EXECUTE.  */
#include "vma-iter.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Returns the declared permissions available on the memory area that
   starts at START and is SIZE bytes long, as a combination of the bit masks
   VMA_PROT_READ, VMA_PROT_WRITE, VMA_PROT_EXECUTE.
   Note: The effective permissions may be larger.  For example, some hardware
   allows execute permission anywhere where read permission is present.
   Returns -1 if it cannot be determined.

   Note: This function is expensive.  If possible, an application should find
   faster alternatives for memory areas that it has allocated itself, such as
   through malloc(), mmap(), or shmat().  */
extern int get_vma_prot (void *start, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* _VMA_PROT_H */
