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

#include <config.h>

/* Specification.  */
#include "vma-prot.h"

#include <stdint.h>

struct locals
{
  /* The common protections seen so far.  */
  unsigned int prot_so_far;
  /* The remaining address interval.
     remaining_start <= remaining_end-1.  */
  uintptr_t remaining_start;
  uintptr_t remaining_end;
};

static int
callback (void *data, uintptr_t start, uintptr_t end, unsigned int flags)
{
  struct locals *l = (struct locals *) data;

  if (start > l->remaining_start)
    {
      /* The interval [remaining_start, min (remaining_end, start)) is
         unmapped.  */
      l->prot_so_far = 0;
      return 1;
    }
  if (end - 1 < l->remaining_start)
    /* The interval [start,end) lies before [remaining_start,remaining_end).  */
    return 0;
  /* Here  start <= remaining_start <= remaining_end-1
     and            remaining_start <= end-1,
     hence start <= remaining_start <= min (end-1, remaining_end-1).
     So, the two intervals intersect.  */
  l->prot_so_far &= flags;
  if (l->prot_so_far == 0)
    return 1;
  if (l->remaining_end - 1 <= end - 1)
    /* Done.  */
    return 1;
  /* Trim the remaining address interval.  */
  l->remaining_start = end;
  return 0;
}

int
get_vma_prot (void *start, size_t size)
{
  uintptr_t start_address = (uintptr_t) start;

  struct locals l;
  l.prot_so_far = VMA_PROT_READ | VMA_PROT_WRITE | VMA_PROT_EXECUTE;
  l.remaining_start = start_address;
  l.remaining_end = start_address + size;

  if (l.remaining_end < l.remaining_start)
    /* Invalid arguments.  */
    return -1;

  if (l.remaining_end > l.remaining_start)
    {
      if (vma_iterate (callback, &l) < 0)
        return -1;
    }
  return l.prot_so_far;
}
