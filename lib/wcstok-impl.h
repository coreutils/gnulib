/* Split a wide string into tokens.
   Copyright (C) 1999, 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 1999.

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

wchar_t *
wcstok (wchar_t *wcs, const wchar_t *delim, wchar_t **ptr)
{
  if (wcs == NULL)
    {
      wcs = *ptr;
      if (wcs == NULL)
        return NULL; /* reminder that end of token sequence has been reached */
    }

  /* Skip leading delimiters.  */
  wcs += wcsspn (wcs, delim);

  /* Found a token?  */
  if (*wcs == (wchar_t)'\0')
    {
      *ptr = NULL;
      return NULL;
    }
  /* Move past the token.  */
  {
    wchar_t *token_end = wcspbrk (wcs, delim);
    if (token_end)
      {
        /* NUL-terminate the token.  */
        *token_end = (wchar_t)'\0';
        *ptr = token_end + 1;
      }
    else
      *ptr = NULL;
  }
  return wcs;
}
