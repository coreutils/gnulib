/* Test of POSIX and GNU compatible c_vaszprintf() and c_aszprintf() functions.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

#include "c-vasprintf.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

#include "test-vaszprintf-gnu.h"

static ptrdiff_t
my_c_aszprintf (char **result, const char *format, ...)
{
  va_list args;
  ptrdiff_t ret;

  va_start (args, format);
  ret = c_vaszprintf (result, format, args);
  va_end (args);
  return ret;
}

static void
test_c_vaszprintf ()
{
  test_function (my_c_aszprintf);
}

static void
test_c_aszprintf ()
{
  test_function (c_aszprintf);
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  test_c_vaszprintf ();
  test_c_aszprintf ();

  return test_exit_status;
}
