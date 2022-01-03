/* Searching a string for a character among a given set of characters.
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
   in the character string ACCEPT.  Return the pointer to it, or NULL if none
   exists.  */
char *
mbspbrk (const char *string, const char *accept)
{
  /* Optimize two cases.  */
  if (accept[0] == '\0')
    return NULL;
  if (accept[1] == '\0')
    return mbschr (string, accept[0]);
  /* General case.  */
  if (MB_CUR_MAX > 1)
    {
      mbui_iterator_t iter;

      for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
        {
          if (mb_len (mbui_cur (iter)) == 1)
            {
              if (mbschr (accept, * mbui_cur_ptr (iter)))
                return (char *) mbui_cur_ptr (iter);
            }
          else
            {
              mbui_iterator_t aiter;

              for (mbui_init (aiter, accept);
                   mbui_avail (aiter);
                   mbui_advance (aiter))
                if (mb_equal (mbui_cur (aiter), mbui_cur (iter)))
                  return (char *) mbui_cur_ptr (iter);
            }
        }
      return NULL;
    }
  else
    return strpbrk (string, accept);
}
