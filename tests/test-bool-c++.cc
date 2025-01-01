/* Test of <stdbool.h> substitute in C++ mode.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#ifdef TEST_STDBOOL_H
# include <stdbool.h>
#endif


/* These tests are a subset of the C language tests in test-stdbool.c.  */

#if false
 "error: false is not 0"
#endif
#if true != 1
 "error: true is not 1"
#endif

struct s { bool u: 1; bool v; } s;

char a[true == 1 ? 1 : -1];
char b[false == 0 ? 1 : -1];
char c[(unsigned char) true == 1 ? 1 : -1];
char d[(unsigned char) false == 0 ? 1 : -1];


int
main ()
{
}
