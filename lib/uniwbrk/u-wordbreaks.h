/* Word breaks in UTF-8/UTF-16/UTF-32 strings.  -*- coding: utf-8 -*-
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

/* This file implements section 4 "Word Boundaries"
   of Unicode Standard Annex #29 <https://www.unicode.org/reports/tr29/>.  */

void
FUNC (const UNIT *s, size_t n, char *p)
{
  if (n > 0)
    {
      const UNIT *s_end = s + n;

      /* Word break property of the last character.
         -1 at the very beginning of the string.  */
      int last_char_prop = -1;

      /* Format and Extend characters are ignored; this means, the mostly used
         unit is the complex character (= character with subsequent ignored
         characters).
         Word break property of the last complex character.
         -1 at the very beginning of the string.  */
      int last_compchar_prop = -1;
      char *last_compchar_ptr = NULL;

      /* For recognizing rules involving 3 complex characters:
         Word break property of the second-to-last complex character.
         -1 at the very beginning of the string.  */
      int secondlast_compchar_prop = -1;

      /* Number of consecutive regional indicator (RI) characters seen
         immediately before the current point.  */
      size_t ri_count = 0;

      /* Don't break inside multibyte characters.  */
      memset (p, 0, n);

      while (s < s_end)
        {
          ucs4_t uc;
          int count = U_MBTOUC_UNSAFE (&uc, s, s_end - s);
          int prop = uc_wordbreak_property (uc);

          /* No break at the start of the string.  */
          if (last_char_prop >= 0)
            {
              /* No break between CR and LF (WB3).  */
              if (last_char_prop == WBP_CR && prop == WBP_LF)
                /* *p = 0 */;
              /* Break before and after newlines (WB3a, WB3b).  */
              else if ((last_char_prop == WBP_CR
                        || last_char_prop == WBP_LF
                        || last_char_prop == WBP_NEWLINE)
                       || (prop == WBP_CR
                           || prop == WBP_LF
                           || prop == WBP_NEWLINE))
                *p = 1;
              /* No break within emoji zwj sequence (WB3c).  */
              else if (last_char_prop == WBP_ZWJ
                       && uc_is_property_extended_pictographic (uc))
                /* *p = 0 */;
              /* Keep horizontal whitespace together (WB3d).  */
              else if (last_char_prop == WBP_WSS && prop == WBP_WSS)
                /* *p = 0 */;
              /* Ignore Format and Extend characters (WB4).  */
              else if (prop == WBP_EXTEND
                       || prop == WBP_FORMAT
                       || prop == WBP_ZWJ)
                /* *p = 0 */;
              else
                {
                  /* No break in these situations (see UAX #29):

                      secondlast          last             current

    (ALetter | HL)   (MidLetter | MidNumLet | SQ) × (ALetter | HL)      (WB7)
    (ALetter | HL) × (MidLetter | MidNumLet | SQ)   (ALetter | HL)      (WB6)
                  Numeric   (MidNum | MidNumLet | SQ)    × Numeric      (WB11)
                  Numeric × (MidNum | MidNumLet | SQ)      Numeric      (WB12)
                                                        HL × DQ HL      (WB7b)
                                                        HL DQ × HL      (WB7c)
                                                ^ (RI RI)* RI × RI      (WB15)
                                            [^RI] (RI RI)* RI × RI      (WB16)
                   */
                  /* No break across certain punctuation.  Also, disable word
                     breaks that were recognized earlier (due to lookahead of
                     only one complex character).  */
                  if (((prop == WBP_ALETTER
                        || prop == WBP_HL)
                       && (last_compchar_prop == WBP_MIDLETTER
                           || last_compchar_prop == WBP_MIDNUMLET
                           || last_compchar_prop == WBP_SQ)
                       && (secondlast_compchar_prop == WBP_ALETTER
                           || secondlast_compchar_prop == WBP_HL))
                      || (prop == WBP_NUMERIC
                          && (last_compchar_prop == WBP_MIDNUM
                              || last_compchar_prop == WBP_MIDNUMLET
                              || last_compchar_prop == WBP_SQ)
                          && secondlast_compchar_prop == WBP_NUMERIC)
                      || (prop == WBP_HL
                          && last_compchar_prop == WBP_DQ
                          && secondlast_compchar_prop == WBP_HL))
                    {
                      *last_compchar_ptr = 0;
                      /* *p = 0; */
                    }
                  /* Break before RI, if odd number of RI's are
                     preceding (WB15, WB16).  */
                  else if (last_compchar_prop == WBP_RI && prop == WBP_RI)
                    {
                      if (ri_count % 2 == 0)
                        *p = 1;
                      /* else *p = 0 */
                    }
                  /* Break after Format and Extend character.  */
                  else if (last_compchar_prop == WBP_EXTEND
                           || last_compchar_prop == WBP_FORMAT)
                    *p = 1;
                  else
                    {
                      int last_compchar_index =
                        uniwbrk_prop_index[last_compchar_prop];
                      int index = uniwbrk_prop_index[prop];

                      /* Break between unknown pair (WB999).  */
                      if (last_compchar_index < 0 || index < 0)
                        *p = 1;
                      /* Perform a single table lookup.  */
                      else if (uniwbrk_table[last_compchar_index][index])
                        *p = 1;
                      /* else *p = 0; */
                    }
                }
            }

          last_char_prop = prop;

          /* Ignore Format and Extend characters, except at the
             start of the line (WB4).  */
          if (last_compchar_prop < 0
              || last_compchar_prop == WBP_CR
              || last_compchar_prop == WBP_LF
              || last_compchar_prop == WBP_NEWLINE
              || !(prop == WBP_EXTEND || prop == WBP_FORMAT || prop == WBP_ZWJ))
            {
              secondlast_compchar_prop = last_compchar_prop;
              last_compchar_prop = prop;
              last_compchar_ptr = p;

              if (prop == WBP_RI)
                ri_count++;
              else
                ri_count = 0;
            }

          s += count;
          p += count;
        }
    }
}
