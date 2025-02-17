/*
 * Copyright (C) 2018-2025 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "crc.h"

#include <stdio.h>
#include <stdlib.h>

#include "bench.h"
#include "macros.h"

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s REPETITIONS\n", argv[0]);
      exit (1);
    }

  int repeat = atoi (argv[1]);

  struct timings_state ts;
  timing_start (&ts);

  int count;
  for (count = 0; count < repeat; count++)
    {
      uint32_t volatile digest = crc32 (randomb, sizeof randomb);
      ASSERT (digest == 0x5299f9d2U);
    }

  timing_end (&ts);
  timing_output (&ts);

  return 0;
}
