/* Test of strndup() function.
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

#include <config.h>

/* Specification.  */
#include <string.h>

#include <stddef.h>

#include "macros.h"

/* Test the library, not the compiler+library.  */
static char *
lib_strndup (char const *s, size_t size)
{
  return strndup (s, size);
}
static char *(*volatile volatile_strndup) (char const *, size_t)
  = lib_strndup;
#undef strndup
#define strndup volatile_strndup

int
main (void)
{
  /* Test zero-length operations on NULL pointers, allowed by
     <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3322.pdf>.  */
  ASSERT (strndup (NULL, 0) != NULL);

  return test_exit_status;
}
