/* Test of determining the number of multibyte characters in a string.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

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

  ASSERT (mbslen ("") == 0);
  ASSERT (mbslen ("Hello") == 5);

  /* The following tests shows how mbslen() is different from strlen().  */
  ASSERT (mbslen ("\303\244\303\266") == 2); /* "äö" */
  ASSERT (mbslen ("7\342\202\254") == 2); /* "7€" */
  ASSERT (mbslen ("\360\237\220\203") == 1); /* "🐃" */

  /* Incomplete characters.  See
     https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf
     page 128 table 3-11.  */
  ASSERT (mbslen ("\303") == 1);
  /* "\341\200\240" = 0xE1 0x80 0xA0 = U+1020.  */
  ASSERT (mbslen ("\341\200") == OR(1,2));
  ASSERT (mbslen ("\341") == 1);
  /* "\360\221\222\240" = 0xF0 0x91 0x92 0xA0 = U+114A0.  */
  ASSERT (mbslen ("\360\221\222") == OR(1,3));
  ASSERT (mbslen ("\360\221") == OR(1,2));
  ASSERT (mbslen ("\360") == 1);

  return test_exit_status;
}
