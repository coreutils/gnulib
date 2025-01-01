/* Compare two wide strings.
   Copyright (C) 1999, 2011-2025 Free Software Foundation, Inc.
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

int
wcsncmp (const wchar_t *s1, const wchar_t *s2, size_t n)
{
  for (; n > 0;)
    {
      wchar_t wc1 = *s1++;
      wchar_t wc2 = *s2++;
      if (wc1 != (wchar_t)'\0' && wc1 == wc2)
        {
          n--;
          continue;
        }
      /* ISO C requires wcsncmp to work with all wchar_t values.
         We cannot assume that wc1 and wc2 are in the range 0..INT_MAX.  */
      return _GL_CMP (wc1, wc2);
             /* > 0 if wc1 > wc2, < 0 if wc1 < wc2,
                = 0 if wc1 and wc2 are both '\0'.  */
    }
  return 0;
}
