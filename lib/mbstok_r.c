/* Tokenizing a string.
   Copyright (C) 1999, 2002, 2006-2007 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#if HAVE_MBRTOWC
# include "mbuiter.h"
#endif

char *
mbstok_r (char *string, const char *delim, char **save_ptr)
{
#if HAVE_MBRTOWC
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
#endif
    return strtok_r (string, delim, save_ptr);
}
