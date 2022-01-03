/* Formatted output to strings in C locale.
   Copyright (C) 2004, 2006-2022 Free Software Foundation, Inc.
   Written by Simon Josefsson and Paul Eggert.
   Modified for C locale by Ben Pfaff.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "c-vasnprintf.h"

/* Print formatted output to string STR.  Similar to sprintf, but
   additional length SIZE limit how much is written into STR.  Returns
   string length of formatted string (which may be larger than SIZE).
   STR may be NULL, in which case nothing will be written.  On error,
   return a negative value.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
int
c_snprintf (char *str, size_t size, const char *format, ...)
{
  char *output;
  size_t len;
  size_t lenbuf = size;
  va_list args;

  va_start (args, format);
  output = c_vasnprintf (str, &lenbuf, format, args);
  len = lenbuf;
  va_end (args);

  if (!output)
    return -1;

  if (output != str)
    {
      if (size)
        {
          size_t pruned_len = (len < size ? len : size - 1);
          memcpy (str, output, pruned_len);
          str[pruned_len] = '\0';
        }

      free (output);
    }

  if (INT_MAX < len)
    {
      errno = EOVERFLOW;
      return -1;
    }

  return len;
}
