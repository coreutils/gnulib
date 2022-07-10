/* Test of getter for RLIMIT_AS.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.

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

#include <config.h>

#include "resource-ext.h"

#include <stdio.h>
#include <stdlib.h>

#include "qemu.h"
#include "macros.h"

void *memchunk1;
void *memchunk2;

int
main ()
{
  uintptr_t value1, value2, value3;

  value1 = get_rusage_as ();

  memchunk1 = malloc (0x88);

  value2 = get_rusage_as ();

  memchunk2 = malloc (0x281237);

  value3 = get_rusage_as ();

  if (value1 == 0 && value2 == 0 && value3 == 0)
    {
      fprintf (stderr, "Skipping test: no way to determine address space size\n");
      return 77;
    }
  else if (is_running_under_qemu_user ())
    {
      fprintf (stderr, "Skipping test: running under QEMU\n");
      return 77;
    }
  else
    {
      /* The address space size is positive.  */
      ASSERT (value1 > 0);

      /* Allocating memory should not decrease the address space size.  */
      ASSERT (value2 >= value1);
      ASSERT (value3 >= value2);

      /* Allocating 2.5 MB of memory should increase the address space size.  */
      #ifdef _AIX
      /* Except on AIX in 32-bit mode, where a single interval is used for
         malloc and for the stack.  malloc() blocks are taken from the bottom
         of this interval.  The stack sits at its top.  */
      if (sizeof (void *) > 4)
      #endif
        ASSERT (value3 > value1);

      return 0;
    }
}
