/* Test of searching a string for a character outside a given set of characters.
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

  ASSERT (mbsspn ("Some text", "") == 0);

  ASSERT (mbsspn ("A long sentence", " ") == 0);
  ASSERT (mbsspn ("  xy", "aei ou") == 2);
  ASSERT (mbsspn ("eau", "aeiou") == 3);

  /* The following tests shows how mbsspn() is different from strspn().  */

  {
    const char input[] = "\303\266\303\274"; /* "öü" */
    ASSERT (mbsspn (input, "\303\266") == 2); /* "ö" */
    ASSERT (mbsspn (input, "\303\244") == 0); /* "ä" */
    ASSERT (mbsspn (input, "\303\274\303\266") == 4); /* "üö" */
    ASSERT (mbsspn (input, "\303\244\303\274") == 0); /* "äü" */
    ASSERT (mbsspn (input, "\303\244\303\266") == 2); /* "äö" */
  }

  {
    const char input[] = "\303\266\303\274"; /* "öü" */
    ASSERT (mbsspn (input, "\303") == 0); /* invalid multibyte sequence */
  }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  {
    const char input[] = "\341\200\341\200\240";
    ASSERT (mbsspn (input, "\341\200") == 2);
  }
  {
    const char input[] = "\341\200\341\200\240";
    ASSERT (mbsspn (input, "\341") == OR(0,1));
  }
  {
    const char input[] = "\341\341\200\240";
    ASSERT (mbsspn (input, "\341") == 1);
  }

  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  {
    const char input[] = "\360\221\222\360\221\222\240";
    ASSERT (mbsspn (input, "\360\221\222") == 3);
  }
  {
    const char input[] = "\360\221\222\360\221\222\240";
    ASSERT (mbsspn (input, "\360\221") == OR(0,2));
  }
  {
    const char input[] = "\360\221\360\221\222\240";
    ASSERT (mbsspn (input, "\360\221") == 2);
  }
  {
    const char input[] = "\360\221\222\360\221\222\240";
    ASSERT (mbsspn (input, "\360") == OR(0,1));
  }
  {
    const char input[] = "\360\221\360\221\222\240";
    ASSERT (mbsspn (input, "\360") == OR(0,1));
  }
  {
    const char input[] = "\360\360\221\222\240";
    ASSERT (mbsspn (input, "\360") == 1);
  }

  return test_exit_status;
}
