/* Formatted output to a stream.
   Copyright (C) 2004, 2006-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  */
#include <stdio.h>

#include <errno.h>
#include <limits.h>
#include <stdarg.h>

#include "fseterr.h"
#include "intprops.h"

int
fprintf (FILE *fp, const char *format, ...)
{
  va_list args;
  off64_t ret;

  va_start (args, format);
  ret = vfzprintf (fp, format, args);
  va_end (args);

  if (TYPE_MAXIMUM (off64_t) > INT_MAX && ret > INT_MAX)
    {
      fseterr (fp);
      errno = EOVERFLOW;
      return -1;
    }

  return ret;
}
