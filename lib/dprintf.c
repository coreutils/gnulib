/* Formatted output to a file descriptor.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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
#include <stdlib.h>

#include "full-write.h"
#include "vasnprintf.h"

int
dprintf (int fd, const char *format, ...)
{
  char buf[2000];
  char *output;
  size_t len;
  size_t lenbuf = sizeof (buf);
  va_list args;

  va_start (args, format);
  output = vasnprintf (buf, &lenbuf, format, args);
  len = lenbuf;
  va_end (args);

  if (!output)
    return -1;

  if (full_write (fd, output, len) < len)
    {
      if (output != buf)
        free (output);
      return -1;
    }

  if (output != buf)
    free (output);

  if (len > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }

  return len;
}
