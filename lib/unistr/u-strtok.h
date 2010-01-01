/* Tokenize UTF-8/UTF-16/UTF-32 string.
   Copyright (C) 1999, 2002, 2006, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

UNIT *
FUNC (UNIT *str, const UNIT *delim, UNIT **ptr)
{
  if (str == NULL)
    {
      str = *ptr;
      if (str == NULL)
        return NULL; /* reminder that end of token sequence has been reached */
    }

  /* Skip leading delimiters.  */
  str += U_STRSPN (str, delim);

  /* Found a token?  */
  if (*str == 0)
    {
      *ptr = NULL;
      return NULL;
    }

  /* Move past the token. */
  {
    UNIT *token_end = U_STRPBRK (str, delim);
    if (token_end)
      {
        /* NUL-terminate the token.  */
        *token_end = 0;
        *ptr = token_end + 1;
      }
    else
      *ptr = NULL;
  }

  return str;
}
