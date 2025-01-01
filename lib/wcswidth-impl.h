/* Determine number of screen columns needed for a size-bounded wide string.
   Copyright (C) 1999, 2011-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 1999.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

int
FUNC (const UNIT *s, size_t n)
{
  int count = 0;
  for (; n > 0; s++, n--)
    {
      UNIT c = *s;
      if (c == (UNIT)'\0')
        break;
      {
        int width = CHARACTER_WIDTH (c);
        if (width < 0)
          goto found_nonprinting;
        if (width > INT_MAX - count)
          goto overflow;
        count += width;
      }
    }
  return count;

  /* The total width has become > INT_MAX.
     Continue searching for a non-printing wide character.  */
  for (; n > 0; s++, n--)
    {
      UNIT c = *s;
      if (c == (UNIT)'\0')
        break;
      {
        int width = CHARACTER_WIDTH (c);
        if (width < 0)
          goto found_nonprinting;
      }
     overflow: ;
    }
  return INT_MAX;

 found_nonprinting:
  return -1;
}
