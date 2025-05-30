/* Grapheme cluster break function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff, Daiki Ueno, Bruno Haible.  */

/* This file implements section 3 "Grapheme Cluster Boundaries"
   of Unicode Standard Annex #29 <https://www.unicode.org/reports/tr29/>.  */

void
FUNC (const UNIT *s, size_t n, char *p)
{
  if (n > 0)
    {
      const UNIT *s_end = s + n;

      /* Grapheme Cluster break property of the last character.
         -1 at the very beginning of the string.  */
      int last_char_prop = -1;

      /* True if the last character ends a sequence of Indic_Conjunct_Break
         values:  consonant {extend|linker}*  */
      bool incb_consonant_extended = false;
      /* True if the last character ends a sequence of Indic_Conjunct_Break
         values:  consonant {extend|linker}* linker  */
      bool incb_consonant_extended_linker = false;
      /* True if the last character ends a sequence of Indic_Conjunct_Break
         values:  consonant {extend|linker}* linker {extend|linker}*  */
      bool incb_consonant_extended_linker_extended = false;

      /* True if the last character ends an emoji modifier sequence
         \p{Extended_Pictographic} Extend*.  */
      bool emoji_modifier_sequence = false;
      /* True if the last character was immediately preceded by an
         emoji modifier sequence   \p{Extended_Pictographic} Extend*.  */
      bool emoji_modifier_sequence_before_last_char = false;

      /* Number of consecutive regional indicator (RI) characters seen
         immediately before the current point.  */
      size_t ri_count = 0;

      /* Don't break inside multibyte characters.  */
      memset (p, 0, n);

      do
        {
          /* Invariant: Here s < s_end.  */
          ucs4_t uc;
          int count = U_MBTOUC (&uc, s, s_end - s);
          int prop = uc_graphemeclusterbreak_property (uc);
          int incb = uc_indic_conjunct_break (uc);

          /* Break at the start of the string (GB1).  */
          if (last_char_prop < 0)
            *p = 1;
          else
            {
              /* No break between CR and LF (GB3).  */
              if (last_char_prop == GBP_CR && prop == GBP_LF)
                /* *p = 0 */;
              /* Break before and after newlines (GB4, GB5).  */
              else if ((last_char_prop == GBP_CR
                        || last_char_prop == GBP_LF
                        || last_char_prop == GBP_CONTROL)
                       || (prop == GBP_CR
                           || prop == GBP_LF
                           || prop == GBP_CONTROL))
                *p = 1;
              /* No break between Hangul syllable sequences (GB6, GB7, GB8).  */
              else if ((last_char_prop == GBP_L
                        && (prop == GBP_L
                            || prop == GBP_V
                            || prop == GBP_LV
                            || prop == GBP_LVT))
                       || ((last_char_prop == GBP_LV
                            || last_char_prop == GBP_V)
                           && (prop == GBP_V
                               || prop == GBP_T))
                       || ((last_char_prop == GBP_LVT
                            || last_char_prop == GBP_T)
                           && prop == GBP_T))
                /* *p = 0 */;
              /* No break before extending characters or ZWJ (GB9).  */
              else if (prop == GBP_EXTEND || prop == GBP_ZWJ)
                /* *p = 0 */;
              /* No break before SpacingMarks (GB9a).  */
              else if (prop == GBP_SPACINGMARK)
                /* *p = 0 */;
              /* No break after Prepend characters (GB9b).  */
              else if (last_char_prop == GBP_PREPEND)
                /* *p = 0 */;
              /* No break within certain combinations of Indic_Conjunct_Break
                 values: Between
                   consonant {extend|linker}* linker {extend|linker}*
                 and
                   consonant
                 (GB9c).  */
              else if (incb_consonant_extended_linker_extended
                       && incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT)
                /* *p = 0 */;
              /* No break within emoji modifier sequences or emoji zwj sequences
                 (GB11).  */
              else if (last_char_prop == GBP_ZWJ
                       && emoji_modifier_sequence_before_last_char
                       && uc_is_property_extended_pictographic (uc))
                /* *p = 0 */;
              /* No break between RI if there is an odd number of RI
                 characters before (GB12, GB13).  */
              else if (prop == GBP_RI && (ri_count % 2) != 0)
                /* *p = 0 */;
              /* Break everywhere (GB999).  */
              else
                *p = 1;
            }

          incb_consonant_extended_linker =
            incb_consonant_extended && incb == UC_INDIC_CONJUNCT_BREAK_LINKER;
          incb_consonant_extended_linker_extended =
            (incb_consonant_extended_linker
             || (incb_consonant_extended_linker_extended
                 && incb >= UC_INDIC_CONJUNCT_BREAK_LINKER));
          incb_consonant_extended =
            (incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT
             || (incb_consonant_extended
                 && incb >= UC_INDIC_CONJUNCT_BREAK_LINKER));

          emoji_modifier_sequence_before_last_char = emoji_modifier_sequence;
          emoji_modifier_sequence =
            (emoji_modifier_sequence && prop == GBP_EXTEND)
            || uc_is_property_extended_pictographic (uc);

          last_char_prop = prop;

          if (prop == GBP_RI)
            ri_count++;
          else
            ri_count = 0;

          s += count;
          p += count;
        }
      while (s < s_end);
    }
}
