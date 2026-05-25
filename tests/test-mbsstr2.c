/* Test of searching in a string in a UTF-8 locale.
   Copyright (C) 2007-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <string.h>

#include <locale.h>
#include <stdlib.h>

#include "macros.h"

/* The mcel-based implementation of mbsnlen behaves differently than the
   original one.  Namely, for invalid/incomplete byte sequences:
   Where we ideally should have multi-byte-per-encoding-error (MEE) behaviour
   everywhere, mcel implements single-byte-per-encoding-error (SEE) behaviour.
   See <https://lists.gnu.org/archive/html/bug-gnulib/2023-07/msg00131.html>,
       <https://lists.gnu.org/archive/html/bug-gnulib/2023-07/msg00145.html>.
   Therefore, here we have different expected results, depending on the
   implementation.  */
#if GNULIB_MCEL_PREFER
# define OR(a,b) b
#else
# define OR(a,b) a
#endif

int
main ()
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  {
    const char input[] = "f\303\266\303\266";
    const char *result = mbsstr (input, "");
    ASSERT (result == input);
  }

  {
    const char input[] = "f\303\266\303\266";
    const char *result = mbsstr (input, "\303\266");
    ASSERT (result == input + 1);
  }

  {
    const char input[] = "f\303\266\303\266";
    const char *result = mbsstr (input, "\266\303");
    ASSERT (result == NULL);
  }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  {
    const char input[] = "f\341\200\341\200";
    const char *result = mbsstr (input, "");
    ASSERT (result == input);
  }
  {
    const char input[] = "f\341\200\341\200";
    const char *result = mbsstr (input, "\341\200");
    ASSERT (result == input + 1);
  }
  {
    const char input[] = "f\341\200\341\200";
    const char *result = mbsstr (input, "\200\341");
    ASSERT (result == OR (NULL, input + 2));
  }

  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "");
    ASSERT (result == input);
  }
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "\360\221\222");
    ASSERT (result == input + 1);
  }
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "\221\222\360\221");
    ASSERT (result == OR (NULL, input + 2));
  }
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "\221\222\360");
    ASSERT (result == OR (NULL, input + 2));
  }
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "\222\360\221");
    ASSERT (result == OR (NULL, input + 3));
  }
  {
    const char input[] = "f\360\221\222\360\221\222";
    const char *result = mbsstr (input, "\222\360");
    ASSERT (result == OR (NULL, input + 3));
  }
  {
    const char input[] = "f\360\221\360\221";
    const char *result = mbsstr (input, "");
    ASSERT (result == input);
  }
  {
    const char input[] = "f\360\221\360\221";
    const char *result = mbsstr (input, "\360\221");
    ASSERT (result == input + 1);
  }
  {
    const char input[] = "f\360\221\360\221";
    const char *result = mbsstr (input, "\221\360");
    ASSERT (result == OR (NULL, input + 2));
  }

  {
    const char input[] = "\303\204BC \303\204BCD\303\204B \303\204BCD\303\204BCD\303\204BDE"; /* "ÄBC ÄBCDÄB ÄBCDÄBCDÄBDE" */
    const char *result = mbsstr (input, "\303\204BCD\303\204BD"); /* "ÄBCDÄBD" */
    ASSERT (result == input + 19);
  }

  {
    const char input[] = "\303\204BC \303\204BCD\303\204B \303\204BCD\303\204BCD\303\204BDE"; /* "ÄBC ÄBCDÄB ÄBCDÄBCDÄBDE" */
    const char *result = mbsstr (input, "\303\204BCD\303\204BE"); /* "ÄBCDÄBE" */
    ASSERT (result == NULL);
  }

  /* Check that a very long haystack is handled quickly if the needle is
     short and occurs near the beginning.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    const char *needle =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    char *haystack = (char *) malloc (m + 1);
    if (haystack != NULL)
      {
        memset (haystack, 'A', m);
        haystack[0] = '\303'; haystack[1] = '\204';
        haystack[m] = '\0';

        for (; repeat > 0; repeat--)
          {
            ASSERT (mbsstr (haystack, needle) == haystack + 2);
          }

        free (haystack);
      }
  }

  /* Check that a very long needle is discarded quickly if the haystack is
     short.  */
  {
    size_t repeat = 10000;
    size_t m = 1000000;
    const char *haystack =
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207"
      "A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207"
      "A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207"
      "A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207"
      "A\303\207A\303\207A\303\207A\303\207A\303\207A\303\207";
    char *needle = (char *) malloc (m + 1);
    if (needle != NULL)
      {
        memset (needle, 'A', m);
        needle[m] = '\0';

        for (; repeat > 0; repeat--)
          {
            ASSERT (mbsstr (haystack, needle) == NULL);
          }

        free (needle);
      }
  }

  /* Check that the asymptotic worst-case complexity is not quadratic.  */
  {
    size_t m = 1000000;
    char *haystack = (char *) malloc (2 * m + 3);
    char *needle = (char *) malloc (m + 3);
    if (haystack != NULL && needle != NULL)
      {
        const char *result;

        memset (haystack, 'A', 2 * m);
        haystack[2 * m] = '\303'; haystack[2 * m + 1] = '\207';
        haystack[2 * m + 2] = '\0';

        memset (needle, 'A', m);
        needle[m] = '\303'; needle[m + 1] = '\207';
        needle[m + 2] = '\0';

        result = mbsstr (haystack, needle);
        ASSERT (result == haystack + m);
      }
    free (needle);
    free (haystack);
  }

  return test_exit_status;
}
