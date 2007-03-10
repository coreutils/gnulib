/* Test of POSIX compatible fprintf() function.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) if (!(expr)) abort ();

#include "test-printf-posix.h"

int
main (int argc, char *argv[])
{
  test_function (printf);
  return 0;
}

/* Test whether __attribute__ (__format__ (...)) still works.  */
#if (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)) && !__STRICT_ANSI__
extern int func1 (char *, size_t, const char *, ...)
     __attribute__ ((__format__ (__printf__, 3, 4)));
extern int func2 (char *, size_t, const char *, ...)
     __attribute__ ((__format__ (printf, 3, 4)));
extern int func3 (char *, size_t, const char *, ...)
     __attribute__ ((format (__printf__, 3, 4)));
extern int func4 (char *, size_t, const char *, ...)
     __attribute__ ((format (printf, 3, 4)));
#endif
