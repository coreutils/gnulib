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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

/* This file implements section 3 "Grapheme Cluster Boundaries"
   of Unicode Standard Annex #29 <https://www.unicode.org/reports/tr29/>
   backwards.  */

/* Returns true if the string [s_start, s) ends with a sequence of
   Indic_Conjunct_Break values like:
     consonant {extend|linker}* linker {extend|linker}*
 */
static bool
ends_with_incb_consonant_extended_linker_extended (const UNIT *s,
                                                   const UNIT *s_start)
{
  /* Look for
       consonant {extend|linker}*
     with at least one linker.  */
  bool seen_linker = false;

  while (s > s_start)
    {
      const UNIT *prev_s;
      ucs4_t uc;

      prev_s = U_PREV (&uc, s, s_start);
      if (prev_s == NULL)
        /* Ill-formed UTF-8 encoding. */
        break;

      int incb = uc_indic_conjunct_break (uc);
      if (incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT)
        return seen_linker;
      if (!(incb >= UC_INDIC_CONJUNCT_BREAK_LINKER))
        break;
      seen_linker |= (incb == UC_INDIC_CONJUNCT_BREAK_LINKER);

      s = prev_s;
    }

  return false;
}

/* Returns true if the string [s_start, s) ends with a sequence of
   characters like:
     \p{Extended_Pictographic} Extend*
 */
static bool
ends_with_emoji_modifier_sequence (const UNIT *s, const UNIT *s_start)
{
  while (s > s_start)
    {
      const UNIT *prev_s;
      ucs4_t uc;

      prev_s = U_PREV (&uc, s, s_start);
      if (prev_s == NULL)
        /* Ill-formed UTF-8 encoding. */
        break;

      if (uc_is_property_extended_pictographic (uc))
        return true;

      if (uc_graphemeclusterbreak_property (uc) != GBP_EXTEND)
        break;

      s = prev_s;
    }

  return false;
}

/* Returns the number of consecutive regional indicator (RI) characters
   at the end of the string [s_start, s).  */
static size_t
ends_with_ri_count (const UNIT *s, const UNIT *s_start)
{
  size_t ri_count = 0;

  while (s > s_start)
    {
      const UNIT *prev_s;
      ucs4_t uc;

      prev_s = U_PREV (&uc, s, s_start);
      if (prev_s == NULL)
        /* Ill-formed UTF-8 encoding. */
        break;

      if (uc_graphemeclusterbreak_property (uc) == GBP_RI)
        ri_count++;
      else
        break;

      s = prev_s;
    }

  return ri_count;
}

const UNIT *
FUNC (const UNIT *s, const UNIT *s_start)
{
  if (s == s_start)
    return NULL;

  /* Traverse the string backwards, from s down to s_start.  */

  /* Grapheme Cluster break property of the next character.
     -1 at the very end of the string.  */
  int next_char_prop = -1;

  /* Indic_Conjunct_Break property of the next character.
     -1 at the very end of the string.  */
  int next_char_incb = -1;

  /* Extended_Pictographic property of the next character.
     false at the very end of the string.  */
  bool next_char_epic = false;

  do
    {
      const UNIT *prev_s;
      ucs4_t uc;

      prev_s = U_PREV (&uc, s, s_start);
      if (prev_s == NULL)
        {
          /* Ill-formed UTF-8 encoding. */
          return s_start;
        }

      int prop = uc_graphemeclusterbreak_property (uc);
      int incb = uc_indic_conjunct_break (uc);
      bool epic = uc_is_property_extended_pictographic (uc);

      /* Break at the end of the string (GB2).  */
      if (next_char_prop < 0)
        /* *p = 1 */;
      else
        {
          /* No break between CR and LF (GB3).  */
          if (prop == GBP_CR && next_char_prop == GBP_LF)
            /* *p = 0 */;
          /* Break before and after newlines (GB4, GB5).  */
          else if ((prop == GBP_CR
                    || prop == GBP_LF
                    || prop == GBP_CONTROL)
                   || (next_char_prop == GBP_CR
                       || next_char_prop == GBP_LF
                       || next_char_prop == GBP_CONTROL))
            break /* *p = 1 */;
          /* No break between Hangul syllable sequences (GB6, GB7, GB8).  */
          else if ((prop == GBP_L
                    && (next_char_prop == GBP_L
                        || next_char_prop == GBP_V
                        || next_char_prop == GBP_LV
                        || next_char_prop == GBP_LVT))
                   || ((prop == GBP_LV
                        || prop == GBP_V)
                       && (next_char_prop == GBP_V
                           || next_char_prop == GBP_T))
                   || ((prop == GBP_LVT
                        || prop == GBP_T)
                       && next_char_prop == GBP_T))
            /* *p = 0 */;
          /* No break before extending characters or ZWJ (GB9).  */
          else if (next_char_prop == GBP_EXTEND || next_char_prop == GBP_ZWJ)
            /* *p = 0 */;
          /* No break before SpacingMarks (GB9a).  */
          else if (next_char_prop == GBP_SPACINGMARK)
            /* *p = 0 */;
          /* No break after Prepend characters (GB9b).  */
          else if (prop == GBP_PREPEND)
            /* *p = 0 */;
          /* No break within certain combinations of Indic_Conjunct_Break
             values: Between
               consonant {extend|linker}* linker {extend|linker}*
             and
               consonant
             (GB9c).  */
          else if (next_char_incb == UC_INDIC_CONJUNCT_BREAK_CONSONANT
                   && ends_with_incb_consonant_extended_linker_extended (s, s_start))
            /* *p = 0 */;
          /* No break within emoji modifier sequences or emoji zwj sequences
             (GB11).  */
          else if (next_char_epic
                   && prop == GBP_ZWJ
                   && ends_with_emoji_modifier_sequence (prev_s, s_start))
            /* *p = 0 */;
          /* No break between RI if there is an odd number of RI
             characters before (GB12, GB13).  */
          else if (next_char_prop == GBP_RI
                   && prop == GBP_RI
                   && (ends_with_ri_count (prev_s, s_start) % 2) == 0)
            /* *p = 0 */;
          /* Break everywhere (GB999).  */
          else
            break /* *p = 1 */;
        }

      s = prev_s;
      next_char_prop = prop;
      next_char_incb = incb;
      next_char_epic = epic;
    }
  while (s > s_start);

  return s;
}
