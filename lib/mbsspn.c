/* Searching a string for a character outside a given set of characters.
   Copyright (C) 1999, 2002, 2006-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

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

#include <config.h>

/* Specification.  */
#include <string.h>

#include "mbuiter.h"

/* Find the first occurrence in the character string STRING of any character
   not in the character string REJECT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.  */
size_t
mbsspn (const char *string, const char *reject)
{
  /* Optimize two cases.  */
  if (reject[0] == '\0')
    return 0;
  if (reject[1] == '\0')
    {
      unsigned char uc = (unsigned char) reject[0];

      if (MB_CUR_MAX > 1)
        {
          mbui_iterator_t iter;

          for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
            if (!(mb_len (mbui_cur (iter)) == 1
                  && (unsigned char) * mbui_cur_ptr (iter) == uc))
              break;
          return mbui_cur_ptr (iter) - string;
        }
      else
        {
          const char *ptr;

          for (ptr = string; *ptr != '\0'; ptr++)
            if ((unsigned char) *ptr != uc)
              break;
          return ptr - string;
        }
    }
  /* General case.  */
  if (MB_CUR_MAX > 1)
    {
      mbui_iterator_t iter;

      for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
        {
          if (mb_len (mbui_cur (iter)) == 1)
            {
              if (mbschr (reject, * mbui_cur_ptr (iter)) == NULL)
                goto found;
            }
          else
            {
              mbui_iterator_t aiter;

              for (mbui_init (aiter, reject);; mbui_advance (aiter))
                {
                  if (!mbui_avail (aiter))
                    goto found;
                  if (mb_equal (mbui_cur (aiter), mbui_cur (iter)))
                    break;
                }
            }
        }
     found:
      return mbui_cur_ptr (iter) - string;
    }
  else
    return strspn (string, reject);
}
