/* Test of POSIX and GNU compatible c_vzsnprintf() function.
   Copyright (C) 2011-2024 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

#include "c-vsnprintf.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

#include "test-zsnprintf-gnu.h"

static ptrdiff_t
my_c_zsnprintf (char *str, size_t size, const char *format, ...)
{
  va_list args;
  ptrdiff_t ret;

  va_start (args, format);
  ret = c_vzsnprintf (str, size, format, args);
  va_end (args);
  return ret;
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  test_function (my_c_zsnprintf);

  return test_exit_status;
}
