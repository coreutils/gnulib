/* Simple atomic operations for multithreading.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#ifndef _SIMPLE_ATOMIC_H
#define _SIMPLE_ATOMIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Guarantees that memory stores that are in code before this call
   are finished before this call, and that memory loads that are in code
   after this call are started after this call.  */
extern void memory_barrier (void);

/* Stores NEWVAL in *VP if the old value *VP is == CMP.
   Returns the old value.  */
extern unsigned int atomic_compare_and_swap (unsigned int volatile *vp,
                                             unsigned int cmp,
                                             unsigned int newval);

/* Stores NEWVAL in *VP if the old value *VP is == CMP.
   Returns the old value.  */
extern uintptr_t atomic_compare_and_swap_ptr (uintptr_t volatile *vp,
                                              uintptr_t cmp,
                                              uintptr_t newval);

#ifdef __cplusplus
}
#endif

#endif /* _SIMPLE_ATOMIC_H */
