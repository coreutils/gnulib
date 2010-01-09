/* Test the Unicode character type functions.
   Copyright (C) 2007-2010 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include "unictype.h"

#include <string.h>

#include "macros.h"

int
main ()
{
  ASSERT (uc_bidi_category_byname ("L") == UC_BIDI_L);
  ASSERT (uc_bidi_category_byname ("LRE") == UC_BIDI_LRE);
  ASSERT (uc_bidi_category_byname ("LRO") == UC_BIDI_LRO);
  ASSERT (uc_bidi_category_byname ("R") == UC_BIDI_R);
  ASSERT (uc_bidi_category_byname ("AL") == UC_BIDI_AL);
  ASSERT (uc_bidi_category_byname ("RLE") == UC_BIDI_RLE);
  ASSERT (uc_bidi_category_byname ("RLO") == UC_BIDI_RLO);
  ASSERT (uc_bidi_category_byname ("PDF") == UC_BIDI_PDF);
  ASSERT (uc_bidi_category_byname ("EN") == UC_BIDI_EN);
  ASSERT (uc_bidi_category_byname ("ES") == UC_BIDI_ES);
  ASSERT (uc_bidi_category_byname ("ET") == UC_BIDI_ET);
  ASSERT (uc_bidi_category_byname ("AN") == UC_BIDI_AN);
  ASSERT (uc_bidi_category_byname ("CS") == UC_BIDI_CS);
  ASSERT (uc_bidi_category_byname ("NSM") == UC_BIDI_NSM);
  ASSERT (uc_bidi_category_byname ("BN") == UC_BIDI_BN);
  ASSERT (uc_bidi_category_byname ("B") == UC_BIDI_B);
  ASSERT (uc_bidi_category_byname ("S") == UC_BIDI_S);
  ASSERT (uc_bidi_category_byname ("WS") == UC_BIDI_WS);
  ASSERT (uc_bidi_category_byname ("ON") == UC_BIDI_ON);
  ASSERT (uc_bidi_category_byname ("X") < 0);
  ASSERT (uc_bidi_category_byname ("") < 0);

  return 0;
}
