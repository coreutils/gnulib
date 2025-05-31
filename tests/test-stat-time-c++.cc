/* Test of <stat-time.h> in C++ mode.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#include "stat-time.h"

struct stat statinfo;

int
main (void)
{
  struct timespec a, c, m, b;
  a = get_stat_atime (&statinfo);
  c = get_stat_ctime (&statinfo);
  m = get_stat_mtime (&statinfo);
  b = get_stat_birthtime (&statinfo);
  (void) (a.tv_sec + c.tv_sec + m.tv_sec + b.tv_sec);
}
