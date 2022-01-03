/* Formatted output to strings in C locale.
   Copyright (C) 1999, 2002, 2006-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "c-vasprintf.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "c-vasnprintf.h"

int
c_vasprintf (char **resultp, const char *format, va_list args)
{
  size_t length;
  char *result = c_vasnprintf (NULL, &length, format, args);
  if (result == NULL)
    return -1;

  if (length > INT_MAX)
    {
      free (result);
      errno = EOVERFLOW;
      return -1;
    }

  *resultp = result;
  /* Return the number of resulting bytes, excluding the trailing NUL.  */
  return length;
}
