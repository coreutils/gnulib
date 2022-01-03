/* Locate a substring in a wide string.
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
wcsstr (const wchar_t *haystack, const wchar_t *needle)
{
  wchar_t n = needle[0];

  /* Is needle empty?  */
  if (n == (wchar_t)'\0')
    return (wchar_t *) haystack;

  /* Is needle nearly empty?  */
  if (needle[1] == (wchar_t)'\0')
    return wcschr (haystack, n);

  /* Search for needle's first character.  */
  for (; *haystack != (wchar_t)'\0'; haystack++)
    {
      if (*haystack == n)
        {
          /* Compare with needle's remaining characters.  */
          const wchar_t *hptr = haystack + 1;
          const wchar_t *nptr = needle + 1;
          for (;;)
            {
              if (*hptr != *nptr)
                break;
              hptr++; nptr++;
              if (*nptr == (wchar_t)'\0')
                return (wchar_t *) haystack;
            }
        }
    }

  return NULL;
}
