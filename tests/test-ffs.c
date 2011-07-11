/*
 * Copyright (C) 2011 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Eric Blake.  */
#include <config.h>

#include <strings.h>

#include "signature.h"
SIGNATURE_CHECK (ffs, int, (int));

#include <limits.h>

#include "macros.h"

static int
naive (int i)
{
  int j;
  for (j = 0; j < CHAR_BIT * sizeof i; j++)
    if (i & (1 << j))
      return j + 1;
  return 0;
}

int
main (int argc, char *argv[])
{
  int i;

  for (i = -128; i <= 128; i++)
    ASSERT (ffs (i) == naive (i));
  for (i = 0; i < CHAR_BIT * sizeof i; i++)
    {
      ASSERT (ffs (1 << i) == naive (1 << i));
      ASSERT (ffs (1 << i) == i + 1);
    }
  return 0;
}
