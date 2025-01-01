/* Formatted output to a file descriptor.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

#include "intprops.h"

int
vdprintf (int fd, const char *format, va_list args)
{
  off64_t ret = vdzprintf (fd, format, args);

  if (TYPE_MAXIMUM (off64_t) > INT_MAX && ret > INT_MAX)
    {
      errno = EOVERFLOW;
      return -1;
    }

  return ret;
}
