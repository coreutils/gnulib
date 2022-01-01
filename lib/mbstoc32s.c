/* Convert string to 32-bit wide string.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2020.

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

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <string.h>
#include <wchar.h>

size_t
mbstoc32s (char32_t *dest, const char *src, size_t len)
{
  mbstate_t state;

  memset (&state, '\0', sizeof (mbstate_t));
  return mbsrtoc32s (dest, &src, len, &state);
}
