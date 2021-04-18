/* Test of realloc function.
   Copyright (C) 2010-2021 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>
#include <stdint.h>

int
main ()
{
  /* Check that realloc (NULL, 0) is not a NULL pointer.  */
  char *p = realloc (NULL, 0);
  if (p == NULL)
    return 1;

  /* Check that realloc (p, n) fails when p is non-null and n exceeds
     PTRDIFF_MAX.  */
  if (PTRDIFF_MAX < SIZE_MAX)
    {
      size_t n = PTRDIFF_MAX, n1 = n + 1;
      if (realloc (p, n1) != NULL)
        return 1;
    }

  free (p);
  return 0;
}
