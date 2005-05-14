/* Formatted output to strings.
   Copyright (C) 2004 Free Software Foundation, Inc.
   Written by Simon Josefsson.

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

#include "snprintf.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "minmax.h"
#include "vasnprintf.h"

/* Print formatted output to string STR.  Similar to sprintf, but
   additional length SIZE limit how much is written into STR.  Returns
   string length of formatted string (which may be larger than SIZE).
   STR may be NULL, in which case nothing will be written.  On error,
   return a negative value. */
int
snprintf (char *str, size_t size, const char *format, ...)
{
  char *output;
  size_t len;
  va_list args;

  va_start (args, format);
  len = size;
  output = vasnprintf (str, &len, format, args);
  va_end (args);

  if (!output)
    return -1;

  if (str != NULL)
    if (len > size - 1) /* equivalent to: (size > 0 && len >= size) */
      str[size - 1] = '\0';

  if (output != str)
    free (output);

  return len;
}
