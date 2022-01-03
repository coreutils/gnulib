/* Canonical combining classes of Unicode characters.
   Copyright (C) 2002, 2006-2007, 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

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

#include <config.h>

/* Specification.  */
#include "unictype.h"

#include <stdlib.h>

static const signed char u_combining_class_index_part1[10] =
{
   0, /* Not Reordered */
   1, /* Overlay */
  -1,
  -1,
  -1,
  -1,
  -1,
   2, /* Nukta */
   3, /* Kana Voicing */
   4  /* Virama */
};
static const signed char u_combining_class_index_part2[241 - 200] =
{
   5, /* Attached Below Left */
  -1,
   6, /* Attached Below */
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
  -1,
   7, /* Attached Above */
  -1,
   8, /* Attached Above Right */
  -1,
   9, /* Below Left */
  -1,
  10, /* Below */
  -1,
  11, /* Below Right */
  -1,
  12, /* Left */
  -1,
  13, /* Right */
  -1,
  14, /* Above Left */
  -1,
  15, /* Above */
  -1,
  16, /* Above Right */
  17, /* Double Below */
  18, /* Double Above */
  -1,
  -1,
  -1,
  -1,
  -1,
  19  /* Iota Subscript */
};

static const char u_combining_class_name[20][5] =
{
  "NR",   /* Not Reordered */
  "OV",   /* Overlay */
  "NK",   /* Nukta */
  "KV",   /* Kana Voicing */
  "VR",   /* Virama */
  "ATBL", /* Attached Below Left */
  "ATB",  /* Attached Below */
  "ATA",  /* Attached Above */
  "ATAR", /* Attached Above Right */
  "BL",   /* Below Left */
  "B",    /* Below */
  "BR",   /* Below Right */
  "L",    /* Left */
  "R",    /* Right */
  "AL",   /* Above Left */
  "A",    /* Above */
  "AR",   /* Above Right */
  "DB",   /* Double Below */
  "DA",   /* Double Above */
  "IS"    /* Iota Subscript */
};

const char *
uc_combining_class_name (int ccc)
{
  if (ccc >= 0)
    {
      int index;

      if (ccc < 10)
        index = u_combining_class_index_part1[ccc];
      else if (ccc >= 200 && ccc < 241)
        index = u_combining_class_index_part2[ccc - 200];
      else
        return NULL;

      if (index >= 0)
        {
          if (index < sizeof (u_combining_class_name) / sizeof (u_combining_class_name[0]))
            return u_combining_class_name[index];
          else
            abort ();
        }
    }
  return NULL;
}
