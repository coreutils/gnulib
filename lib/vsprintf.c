/* Formatted output to strings.
   Copyright (C) 2004, 2006-2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>

#include "vasnprintf.h"

/* Some systems, like OSF/1 4.0 and Woe32, don't have EOVERFLOW.  */
#ifndef EOVERFLOW
# define EOVERFLOW E2BIG
#endif

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif

/* Print formatted output to string STR.
   Return string length of formatted string.  On error, return a negative
   value.  */
int
vsprintf (char *str, const char *format, va_list args)
{
  char *output;
  size_t len;
  size_t lenbuf = SIZE_MAX;

  output = vasnprintf (str, &lenbuf, format, args);
  len = lenbuf;

  if (!output)
    return -1;

  if (output != str)
    {
      /* len is near SIZE_MAX.  */
      free (output);
      errno = EOVERFLOW;
      return -1;
    }

  if (len > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }

  return len;
}
