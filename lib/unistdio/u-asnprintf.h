/* Formatted output to strings.
   Copyright (C) 1999, 2002, 2005-2007, 2009-2022 Free Software Foundation,
   Inc.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

DCHAR_T *
ASNPRINTF (DCHAR_T *resultbuf, size_t *lengthp, const FCHAR_T *format, ...)
{
  va_list args;
  DCHAR_T *result;

  va_start (args, format);
  result = VASNPRINTF (resultbuf, lengthp, format, args);
  va_end (args);
  return result;
}
