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

#include <stdbool.h>
#include <string.h>

#include "macros.h"

static bool
category_equals (uc_general_category_t category1,
                 uc_general_category_t category2)
{
  return (category1.bitmask == category2.bitmask
          && category1.generic == category2.generic
          && (category1.generic
              ? category1.lookup.lookup_fn == category2.lookup.lookup_fn
              : category1.lookup.table == category2.lookup.table));
}

int
main ()
{
  ASSERT (category_equals (uc_general_category_byname ("L"), UC_CATEGORY_L));
  ASSERT (category_equals (uc_general_category_byname ("Lu"), UC_CATEGORY_Lu));
  ASSERT (category_equals (uc_general_category_byname ("Ll"), UC_CATEGORY_Ll));
  ASSERT (category_equals (uc_general_category_byname ("Lt"), UC_CATEGORY_Lt));
  ASSERT (category_equals (uc_general_category_byname ("Lm"), UC_CATEGORY_Lm));
  ASSERT (category_equals (uc_general_category_byname ("Lo"), UC_CATEGORY_Lo));
  ASSERT (category_equals (uc_general_category_byname ("M"), UC_CATEGORY_M));
  ASSERT (category_equals (uc_general_category_byname ("Mn"), UC_CATEGORY_Mn));
  ASSERT (category_equals (uc_general_category_byname ("Mc"), UC_CATEGORY_Mc));
  ASSERT (category_equals (uc_general_category_byname ("Me"), UC_CATEGORY_Me));
  ASSERT (category_equals (uc_general_category_byname ("N"), UC_CATEGORY_N));
  ASSERT (category_equals (uc_general_category_byname ("Nd"), UC_CATEGORY_Nd));
  ASSERT (category_equals (uc_general_category_byname ("Nl"), UC_CATEGORY_Nl));
  ASSERT (category_equals (uc_general_category_byname ("No"), UC_CATEGORY_No));
  ASSERT (category_equals (uc_general_category_byname ("P"), UC_CATEGORY_P));
  ASSERT (category_equals (uc_general_category_byname ("Pc"), UC_CATEGORY_Pc));
  ASSERT (category_equals (uc_general_category_byname ("Pd"), UC_CATEGORY_Pd));
  ASSERT (category_equals (uc_general_category_byname ("Ps"), UC_CATEGORY_Ps));
  ASSERT (category_equals (uc_general_category_byname ("Pe"), UC_CATEGORY_Pe));
  ASSERT (category_equals (uc_general_category_byname ("Pi"), UC_CATEGORY_Pi));
  ASSERT (category_equals (uc_general_category_byname ("Pf"), UC_CATEGORY_Pf));
  ASSERT (category_equals (uc_general_category_byname ("Po"), UC_CATEGORY_Po));
  ASSERT (category_equals (uc_general_category_byname ("S"), UC_CATEGORY_S));
  ASSERT (category_equals (uc_general_category_byname ("Sm"), UC_CATEGORY_Sm));
  ASSERT (category_equals (uc_general_category_byname ("Sc"), UC_CATEGORY_Sc));
  ASSERT (category_equals (uc_general_category_byname ("Sk"), UC_CATEGORY_Sk));
  ASSERT (category_equals (uc_general_category_byname ("So"), UC_CATEGORY_So));
  ASSERT (category_equals (uc_general_category_byname ("Z"), UC_CATEGORY_Z));
  ASSERT (category_equals (uc_general_category_byname ("Zs"), UC_CATEGORY_Zs));
  ASSERT (category_equals (uc_general_category_byname ("Zl"), UC_CATEGORY_Zl));
  ASSERT (category_equals (uc_general_category_byname ("Zp"), UC_CATEGORY_Zp));
  ASSERT (category_equals (uc_general_category_byname ("C"), UC_CATEGORY_C));
  ASSERT (category_equals (uc_general_category_byname ("Cc"), UC_CATEGORY_Cc));
  ASSERT (category_equals (uc_general_category_byname ("Cf"), UC_CATEGORY_Cf));
  ASSERT (category_equals (uc_general_category_byname ("Cs"), UC_CATEGORY_Cs));
  ASSERT (category_equals (uc_general_category_byname ("Co"), UC_CATEGORY_Co));
  ASSERT (category_equals (uc_general_category_byname ("Cn"), UC_CATEGORY_Cn));

  uc_general_category_byname ("Nl");

  {
    uc_general_category_t ct = uc_general_category_byname ("Nd");
    unsigned int c;

    for (c = 0x30; c <= 0x39; c++)
      ASSERT (uc_is_general_category (c, ct));
    for (c = 0x40; c < 0x80; c++)
      ASSERT (!uc_is_general_category (c, ct));
  }

  {
    uc_general_category_t ct = uc_general_category_byname ("Foo");
    unsigned int c;

    for (c = 0; c < 0x110000; c++)
      ASSERT (!uc_is_general_category (c, ct));
  }

  return 0;
}
