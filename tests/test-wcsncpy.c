/* Test of wcsncpy() function.
   Copyright (C) 2024-2026 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <wchar.h>

#include <stddef.h>

#include "macros.h"

/* Test the prototype in <wchar.h> + compiler.
   Some glibc versions use the nonnull attribute, which breaks this test.  */
static wchar_t *
null_wcsncpy (wchar_t *ws1, wchar_t const *ws2, size_t n)
{
  wchar_t *p = wcsncpy (ws1, ws2, n);
#if ! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__)
  ASSERT (ws2 == NULL);
#endif
  return p;
}

/* Test the library, not the compiler+library.  */
static wchar_t *
lib_wcsncpy (wchar_t *ws1, wchar_t const *ws2, size_t n)
{
  return wcsncpy (ws1, ws2, n);
}
static wchar_t *(*volatile volatile_wcsncpy) (wchar_t *, wchar_t const *,
                                              size_t)
  = lib_wcsncpy;
#undef wcsncpy
#define wcsncpy volatile_wcsncpy

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */

  ASSERT (wcsncpy (NULL, L"x", 0) == NULL);

  {
    wchar_t y[1];
    ASSERT (wcsncpy (y, NULL, 0) == y);

    volatile_wcsncpy = null_wcsncpy;
    ASSERT (wcsncpy (y, NULL, 0) == y);
  }

  return test_exit_status;
}
