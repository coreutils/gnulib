/* Grapheme cluster break function.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

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
#include "unigbrk.h"

/* Evaluates to true if there is an extended grapheme cluster break between
   code points with GBP_* values A and B, false if there is not.  The comments
   are the grapheme cluster boundary rules from in UAX #29. */
#define UC_IS_GRAPHEME_BREAK(A, B)                                      \
  (/* GB1 and GB2 are covered--just use a GBP_CONTROL character, such   \
      as 0, for sot and eot. */                                         \
                                                                        \
   /* GB3 */                                                            \
   (A) == GBP_CR && (B) == GBP_LF ? false :                             \
                                                                        \
   /* GB4 */                                                            \
   (A) == GBP_CONTROL || (A) == GBP_CR || (A) == GBP_LF ? true :        \
                                                                        \
   /* GB5 */                                                            \
   (B) == GBP_CONTROL || (B) == GBP_CR || (B) == GBP_LF ? true :        \
                                                                        \
   /* GB6 */                                                            \
   (A) == GBP_L && ((B) == GBP_L || (B) == GBP_V                        \
                    || (B) == GBP_LV || (B) == GBP_LVT) ? false :       \
                                                                        \
   /* GB7 */                                                            \
   ((A) == GBP_LV || (A) == GBP_V)                                      \
   && ((B) == GBP_V || (B) == GBP_T) ? false :                          \
                                                                        \
   /* GB8 */                                                            \
   ((A) == GBP_LVT || (A) == GBP_T) && (B) == GBP_T ? false :           \
                                                                        \
   /* GB9 */                                                            \
   (B) == GBP_EXTEND || (B) == GBP_ZWJ ? false :                        \
                                                                        \
   /* GB9a */                                                           \
   (B) == GBP_SPACINGMARK ? false :                                     \
                                                                        \
   /* GB9b */                                                           \
   (A) == GBP_PREPEND ? false :                                         \
                                                                        \
   /* GB10 -- incomplete */                                             \
   ((A) == GBP_EB || (A) == GBP_EBG) && (B) == GBP_EM ? false :         \
                                                                        \
   /* GB11 */                                                           \
   (A) == GBP_ZWJ && ((B) == GBP_GAZ || (B) == GBP_EBG) ? false         \
                                                                        \
   /* GB999 */                                                          \
   : true)

#define UC_GRAPHEME_BREAKS_FOR(A)                                       \
  (  (UC_IS_GRAPHEME_BREAK(A, GBP_OTHER)       << GBP_OTHER)            \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_CR)          << GBP_CR)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_LF)          << GBP_LF)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_CONTROL)     << GBP_CONTROL)          \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_EXTEND)      << GBP_EXTEND)           \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_PREPEND)     << GBP_PREPEND)          \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_SPACINGMARK) << GBP_SPACINGMARK)      \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_L)           << GBP_L)                \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_V)           << GBP_V)                \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_T)           << GBP_T)                \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_LV)          << GBP_LV)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_LVT)         << GBP_LVT)              \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_RI)          << GBP_RI)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_ZWJ)         << GBP_ZWJ)              \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_EB)          << GBP_EB)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_EM)          << GBP_EM)               \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_GAZ)         << GBP_GAZ)              \
   | (UC_IS_GRAPHEME_BREAK(A, GBP_EBG)         << GBP_EBG))

static const unsigned long int gb_table[18] =
  {
    UC_GRAPHEME_BREAKS_FOR(0),  /* GBP_OTHER */
    UC_GRAPHEME_BREAKS_FOR(1),  /* GBP_CR */
    UC_GRAPHEME_BREAKS_FOR(2),  /* GBP_LF */
    UC_GRAPHEME_BREAKS_FOR(3),  /* GBP_CONTROL */
    UC_GRAPHEME_BREAKS_FOR(4),  /* GBP_EXTEND */
    UC_GRAPHEME_BREAKS_FOR(5),  /* GBP_PREPEND */
    UC_GRAPHEME_BREAKS_FOR(6),  /* GBP_SPACINGMARK */
    UC_GRAPHEME_BREAKS_FOR(7),  /* GBP_L */
    UC_GRAPHEME_BREAKS_FOR(8),  /* GBP_V */
    UC_GRAPHEME_BREAKS_FOR(9),  /* GBP_T */
    UC_GRAPHEME_BREAKS_FOR(10), /* GBP_LV */
    UC_GRAPHEME_BREAKS_FOR(11), /* GBP_LVT */
    UC_GRAPHEME_BREAKS_FOR(12), /* GBP_RI */
    UC_GRAPHEME_BREAKS_FOR(13), /* GBP_ZWJ */
    UC_GRAPHEME_BREAKS_FOR(14), /* GBP_EB */
    UC_GRAPHEME_BREAKS_FOR(15), /* GBP_EM */
    UC_GRAPHEME_BREAKS_FOR(16), /* GBP_GAZ */
    UC_GRAPHEME_BREAKS_FOR(17), /* GBP_EBG */
  };

bool
uc_is_grapheme_break (ucs4_t a, ucs4_t b)
{
  int a_gcp, b_gcp;

  if ((a | b) < 0x300)
    {
      /* GB3 is the only relevant rule for this case. */
      return a != '\r' || b != '\n';
    }

  a_gcp = uc_graphemeclusterbreak_property (a);
  b_gcp = uc_graphemeclusterbreak_property (b);
  return (gb_table[a_gcp] >> b_gcp) & 1;
}
