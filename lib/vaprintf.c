/* Simplified variant of vasprintf and asprintf.
   Copyright (C) 1999, 2002-2004, 2006-2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Checked size_t computations.  */
#include "xsize.h"

static char *
astrcat (size_t argcount, va_list args)
{
  /* Determine the total size.  */
  size_t totalsize = 0;
  {
    va_list ap;
    va_copy (ap, args);
    for (size_t i = argcount; i > 0; i--)
      {
        const char *next = va_arg (ap, const char *);
        totalsize = xsum (totalsize, strlen (next));
      }
    va_end (ap);
  }

  /* Test for overflow in the summing pass above or in (totalsize + 1)
     below.  */
  if (totalsize == SIZE_MAX)
    {
      errno = ENOMEM;
      return NULL;
    }

  /* Allocate and fill the result string.  */
  char *result = (char *) malloc (totalsize + 1);
  if (result == NULL)
    return NULL;
  {
    char *p = result;
    for (size_t i = argcount; i > 0; i--)
      {
        const char *next = va_arg (args, const char *);
        size_t len = strlen (next);
        memcpy (p, next, len);
        p += len;
      }
    *p = '\0';
  }

  return result;
}

char *
vaprintf (const char *format, va_list args)
{
  /* Recognize the special case format = "%s...%s".  It is a frequently used
     idiom for string concatenation and needs to be fast.  We don't want to
     have a separate function astrcat() for this purpose.  */
  {
    size_t argcount = 0;

    for (const char *f = format;;)
      {
        if (*f == '\0')
          /* Recognized the special case of string concatenation.  */
          return astrcat (argcount, args);
        if (*f != '%')
          break;
        f++;
        if (*f != 's')
          break;
        f++;
        argcount++;
      }
  }

  char *result;
  if (vaszprintf (&result, format, args) < 0)
    return NULL;

  return result;
}
