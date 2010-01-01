/* Substring test for UTF-8/UTF-16/UTF-32 strings.
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
FUNC (const UNIT *haystack, const UNIT *needle)
{
  UNIT first = needle[0];

  /* Is needle empty?  */
  if (first == 0)
    return (UNIT *) haystack;

  /* Is needle nearly empty?  */
  if (needle[1] == 0)
    return U_STRCHR (haystack, first);

  /* Search for needle's first unit.  */
  for (; *haystack != 0; haystack++)
    if (*haystack == first)
      {
        /* Compare with needle's remaining units.  */
        const UNIT *hptr = haystack + 1;
        const UNIT *nptr = needle + 1;
        for (;;)
          {
            if (*hptr != *nptr)
              break;
            hptr++; nptr++;
            if (*nptr == 0)
              return (UNIT *) haystack;
          }
      }

  return NULL;
}
