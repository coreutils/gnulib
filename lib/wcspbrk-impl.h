/* Search a wide string for any of a set of wide characters.
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
wcspbrk (const wchar_t *wcs, const wchar_t *accept)
{
  /* Optimize two cases.  */
  if (accept[0] == (wchar_t)'\0')
    return NULL;

  if (accept[1] == (wchar_t)'\0')
    {
      wchar_t wc = accept[0];
      const wchar_t *ptr = wcs;
      for (; *ptr != (wchar_t)'\0'; ptr++)
        {
          if (*ptr == wc)
            return (wchar_t *) ptr;
        }
      return NULL;
    }

  /* General case.  */
  {
    const wchar_t *ptr = wcs;
    for (; *ptr != (wchar_t)'\0'; ptr++)
      {
        if (wcschr (accept, *ptr))
          return (wchar_t *) ptr;
      }
    return NULL;
  }
}
