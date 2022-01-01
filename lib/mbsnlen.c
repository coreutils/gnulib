/* Counting the multibyte characters in a string.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

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
#include <string.h>

#include <stdlib.h>

#include "mbiter.h"

/* Return the number of multibyte characters in the character string starting
   at STRING and ending at STRING + LEN.  */
size_t
mbsnlen (const char *string, size_t len)
{
  if (MB_CUR_MAX > 1)
    {
      size_t count;
      mbi_iterator_t iter;

      count = 0;
      for (mbi_init (iter, string, len); mbi_avail (iter); mbi_advance (iter))
        count++;

      return count;
    }
  else
    return len;
}
