/* Test of <utime.h> substitute.
   Copyright (C) 2017-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2017.  */

#include <config.h>

#include <utime.h>

/* Check that 'struct utimbuf' is defined.  */
struct utimbuf b;

int
main (void)
{
  b.actime = b.modtime = 1493490248; /* 2017-04-29 18:24:08 GMT */

  return 0;
}
