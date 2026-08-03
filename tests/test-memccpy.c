/* Test of memccpy() function.
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
#include <string.h>

#include <stddef.h>

#include "macros.h"

/* Test the prototype in <string.h> + compiler.
   Some glibc versions use the nonnull attribute, which breaks this test.  */
static void *
null_memccpy (void *dest, const void *src, int c, size_t n)
{
  void *p = memccpy (dest, src, c, n);
#if ! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__)
  ASSERT (dest == NULL);
#endif
  return p;
}

/* Test the library, not the compiler+library.  */
static void *
lib_memccpy (void *dest, void const *src, int c, size_t n)
{
  return memccpy (dest, src, c, n);
}
static void *(*volatile volatile_memccpy) (void *, void const *, int, size_t)
  = lib_memccpy;
#undef memccpy
#define memccpy volatile_memccpy

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */

  ASSERT (memccpy (NULL, "x", '?', 0) == NULL);

  {
    char y[1];
    ASSERT (memccpy (y, NULL, '?', 0) == NULL);
  }

  volatile_memccpy = null_memccpy;
  ASSERT (memccpy (NULL, "x", '?', 0) == NULL);

  return test_exit_status;
}
