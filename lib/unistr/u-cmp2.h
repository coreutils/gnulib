/* Compare pieces of UTF-8/UTF-16/UTF-32 strings.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

int
FUNC (const UNIT *s1, size_t n1, const UNIT *s2, size_t n2)
{
  int cmp = U_CMP (s1, s2, MIN (n1, n2));

  if (cmp == 0)
    {
      if (n1 < n2)
        cmp = -1;
      else if (n1 > n2)
        cmp = 1;
    }

  return cmp;
}
