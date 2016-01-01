/* Word breaks in UTF-8/UTF-16/UTF-32 strings.  -*- coding: utf-8 -*-
   Copyright (C) 2009-2016 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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
              /* No break between CR and LF.  */
              if (last_char_prop == WBP_CR && prop == WBP_LF)
                /* *p = 0 */;
              /* Break before and after newlines.  */
              else if ((last_char_prop == WBP_CR
                        || last_char_prop == WBP_LF
                        || last_char_prop == WBP_NEWLINE)
                       || (prop == WBP_CR
                           || prop == WBP_LF
                           || prop == WBP_NEWLINE))
                *p = 1;
              /* Ignore Format and Extend characters.  */
              else if (!(prop == WBP_EXTEND || prop == WBP_FORMAT))
                {
                  /* No break in these situations (see UAX #29):

                      secondlast          last             current

    (ALetter | HL)   (MidLetter | MidNumLet | SQ) × (ALetter | HL)      (WB7)
    (ALetter | HL) × (MidLetter | MidNumLet | SQ)   (ALetter | HL)      (WB6)
                  Numeric   (MidNum | MidNumLet | SQ)    × Numeric      (WB11)
                  Numeric × (MidNum | MidNumLet | SQ)      Numeric      (WB12)
                                                        HL × DQ HL      (WB7b)
                                                        HL DQ × HL      (WB7c)
                                   (ALetter | HL) × (ALetter | HL)      (WB5)
                                          (ALetter | HL) × Numeric      (WB9)
                                          Numeric × (ALetter | HL)      (WB10)
                                                 Numeric × Numeric      (WB8)
                                                      HL × SQ           (WB7a)
                                                Katakana × Katakana     (WB13)
                     (ALetter | HL | Numeric | Katakana) × ExtendNumLet (WB13a)
                                            ExtendNumLet × ExtendNumLet (WB13a)
                    ExtendNumLet × (ALetter | HL | Numeric | Katakana)  (WB13b)
                               Regional_Indicator × Regional_Indicator  (WB13c)
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
                  /* Break after Format and Extend characters.  */
                  else if (last_compchar_prop == WBP_EXTEND
                           || last_compchar_prop == WBP_FORMAT)
                    *p = 1;
                  else
                    {
                      /* Normalize property value to table index,
                         skipping 5 properties: WBP_EXTEND,
                         WBP_FORMAT, WBP_NEWLINE, WBP_CR, and
                         WBP_LF.  */
                      int last_compchar_prop_index = last_compchar_prop;
                      int prop_index = prop;

                      if (last_compchar_prop_index >= WBP_EXTEND)
                        last_compchar_prop_index -= 5;

                      if (prop_index >= WBP_EXTEND)
                        prop_index -= 5;

                      /* Perform a single table lookup.  */
                      if (uniwbrk_table[last_compchar_prop_index][prop_index])
                        *p = 1;
                      /* else *p = 0; */
                    }
                }
            }

          last_char_prop = prop;
          /* Ignore Format and Extend characters, except at the start
             of the line.  */
          if (last_compchar_prop < 0
              || last_compchar_prop == WBP_CR
              || last_compchar_prop == WBP_LF
              || last_compchar_prop == WBP_NEWLINE
              || !(prop == WBP_EXTEND || prop == WBP_FORMAT))
            {
              secondlast_compchar_prop = last_compchar_prop;
              last_compchar_prop = prop;
              last_compchar_ptr = p;
            }

          s += count;
          p += count;
        }
    }
}
