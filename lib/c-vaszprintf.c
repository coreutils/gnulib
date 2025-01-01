/* Formatted output to strings in C locale.
   Copyright (C) 1999-2025 Free Software Foundation, Inc.

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
#include "c-vasprintf.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "c-vasnprintf.h"

ptrdiff_t
c_vaszprintf (char **resultp, const char *format, va_list args)
{
  size_t length;
  char *result = c_vasnprintf (NULL, &length, format, args);
  if (result == NULL)
    return -1;

  if (length > PTRDIFF_MAX)
    {
      free (result);
      errno = ENOMEM;
      return -1;
    }

  *resultp = result;
  /* Return the number of resulting bytes, excluding the trailing NUL.  */
  return length;
}
