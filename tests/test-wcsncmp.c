/* Test of wcsncmp() function.
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
SIGNATURE_CHECK (wcsncmp, int, (const wchar_t *, const wchar_t *, size_t));

#include "macros.h"

/* Test the library, not the compiler+library.  */
static int
lib_wcsncmp (wchar_t const *ws1, wchar_t const *ws2, size_t n)
{
  return wcsncmp (ws1, ws2, n);
}
static int (*volatile volatile_wcsncmp) (wchar_t const *,
                                         wchar_t const *, size_t)
  = lib_wcsncmp;
#undef wcsncmp
#define wcsncmp volatile_wcsncmp

int
main (int argc, char *argv[])
{
  /* Test simple cases.  */
  {
    static const wchar_t input1[] = { 0 };
    static const wchar_t input2[] = { 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) == 0);
    ASSERT (wcsncmp (input1, input2, 1000000) == 0);
  }
  {
    static const wchar_t input1[] = { 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) < 0);
    ASSERT (wcsncmp (input2, input1, 1) > 0);
    ASSERT (wcsncmp (input1, input2, 3) < 0);
    ASSERT (wcsncmp (input2, input1, 3) > 0);
    ASSERT (wcsncmp (input1, input2, 4) < 0);
    ASSERT (wcsncmp (input2, input1, 4) > 0);
    ASSERT (wcsncmp (input1, input2, 1000000) < 0);
    ASSERT (wcsncmp (input2, input1, 1000000) > 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) == 0);
    ASSERT (wcsncmp (input1, input2, 2) == 0);
    ASSERT (wcsncmp (input1, input2, 3) == 0);
    ASSERT (wcsncmp (input1, input2, 4) == 0);
    ASSERT (wcsncmp (input1, input2, 1000000) == 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'b', 'a', 'r', 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) > 0);
    ASSERT (wcsncmp (input2, input1, 1) < 0);
    ASSERT (wcsncmp (input1, input2, 2) > 0);
    ASSERT (wcsncmp (input2, input1, 2) < 0);
    ASSERT (wcsncmp (input1, input2, 1000000) > 0);
    ASSERT (wcsncmp (input2, input1, 1000000) < 0);
  }
  {
    static const wchar_t input1[] = { 'f', 'o', 'o', 0 };
    static const wchar_t input2[] = { 'f', 'o', 'o', 'b', 'a', 'r', 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) == 0);
    ASSERT (wcsncmp (input1, input2, 2) == 0);
    ASSERT (wcsncmp (input1, input2, 3) == 0);
    ASSERT (wcsncmp (input1, input2, 4) < 0);
    ASSERT (wcsncmp (input2, input1, 4) > 0);
    ASSERT (wcsncmp (input1, input2, 1000000) < 0);
    ASSERT (wcsncmp (input2, input1, 1000000) > 0);
  }
  {
    static const wchar_t input1[] = { 'o', 'o', 'm', 'p', 'h', 0 };
    static const wchar_t input2[] = { 'o', 'o', 'p', 's', 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) == 0);
    ASSERT (wcsncmp (input1, input2, 2) == 0);
    ASSERT (wcsncmp (input1, input2, 3) < 0);
    ASSERT (wcsncmp (input2, input1, 3) > 0);
    ASSERT (wcsncmp (input1, input2, 4) < 0);
    ASSERT (wcsncmp (input2, input1, 4) > 0);
    ASSERT (wcsncmp (input1, input2, 5) < 0);
    ASSERT (wcsncmp (input2, input1, 5) > 0);
    ASSERT (wcsncmp (input1, input2, 6) < 0);
    ASSERT (wcsncmp (input2, input1, 6) > 0);
    ASSERT (wcsncmp (input1, input2, 1000000) < 0);
    ASSERT (wcsncmp (input2, input1, 1000000) > 0);
  }

  /* ISO C requires wcsncmp to work with all wchar_t values.
     ISO C 17 § 7.29.4.4 says:
       "Unless explicitly stated otherwise, the functions described in this
        subclause order two wide characters the same way as two integers of
        the underlying integer type designated by wchar_t."  */
  {
    static const wchar_t input1[] = { (wchar_t) 0x76547654, 0 };
    static const wchar_t input2[] = { (wchar_t) 0x9abc9abc, 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    if ((wchar_t)-1 < 0)
      {
        /* wchar_t is signed.  */
        ASSERT (wcsncmp (input1, input2, 1) > 0);
        ASSERT (wcsncmp (input2, input1, 1) < 0);
        ASSERT (wcsncmp (input1, input2, 2) > 0);
        ASSERT (wcsncmp (input2, input1, 2) < 0);
        ASSERT (wcsncmp (input1, input2, 1000000) > 0);
        ASSERT (wcsncmp (input2, input1, 1000000) < 0);
      }
    else
      {
        /* wchar_t is unsigned.  */
        ASSERT (wcsncmp (input1, input2, 1) < 0);
        ASSERT (wcsncmp (input2, input1, 1) > 0);
        ASSERT (wcsncmp (input1, input2, 2) < 0);
        ASSERT (wcsncmp (input2, input1, 2) > 0);
        ASSERT (wcsncmp (input1, input2, 1000000) < 0);
        ASSERT (wcsncmp (input2, input1, 1000000) > 0);
      }
  }
  {
    static const wchar_t input1[] = { (wchar_t) 0x9abc9abc, 0 };
    static const wchar_t input2[] = { (wchar_t) 0x9bdf9bdf, 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) < 0);
    ASSERT (wcsncmp (input2, input1, 1) > 0);
    ASSERT (wcsncmp (input1, input2, 2) < 0);
    ASSERT (wcsncmp (input2, input1, 2) > 0);
    ASSERT (wcsncmp (input1, input2, 1000000) < 0);
    ASSERT (wcsncmp (input2, input1, 1000000) > 0);
  }

  /* Comparing a negative wchar_t value against a null wchar_t.
     ISO C 17 § 7.29.4.4.1 says:
       "The wcsncmp function compares the wide string pointed to by s1 to
        the wide string pointed to by s2."
     ISO C 17 § 7.1.1 defines the term "wide string":
       "A wide string is a contiguous sequence of wide characters terminated
        by and including the first null wide character."
     This means that the comparison extends up to and *including* the first
     null wchar_t.  */
  {
    static const wchar_t input1[] = { (wchar_t) 'x', 0 };
    static const wchar_t input2[] = { (wchar_t) 'x', (wchar_t) 0x9abc9abc, 0 };
    ASSERT (wcsncmp (input1, input2, 0) == 0);
    ASSERT (wcsncmp (input1, input2, 1) == 0);
    if ((wchar_t)-1 < 0)
      {
        /* wchar_t is signed.  */
        ASSERT (wcsncmp (input1, input2, 2) > 0);
        ASSERT (wcsncmp (input2, input1, 2) < 0);
        ASSERT (wcsncmp (input1, input2, 3) > 0);
        ASSERT (wcsncmp (input2, input1, 3) < 0);
        ASSERT (wcsncmp (input1, input2, 1000000) > 0);
        ASSERT (wcsncmp (input2, input1, 1000000) < 0);
      }
    else
      {
        /* wchar_t is unsigned.  */
        ASSERT (wcsncmp (input1, input2, 2) < 0);
        ASSERT (wcsncmp (input2, input1, 2) > 0);
        ASSERT (wcsncmp (input1, input2, 3) < 0);
        ASSERT (wcsncmp (input2, input1, 3) > 0);
        ASSERT (wcsncmp (input1, input2, 1000000) < 0);
        ASSERT (wcsncmp (input2, input1, 1000000) > 0);
      }
  }

  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */
  ASSERT (wcsncmp (NULL, L"x", 0) == 0);
  ASSERT (wcsncmp (L"x", NULL, 0) == 0);
  ASSERT (wcsncmp (NULL, NULL, 0) == 0);

  return test_exit_status;
}
