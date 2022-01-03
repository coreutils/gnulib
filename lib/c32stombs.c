/* Convert 32-bit wide string to string.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2020.

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

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <string.h>
#include <wchar.h>

size_t
c32stombs (char *dest, const char32_t *src, size_t len)
{
  mbstate_t state;

  memset (&state, '\0', sizeof (mbstate_t));
  return c32srtombs (dest, &src, len, &state);
}
