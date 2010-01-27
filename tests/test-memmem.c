/*
 * Copyright (C) 2004, 2007-2010 Free Software Foundation, Inc.
 * Written by Bruno Haible and Eric Blake
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <string.h>

#include "signature.h"
SIGNATURE_CHECK (memmem, void *, (void const *, size_t, void const *, size_t));

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "zerosize-ptr.h"
#include "macros.h"

static void *
null_ptr (void)
{
  return NULL;
}

int
main (int argc, char *argv[])
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  All known platforms that lack alarm also lack
     memmem, and the replacement memmem is known to not take too
     long.  */
  signal (SIGALRM, SIG_DFL);
  alarm (100);
#endif

  {
    const char input[] = "foo";
    const char *result = memmem (input, strlen (input), "", 0);
    ASSERT (result == input);
  }

  {
    const char input[] = "foo";
    const char *result = memmem (input, strlen (input), "o", 1);
    ASSERT (result == input + 1);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = memmem (input, strlen (input), "ABCDABD", 7);
    ASSERT (result == input + 15);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = memmem (input, strlen (input), "ABCDABE", 7);
    ASSERT (result == NULL);
  }

  {
    const char input[] = "ABC ABCDAB ABCDABCDABDE";
    const char *result = memmem (input, strlen (input), "ABCDABCD", 8);
    ASSERT (result == input + 11);
  }

  /* Check that length 0 does not dereference the pointer.  */
  {
    const char *result = memmem (zerosize_ptr (), 0, "foo", 3);
    ASSERT (result == NULL);
  }

  {
    const char input[] = "foo";
    const char *result = memmem (input, strlen (input), null_ptr (), 0);
    ASSERT (result == input);
  }

  /* Check that a very long haystack is handled quickly if the needle is
     short and occurs near the beginning.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    char *needle =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    size_t n = strlen (needle);
    char *haystack = (char *) malloc (m + 1);
    if (haystack != NULL)
      {
        memset (haystack, 'A', m);
        haystack[0] = 'B';

        for (; repeat > 0; repeat--)
          {
            ASSERT (memmem (haystack, m, needle, n) == haystack + 1);
          }

        free (haystack);
      }
  }

  /* Check that a very long needle is discarded quickly if the haystack is
     short.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    char *haystack =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
    size_t n = strlen (haystack);
    char *needle = (char *) malloc (m + 1);
    if (needle != NULL)
      {
        memset (needle, 'A', m);

        for (; repeat > 0; repeat--)
          {
            ASSERT (memmem (haystack, n, needle, m) == NULL);
          }

        free (needle);
      }
  }

  /* Check that the asymptotic worst-case complexity is not quadratic.  */
  {
    size_t m = 1000000;
    char *haystack = (char *) malloc (2 * m + 1);
    char *needle = (char *) malloc (m + 1);
    if (haystack != NULL && needle != NULL)
      {
        const char *result;

        memset (haystack, 'A', 2 * m);
        haystack[2 * m] = 'B';

        memset (needle, 'A', m);
        needle[m] = 'B';

        result = memmem (haystack, 2 * m + 1, needle, m + 1);
        ASSERT (result == haystack + m);
      }
    free (needle);
    free (haystack);
  }

  /* Check that long needles not present in a haystack can be handled
     with sublinear speed.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    size_t n = 1000;
    char *haystack = (char *) malloc (m);
    char *needle = (char *) malloc (n);
    if (haystack != NULL && needle != NULL)
      {
        const char *result;

        memset (haystack, 'A', m);
        memset (needle, 'B', n);

        for (; repeat > 0; repeat--)
          {
            result = memmem (haystack, m, needle, n);
            ASSERT (result == NULL);
          }
      }
    free (haystack);
    free (needle);
  }

  return 0;
}
