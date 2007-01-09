/* Substring test for UTF-8/UTF-16/UTF-32 strings.
   Copyright (C) 2002, 2006 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

bool
FUNC (const UNIT *str, const UNIT *prefix)
{
  for (;;)
    {
      UNIT uc1 = *str++;
      UNIT uc2 = *prefix++;
      if (uc2 == 0)
	return true;
      if (uc1 != uc2)
	return false;
    }
}
