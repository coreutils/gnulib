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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Get specification.  */
#include "snprintf.h"

/* Get vasnprintf.  */
#include "vasnprintf.h"

/* Get MIN. */
#include <minmax.h>

/* Print formatted output to string STR.  Similar to sprintf, but
   additional length SIZE limit how much is written into STR.  Returns
   string length of formatted string (which may be larger than SIZE).
   STR may be NULL, in which case nothing will be written.  On error,
   return a negative value. */
int
snprintf (char *str, size_t size, const char *format, ...)
{
  size_t len;
  char *out = vasnprintf (NULL, &len, format, args);

  if (!out)
    return -1;

  if (str)
    {
      memcpy (str, out, MIN (len + 1, size));
      str[size - 1] = '\0';
    }

  free (out);

  return len;
}
