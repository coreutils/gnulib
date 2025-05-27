/* Test of <stddef.h> substitute in C++ mode.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

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

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include <cstddef>

/* Test the expected contents of <cstddef>, cf.
   <https://en.cppreference.com/w/cpp/header/cstddef>.  */

/* Check that appropriate types are defined.  */
ptrdiff_t b2 = 1;
size_t c2 = 2;

#if !(defined __cplusplus && defined _MSC_VER)
/* Check that NULL can be passed through varargs as a pointer type,
   per POSIX 2008.  */
static_assert (sizeof NULL == sizeof (void *));
#endif

/* Check that offsetof produces integer constants with correct type.  */
struct d
{
  char e;
  char f;
};
static_assert (sizeof (offsetof (struct d, e)) == sizeof (size_t));
static_assert (offsetof (struct d, f) == 1);

#include <limits.h> /* INT_MAX */

/* offsetof promotes to an unsigned integer if and only if sizes do
   not fit in int.  */
static_assert ((offsetof (struct d, e) < -1) == (INT_MAX < (size_t) -1));
