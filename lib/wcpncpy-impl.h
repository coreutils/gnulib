/* Copy a size-bounded wide string.
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
wcpncpy (wchar_t *dest, const wchar_t *src, size_t n)
{
  for (; n > 0 && (*dest = *src) != (wchar_t)'\0'; src++, dest++, n--)
    ;

  /* This behavior is rarely useful, but it is here for consistency with
     wcsncpy.  */
  for (; n > 0; n--)
    *dest++ = (wchar_t)'\0';

  return dest-1;
}
