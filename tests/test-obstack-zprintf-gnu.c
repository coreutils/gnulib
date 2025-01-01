/* Test of POSIX and GNU compatible obstack_[v]zprintf() functions.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

/* This test exercises only a few POSIX compliance problems that are still
   visible on platforms relevant in 2024.  */

#include <config.h>

#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "obstack.h"

#include "macros.h"

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

static void
test_function (ptrdiff_t (*my_obstack_zprintf) (struct obstack *, const char *, ...))
{
  struct obstack obs;
  obstack_init (&obs);

  #define RESULT_EQ(expected) \
    (len == strlen (expected) && memcmp (result, expected, len) == 0)

  /* Test the support of the 'B' conversion specifier for binary output of
     integers.  */

  { /* This test would fail on all platforms other than glibc ≥ 2.35.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%#B %d", 12345, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("0B11000000111001 33"));
    obstack_free (&obs, result);
  }

  obstack_free (&obs, NULL);
}

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

static void
test_obstack_vzprintf ()
{
  test_function (my_obstack_zprintf);
}

static void
test_obstack_zprintf ()
{
  test_function (obstack_zprintf);
}

int
main (int argc, char *argv[])
{
  test_obstack_vzprintf ();
  test_obstack_zprintf ();
  return test_exit_status;
}
