/* String descriptors, with out-of-memory checking.
   Copyright (C) 2023-2024 Free Software Foundation, Inc.

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

#define GL_XSTRING_DESC_INLINE _GL_EXTERN_INLINE
#include "xstring-desc.h"

#include "ialloc.h"

string_desc_t
xstring_desc_concat (idx_t n, string_desc_t string1, ...)
{
  if (n <= 0)
    /* Invalid argument.  */
    abort ();

  idx_t total = 0;
  total += string1._nbytes;
  if (n > 1)
    {
      va_list other_strings;
      idx_t i;

      va_start (other_strings, string1);
      for (i = n - 1; i > 0; i--)
        {
          string_desc_t arg = va_arg (other_strings, string_desc_t);
          total += arg._nbytes;
        }
      va_end (other_strings);
    }

  char *combined = (char *) imalloc (total);
  if (combined == NULL)
    xalloc_die ();
  idx_t pos = 0;
  memcpy (combined, string1._data, string1._nbytes);
  pos += string1._nbytes;
  if (n > 1)
    {
      va_list other_strings;
      idx_t i;

      va_start (other_strings, string1);
      for (i = n - 1; i > 0; i--)
        {
          string_desc_t arg = va_arg (other_strings, string_desc_t);
          if (arg._nbytes > 0)
            memcpy (combined + pos, arg._data, arg._nbytes);
          pos += arg._nbytes;
        }
      va_end (other_strings);
    }

  string_desc_t result;
  result._nbytes = total;
  result._data = combined;

  return result;
}
