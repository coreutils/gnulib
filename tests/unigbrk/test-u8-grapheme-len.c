/* Grapheme cluster length test.
   Copyright (C) 2010, 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Ben Pfaff <blp@cs.stanford.edu>, 2010. */

#include <config.h>

/* Specification. */
#include <unigbrk.h>

#include "macros.h"

int
main (void)
{
  /* Empty string. */
  ASSERT (u8_grapheme_len ("", 0) == 0);

  /* Standalone 1-unit graphemes.  */
  ASSERT (u8_grapheme_len ("a", 1) == 1);
  ASSERT (u8_grapheme_len ("ab", 2) == 1);
  ASSERT (u8_grapheme_len ("abc", 3) == 1);

  /* Multi-unit, single code point graphemes. */
#define HIRAGANA_A "\343\201\202" /* あ: Hiragana letter 'a'. */
  ASSERT (u8_grapheme_len (HIRAGANA_A, 3) == 3);
  ASSERT (u8_grapheme_len (HIRAGANA_A"x", 4) == 3);
  ASSERT (u8_grapheme_len (HIRAGANA_A HIRAGANA_A, 6) == 3);

  /* Combining accents. */
#define GRAVE "\314\200"        /* Combining grave accent. */
#define ACUTE "\314\201"        /* Combining acute accent. */
  ASSERT (u8_grapheme_len ("e"ACUTE, 3) == 3);
  ASSERT (u8_grapheme_len ("e"ACUTE GRAVE, 5) == 5);
  ASSERT (u8_grapheme_len ("e"ACUTE"x", 4) == 3);
  ASSERT (u8_grapheme_len ("e"ACUTE "e"ACUTE, 6) == 3);

  return 0;
}
