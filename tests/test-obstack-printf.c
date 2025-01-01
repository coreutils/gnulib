/* Test of obstack_printf() and obstack_vprintf() functions.
   Copyright (C) 2008-2025 Free Software Foundation, Inc.

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

/* Written by Eric Blake <ebb9@byu.net>, 2008.  */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include "signature.h"
SIGNATURE_CHECK (obstack_printf, int, (struct obstack *, char const *, ...));
SIGNATURE_CHECK (obstack_vprintf, int,
                 (struct obstack *, char const *, va_list));

#include "obstack.h"
#include "xalloc.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#define RETTYPE int
#include "test-obstack-printf.h"

static int
my_obstack_printf (struct obstack *obs, const char *format, ...)
{
  va_list args;
  int ret;

  va_start (args, format);
  ret = obstack_vprintf (obs, format, args);
  va_end (args);
  return ret;
}

int
main (int argc, char *argv[])
{
  test_function (my_obstack_printf);
  test_function (obstack_printf);
  return test_exit_status;
}
