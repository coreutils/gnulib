/* Test of POSIX compatible obstack_[v]zprintf() functions.
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
#include "infinity.h"
#include "nan.h"

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

/* Test whether string[start_index..end_index-1] is a valid textual
   representation of NaN.  */
static int
strisnan (const char *string, size_t start_index, size_t end_index, int uppercase)
{
  if (start_index < end_index)
    {
      if (string[start_index] == '-')
        start_index++;
      if (start_index + 3 <= end_index
          && memcmp (string + start_index, uppercase ? "NAN" : "nan", 3) == 0)
        {
          start_index += 3;
          if (start_index == end_index
              || (string[start_index] == '(' && string[end_index - 1] == ')'))
            return 1;
        }
    }
  return 0;
}

static void
test_function (ptrdiff_t (*my_obstack_zprintf) (struct obstack *, const char *, ...))
{
  struct obstack obs;
  obstack_init (&obs);

  #define RESULT_EQ(expected) \
    (len == strlen (expected) && memcmp (result, expected, len) == 0)

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* This test would fail on Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("0x1.922p+1 33")
            || RESULT_EQ ("0x3.244p+0 33")
            || RESULT_EQ ("0x6.488p-1 33")
            || RESULT_EQ ("0xc.91p-2 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("-0X1.922P+1 33")
            || RESULT_EQ ("-0X3.244P+0 33")
            || RESULT_EQ ("-0X6.488P-1 33")
            || RESULT_EQ ("-0XC.91P-2 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on FreeBSD 6.1, NetBSD 10.0. */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%.2a %d", 1.51, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("0x1.83p+0 33")
            || RESULT_EQ ("0x3.05p-1 33")
            || RESULT_EQ ("0x6.0ap-2 33")
            || RESULT_EQ ("0xc.14p-3 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on macOS 14, FreeBSD 14.0, OpenBSD 7.5, AIX 7.3,
       Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%.0a %d", 1.51, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("0x2p+0 33")
            || RESULT_EQ ("0x3p-1 33")
            || RESULT_EQ ("0x6p-2 33")
            || RESULT_EQ ("0xcp-3 33"));
    obstack_free (&obs, result);
  }

  /* Test the support of the %f format directive.  */

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%f %d", Infinityd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("inf 33")
            || RESULT_EQ ("infinity 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%f %d", NaNd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (len >= 3 + 3
            && strisnan (result, 0, len - 3, 0)
            && memcmp (result + len - 3, " 33", 3) == 0);
    obstack_free (&obs, result);
  }

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%010f %d", Infinityd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("       inf 33")
            || RESULT_EQ ("  infinity 33"));
    obstack_free (&obs, result);
  }

  /* Test the support of the %e format directive.  */

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%e %d", Infinityd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("inf 33")
            || RESULT_EQ ("infinity 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%e %d", NaNd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (len >= 3 + 3
            && strisnan (result, 0, len - 3, 0)
            && memcmp (result + len - 3, " 33", 3) == 0);
    obstack_free (&obs, result);
  }

  /* Test the support of the %g format directive.  */

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%g %d", Infinityd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("inf 33")
            || RESULT_EQ ("infinity 33"));
    obstack_free (&obs, result);
  }

  { /* This test would fail on AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%g %d", NaNd (), 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (len >= 3 + 3
            && strisnan (result, 0, len - 3, 0)
            && memcmp (result + len - 3, " 33", 3) == 0);
    obstack_free (&obs, result);
  }

  /* Test the support of large precision.  */

  { /* This test would fail on AIX 7.1.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%.4000d %d", 1234567, 99);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (len == 4000 + 3);
    size_t i;
    for (i = 0; i < 4000 - 7; i++)
      ASSERT (result[i] == '0');
    ASSERT (memcmp (result + 4000 - 7, "1234567 99", 7 + 3) == 0);
    obstack_free (&obs, result);
  }

  /* Test the support of the 'b' conversion specifier for binary output of
     integers.  */

  { /* This test would fail on glibc 2.34, musl libc, macOS 14,
       FreeBSD 13.2, NetBSD 10.0, OpenBSD 7.5, AIX 7.3, Solaris 11.4.  */
    ptrdiff_t len =
      my_obstack_zprintf (&obs, "%b %d", 12345, 33, 44, 55);
    ASSERT (len >= 0);
    char *result = obstack_finish (&obs);
    ASSERT (RESULT_EQ ("11000000111001 33"));
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
