/* Test of POSIX and GNU compatible vszprintf() and szprintf() functions.
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
   visible on platforms relevant in 2024.  For a much more complete test suite,
   see test-sprintf-posix.h.  */

static void
test_function (ptrdiff_t (*my_szprintf) (char *, const char *, ...))
{
  char result[5000];

  /* Test the support of the 'B' conversion specifier for binary output of
     integers.  */

  { /* This test would fail on all platforms other than glibc ≥ 2.35.  */
    ptrdiff_t retval =
      my_szprintf (result, "%#B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "0B11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }
}
