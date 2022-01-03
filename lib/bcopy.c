/* bcopy.c -- copy memory.

   Copyright (C) 1992-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by David MacKenzie <djm@gnu.ai.mit.edu>.  */

#include <config.h>

#include <stddef.h>

/* Copy LENGTH bytes from SOURCE to DEST.  Does not null-terminate.  */

void
bcopy (void const *source0, void *dest0, size_t length)
{
  char const *source = source0;
  char *dest = dest0;
  if (source < dest)
    /* Moving from low mem to hi mem; start at end.  */
    for (source += length, dest += length; length; --length)
      *--dest = *--source;
  else if (source != dest)
    /* Moving from hi mem to low mem; start at beginning.  */
    for (; length; --length)
      *dest++ = *source++;
}
