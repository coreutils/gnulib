/* Test of [v]aszprintf() with big results.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

#include <stdio.h>

/* Get INT_MAX.  */
#include <limits.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

int
main ()
{
  /* Large width given as argument.  */
  {
    char *result;
    ptrdiff_t retval =
      aszprintf (&result, "%.*d %d", INT_MAX, 1234567, 99);
    if (retval < 0)
      ASSERT (errno == ENOMEM);
    else
      {
        for (int i = 0; i < INT_MAX - 7; i++)
          ASSERT (result[i] == '0');
        ASSERT (streq (result + INT_MAX - 7, "1234567 99"));
        ASSERT (retval == strlen (result));
        free (result);
      }
  }

  return test_exit_status;
}
