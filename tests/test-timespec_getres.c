/* Test timespec_getres.
   Copyright 2021-2022 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include <time.h>

#include "signature.h"
SIGNATURE_CHECK (timespec_getres, int, (struct timespec *ts, int base));

#include "macros.h"

int
main (void)
{
  struct timespec ts;
  ASSERT (timespec_getres (&ts, TIME_UTC) == TIME_UTC);

  /* No clock resolution should be worse than 1 s or better than 1 ns.  */
  ASSERT (ts.tv_sec == 0
          ? 0 < ts.tv_nsec && ts.tv_nsec < 1000000000
          : ts.tv_sec == 1 && ts.tv_nsec == 0);

  return 0;
}
