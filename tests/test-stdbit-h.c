/* Test of <stdbit.h> substitute.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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

#include <stdbit.h>

/* Check that __STDC_ENDIAN_BIG__ and __STDC_ENDIAN_LITTLE__ are defined.  */
int a[2] = { __STDC_ENDIAN_BIG__, __STDC_ENDIAN_LITTLE__ };

/* Check that __STDC_ENDIAN_NATIVE__ is defined.  */
int b = __STDC_ENDIAN_NATIVE__;

/* Check that __STDC_ENDIAN_NATIVE__ is either __STDC_ENDIAN_BIG__ or
   __STDC_ENDIAN_LITTLE__.  */
#if !(__STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_BIG__ \
      || __STDC_ENDIAN_NATIVE__ == __STDC_ENDIAN_LITTLE__)
# error "unknown endianness"
#endif

int
main (void)
{
  return 0;
}
