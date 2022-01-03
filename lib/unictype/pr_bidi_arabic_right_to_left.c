/* Properties of Unicode characters.
   Copyright (C) 2002, 2006-2007, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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

#if 0

#include "bitmap.h"

/* Define u_property_bidi_arabic_right_to_left table.  */
#include "pr_bidi_arabic_right_to_left.h"

bool
uc_is_property_bidi_arabic_right_to_left (ucs4_t uc)
{
  return bitmap_lookup (&u_property_bidi_arabic_right_to_left, uc);
}

#else

bool
uc_is_property_bidi_arabic_right_to_left (ucs4_t uc)
{
  return (uc_bidi_category (uc) == UC_BIDI_AL);
}

#endif

const uc_property_t UC_PROPERTY_BIDI_ARABIC_RIGHT_TO_LEFT =
  { &uc_is_property_bidi_arabic_right_to_left };
