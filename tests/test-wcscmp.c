/* Test of wcscmp() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include <wchar.h>

#include "signature.h"
SIGNATURE_CHECK (wcscmp, int, (const wchar_t *, const wchar_t *));

#include "macros.h"

int
main (int argc, char *argv[])
{
  /* Test simple cases.  */
  {
    static const wchar_t input1[] = { 0 };
    static const wchar_t input2[] = { 0 };
    ASSERT (wcscmp (input1, input2) == 0);
  }
  {
    static const wchar_t input1[] = { 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 0 };
    ASSERT (wcscmp (input1, input2) < 0);
    ASSERT (wcscmp (input2, input1) > 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 0 };
    ASSERT (wcscmp (input1, input2) == 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'b', 'a', 'r', 0 };
    ASSERT (wcscmp (input1, input2) > 0);
    ASSERT (wcscmp (input2, input1) < 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 'b', 'a', 'r', 0 };
    ASSERT (wcscmp (input1, input2) < 0);
    ASSERT (wcscmp (input2, input1) > 0);
  }
  {
    static const wchar_t input1[] = { 'o', 'o', 'm', 'p', 'h', 0 };
    static const wchar_t input2[] = { 'o', 'o', 'p', 's', 0 };
    ASSERT (wcscmp (input1, input2) < 0);
    ASSERT (wcscmp (input2, input1) > 0);
  }

  /* ISO C requires wcscmp to work with all wchar_t values.
     ISO C 17 § 7.29.4.4 says:
       "Unless explicitly stated otherwise, the functions described in this
        subclause order two wide characters the same way as two integers of
        the underlying integer type designated by wchar_t."  */
  {
    static const wchar_t input1[] = { (wchar_t) 0x76547654, 0 };
    static const wchar_t input2[] = { (wchar_t) 0x9abc9abc, 0 };
    if ((wchar_t)-1 < 0)
      {
        /* wchar_t is signed.  */
        ASSERT (wcscmp (input1, input2) > 0);
        ASSERT (wcscmp (input2, input1) < 0);
      }
    else
      {
        /* wchar_t is unsigned.  */
        ASSERT (wcscmp (input1, input2) < 0);
        ASSERT (wcscmp (input2, input1) > 0);
      }
  }
  {
    static const wchar_t input1[] = { (wchar_t) 0x9abc9abc, 0 };
    static const wchar_t input2[] = { (wchar_t) 0x9bdf9bdf, 0 };
    ASSERT (wcscmp (input1, input2) < 0);
    ASSERT (wcscmp (input2, input1) > 0);
  }

  /* Comparing a negative wchar_t value against a null wchar_t.
     ISO C 17 § 7.29.4.4.1 says:
       "The wcscmp function compares the wide string pointed to by s1 to
        the wide string pointed to by s2."
     ISO C 17 § 7.1.1 defines the term "wide string":
       "A wide string is a contiguous sequence of wide characters terminated
        by and including the first null wide character."
     This means that the comparison extends up to and *including* the first
     null wchar_t.  */
  {
    static const wchar_t input1[] = { (wchar_t) 'x', 0 };
    static const wchar_t input2[] = { (wchar_t) 'x', (wchar_t) 0x9abc9abc, 0 };
    if ((wchar_t)-1 < 0)
      {
        /* wchar_t is signed.  */
        ASSERT (wcscmp (input1, input2) > 0);
        ASSERT (wcscmp (input2, input1) < 0);
      }
    else
      {
        /* wchar_t is unsigned.  */
        ASSERT (wcscmp (input1, input2) < 0);
        ASSERT (wcscmp (input2, input1) > 0);
      }
  }

  return test_exit_status;
}
