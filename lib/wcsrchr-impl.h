/* Search wide string for a wide character.
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
wcsrchr (const wchar_t *wcs, wchar_t wc)
{
  /* Calling wcslen and then searching from the other end would cause more
     memory accesses.  Avoid that, at the cost of a few more comparisons.  */
  wchar_t *result = NULL;

  for (;; wcs++)
    {
      if (*wcs == wc)
        result = (wchar_t *) wcs;
      if (*wcs == (wchar_t)'\0')
        break;
    }
  return result;
}
