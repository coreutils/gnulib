/* Test of [v]asnprintf() with big results.
   Copyright (C) 2024-2026 Free Software Foundation, Inc.

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

#include "vasnprintf.h"

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
    size_t length;
    char *result =
      asnprintf (NULL, &length, "%*d", INT_MAX, INT_MIN);
    if (!result)
      ASSERT (errno == ENOMEM);
    else
      {
        ASSERT (atoi (result) == INT_MIN);
        ASSERT (length == INT_MAX);
        ASSERT (length == strlen (result));
        free (result);
      }
  }

  return test_exit_status;
}
