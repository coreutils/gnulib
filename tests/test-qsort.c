/* Test of qsort() function.
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
#include <stdlib.h>

#include "macros.h"

/* Test the prototype in <stdlib.h> + compiler.
   Some glibc versions use the nonnull attribute, which breaks this test.  */
static void
null_qsort (void *base, size_t nel, size_t width,
            int (*compar) (void const *, void const *))
{
  qsort (base, nel, width, compar);
#if ! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__)
  ASSERT (base == NULL);
#endif
}
static void (*volatile volatile_null_qsort) (void *, size_t, size_t,
                                             int (*) (void const *, void const *))
  = null_qsort;

/* Test the library, not the compiler+library.  */
static void
lib_qsort (void *base, size_t nel, size_t width,
           int (*compar) (void const *, void const *))
{
  qsort (base, nel, width, compar);
}
static void (*volatile volatile_lib_qsort) (void *, size_t, size_t,
                                            int (*) (void const *, void const *))
  = lib_qsort;
#undef qsort
#define qsort volatile_lib_qsort

static int
cmp (const void *a, const void *b)
{
  return 0;
}

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */

#if ! defined __GLIBC__ || 2 < __GLIBC__ + (99 <= __GLIBC_MINOR__)
  qsort (NULL, 0, 1, cmp);

  volatile_null_qsort (NULL, 0, 1, cmp);
#endif

  return 0;
}
