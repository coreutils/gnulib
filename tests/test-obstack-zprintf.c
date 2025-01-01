/* Test of obstack_zprintf() and obstack_vzprintf() functions.
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

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include "signature.h"
SIGNATURE_CHECK (obstack_zprintf, ptrdiff_t,
                 (struct obstack *, char const *, ...));
SIGNATURE_CHECK (obstack_vzprintf, ptrdiff_t,
                 (struct obstack *, char const *, va_list));

#include "obstack.h"
#include "xalloc.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#define RETTYPE ptrdiff_t
#include "test-obstack-printf.h"

static ptrdiff_t
my_obstack_zprintf (struct obstack *obs, const char *format, ...)
{
  va_list args;
  ptrdiff_t ret;

  va_start (args, format);
  ret = obstack_vzprintf (obs, format, args);
  va_end (args);
  return ret;
}

int
main (int argc, char *argv[])
{
  test_function (my_obstack_zprintf);
  test_function (obstack_zprintf);
  return test_exit_status;
}
