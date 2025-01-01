/* Tokenizing a string.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.
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

#if GNULIB_MCEL_PREFER
# include "mcel.h"
#else
# include "mbuiterf.h"
#endif

char *
mbssep (char **stringp, const char *delim)
{
  if (MB_CUR_MAX > 1)
    {
      char *start = *stringp;

      if (start == NULL)
        return NULL;

      /* No need to optimize the cases of 0 or 1 delimiters specially,
         since mbspbrk already optimizes them.  */

      char *ptr = mbspbrk (start, delim);

      if (ptr == NULL)
        *stringp = NULL;
      else
        {
#if GNULIB_MCEL_PREFER
          *stringp = ptr + mcel_scanz (ptr).len;
#else
          mbuif_state_t state;
          mbuif_init (state);
          if (!mbuif_avail (state, ptr))
            abort ();
          mbchar_t cur = mbuif_next (state, ptr);
          *stringp = ptr + mb_len (cur);
#endif
          *ptr = '\0';
        }
      return start;
    }
  else
    return strsep (stringp, delim);
}
