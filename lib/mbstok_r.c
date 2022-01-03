/* Tokenizing a string.
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

char *
mbstok_r (char *string, const char *delim, char **save_ptr)
{
  if (MB_CUR_MAX > 1)
    {
      if (string == NULL)
        {
          string = *save_ptr;
          if (string == NULL)
            return NULL; /* reminder that end of token sequence has been
                            reached */
        }

      /* Skip leading delimiters.  */
      string += mbsspn (string, delim);

      /* Found a token?  */
      if (*string == '\0')
        {
          *save_ptr = NULL;
          return NULL;
        }

      /* Move past the token.  */
      {
        char *token_end = mbspbrk (string, delim);

        if (token_end != NULL)
          {
            /* NUL-terminate the token.  */
            *token_end = '\0';
            *save_ptr = token_end + 1;
          }
        else
          *save_ptr = NULL;
      }

      return string;
    }
  else
    return strtok_r (string, delim, save_ptr);
}
