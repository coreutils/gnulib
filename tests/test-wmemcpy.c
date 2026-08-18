/* Test of wmemcpy() function.
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

/* Test the prototype in <wchar.h> + compiler.  */
static wchar_t *
null_wmemcpy (wchar_t *s1, wchar_t const *s2, size_t n)
{
  wchar_t *p = wmemcpy (s1, s2, n);
  ASSERT (s1 == NULL);
  return p;
}
static wchar_t *(*volatile volatile_null_wmemcpy) (wchar_t *, wchar_t const *,
                                                   size_t)
  = null_wmemcpy;

/* Test the library, not the compiler+library.  */
static wchar_t *
lib_wmemcpy (wchar_t *s1, wchar_t const *s2, size_t n)
{
  return wmemcpy (s1, s2, n);
}
static wchar_t *(*volatile volatile_lib_wmemcpy) (wchar_t *, wchar_t const *,
                                                  size_t)
  = lib_wmemcpy;
#undef wmemcpy
#define wmemcpy volatile_lib_wmemcpy

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.
     In mingw-w64 14.0.0, wmemcpy is an inline function that calls memcpy.
     In GCC < 15, memcpy is a builtin that has the nonnull attribute.  */

#if ! (defined __MINGW32__ \
       && defined __GNUC__ && !defined __clang__ && __GNUC__ < 15)
  ASSERT (wmemcpy (NULL, L"x", 0) == NULL);

  {
    wchar_t y[1];
    ASSERT (wmemcpy (y, NULL, 0) == y);
  }

  ASSERT (volatile_null_wmemcpy (NULL, L"x", 0) == NULL);
#endif

  return test_exit_status;
}
