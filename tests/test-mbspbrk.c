/* Test of searching a string for a character among a given set of characters.
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

  ASSERT (mbspbrk ("Some text", "") == NULL);

  {
    const char input[] = "A long sentence";
    ASSERT (mbspbrk (input, "aeiou") == input + 3);
    ASSERT (mbspbrk (input, "iI") == NULL);
  }

  /* The following tests shows how mbspbrk() is different from strpbrk().  */

  {
    const char input[] = "B\303\266se B\303\274bchen"; /* "Böse Bübchen" */
    ASSERT (mbspbrk (input, "\303\244\303\274") == input + 7); /* "äü" */
  }

  {
    const char input[] = "B\303\266se B\303\274bchen"; /* "Böse Bübchen" */
    ASSERT (mbspbrk (input, "\303") == NULL); /* invalid multibyte sequence */
  }

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */

  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  {
    const char input[] = "\341\200\240x\341\200y";
    ASSERT (mbspbrk (input, "\341\200") == input + 4);
  }
  {
    const char input[] = "\341\200\240x\341\200";
    ASSERT (mbspbrk (input, "\341\200") == input + 4);
  }
  {
    const char input[] = "\341\200\240x\341\200";
    ASSERT (mbspbrk (input, "\341") == OR (NULL, input + 4));
  }
  {
    const char input[] = "\341\200\240x\341y";
    ASSERT (mbspbrk (input, "\341") == input + 4);
  }
  {
    const char input[] = "\341\200\240x\341";
    ASSERT (mbspbrk (input, "\341") == input + 4);
  }

  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  {
    const char input[] = "\360\221\222\240x\360\221\222y";
    ASSERT (mbspbrk (input, "\360\221\222") == input + 5);
  }
  {
    const char input[] = "\360\221\222\240x\360\221\222";
    ASSERT (mbspbrk (input, "\360\221\222") == input + 5);
  }
  {
    const char input[] = "\360\221\222\240x\360\221\222";
    ASSERT (mbspbrk (input, "\360\221") == OR (NULL, input + 5));
  }
  {
    const char input[] = "\360\221\222\240x\360\221y";
    ASSERT (mbspbrk (input, "\360\221") == input + 5);
  }
  {
    const char input[] = "\360\221\222\240x\360\221";
    ASSERT (mbspbrk (input, "\360\221") == input + 5);
  }
  {
    const char input[] = "\360\221\222\240x\360\221\222";
    ASSERT (mbspbrk (input, "\360") == OR (NULL, input + 5));
  }
  {
    const char input[] = "\360\221\222\240x\360\221";
    ASSERT (mbspbrk (input, "\360") == OR (NULL, input + 5));
  }
  {
    const char input[] = "\360\221\222\240x\360y";
    ASSERT (mbspbrk (input, "\360") == input + 5);
  }
  {
    const char input[] = "\360\221\222\240x\360";
    ASSERT (mbspbrk (input, "\360") == input + 5);
  }

  return test_exit_status;
}
