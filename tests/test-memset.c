/* Test of memset() function.
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
   In GCC < 15 this is a builtin that has the nonnull attribute.
   Some glibc versions use the nonnull attribute, which breaks this test.  */
static void *
null_memset (void *s, int c, size_t n)
{
  void *p = memset (s, c, n);
#if (! defined __GNUC__ || __GNUC__ >= 15 || defined __clang__) \
    && (! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__))
  ASSERT (s == NULL);
#endif
  return p;
}
static void *(*volatile volatile_null_memset) (void *, int, size_t)
  = null_memset;

/* Test the library, not the compiler+library.  */
static void *
lib_memset (void *s, int c, size_t n)
{
  return memset (s, c, n);
}
static void *(*volatile volatile_lib_memset) (void *, int, size_t)
  = lib_memset;
#undef memset
#define memset volatile_lib_memset

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */
#if (! defined __GNUC__ || __GNUC__ >= 15 || defined __clang__) \
    && (! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__))
  ASSERT (memset (NULL, '?', 0) == NULL);

  ASSERT (volatile_null_memset (NULL, '?', 0) == NULL);
#endif

  return test_exit_status;
}
