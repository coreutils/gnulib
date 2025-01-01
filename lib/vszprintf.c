/* Formatted output to strings.
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
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "vasnprintf.h"

ptrdiff_t
vszprintf (char *str, const char *format, va_list args)
{
  char *output;
  size_t len;
  size_t lenbuf;

  /* Set lenbuf = min (SIZE_MAX, - (uintptr_t) str - 1).  */
  lenbuf = SIZE_MAX;
  if (lenbuf >= ~ (uintptr_t) str)
    lenbuf = ~ (uintptr_t) str;

  output = vasnprintf (str, &lenbuf, format, args);
  len = lenbuf;

  if (!output)
    return -1;

  if (output != str)
    {
      /* len is near SIZE_MAX.  */
      free (output);
      errno = ENOMEM;
      return -1;
    }

  if (len > PTRDIFF_MAX)
    {
      errno = ENOMEM;
      return -1;
    }

  return len;
}
