/* Test of c_vasnprintf() function.
   Copyright (C) 2011-2025 Free Software Foundation, Inc.

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

#include "c-vasnprintf.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

static char *
my_c_asnprintf (char *resultbuf, size_t *lengthp, const char *format, ...)
{
  va_list args;
  char *ret;

  va_start (args, format);
  ret = c_vasnprintf (resultbuf, lengthp, format, args);
  va_end (args);
  return ret;
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  /* Test behaviour of c_vasnprintf().
     It should always use '.' as the decimal point. */
  {
    char buf[16];
    size_t length = sizeof (buf);
    char *result = my_c_asnprintf (buf, &length, "%#.0f", 1.0);
    ASSERT (result == buf);
    ASSERT (strcmp (result, "1.") == 0);
  }

  return test_exit_status;
}
