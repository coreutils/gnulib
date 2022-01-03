/* Tokenizing a string.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.
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

#include <stdlib.h>

#include "mbuiter.h"

char *
mbssep (char **stringp, const char *delim)
{
  if (MB_CUR_MAX > 1)
    {
      char *start = *stringp;
      char *ptr;

      if (start == NULL)
        return NULL;

      /* No need to optimize the cases of 0 or 1 delimiters specially,
         since mbspbrk already optimizes them.  */

      ptr = mbspbrk (start, delim);

      if (ptr == NULL)
        {
          *stringp = NULL;
          return start;
        }
      else
        {
          mbui_iterator_t iter;

          mbui_init (iter, ptr);
          if (!mbui_avail (iter))
            abort ();
          mbui_advance (iter);
          *ptr = '\0';
          *stringp = (char *) mbui_cur_ptr (iter);
          return start;
        }
    }
  else
    return strsep (stringp, delim);
}
