/* Test of POSIX and GNU compatible vdzprintf() function.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "macros.h"

#define RETTYPE off64_t
#include "test-fprintf-gnu.h"

static off64_t
my_fzprintf (FILE *fp, const char *format, ...)
{
  va_list args;
  off64_t ret;

  va_start (args, format);
  ret = vdzprintf (fileno (fp), format, args);
  va_end (args);
  return ret;
}

int
main (int argc, char *argv[])
{
  test_function (my_fzprintf);
  return test_exit_status;
}
