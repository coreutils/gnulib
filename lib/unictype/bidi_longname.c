/* Bidi classes of Unicode characters.
   Copyright (C) 2002, 2006, 2011-2022 Free Software Foundation, Inc.
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

static const char u_bidi_class_long_name[19][24] =
{
  "Left To Right",
  "Left To Right Embedding",
  "Left To Right Override",
  "Right To Left",
  "Arabic Letter",
  "Right To Left Embedding",
  "Right To Left Override",
  "Pop Directional Format",
  "European Number",
  "European Separator",
  "European Terminator",
  "Arabic Number",
  "Common Separator",
  "Nonspacing Mark",
  "Boundary Neutral",
  "Paragraph Separator",
  "Segment Separator",
  "White Space",
  "Other Neutral"
};

const char *
uc_bidi_class_long_name (int bidi_class)
{
  if (bidi_class >= 0
      && bidi_class < sizeof (u_bidi_class_long_name) / sizeof (u_bidi_class_long_name[0]))
    return u_bidi_class_long_name[bidi_class];
  return NULL;
}
