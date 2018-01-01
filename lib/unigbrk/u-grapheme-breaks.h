/* Grapheme cluster break function.
   Copyright (C) 2010-2018 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

void
FUNC (const UNIT *s, size_t n, char *p)
{
  if (n > 0)
    {
      const UNIT *s_end = s + n;

      /* Grapheme Cluster break property of the last character.
         -1 at the very beginning of the string.  */
      int last_char_prop = -1;

      /* Grapheme Cluster break property of the last complex character.
         -1 at the very beginning of the string.  */
      int last_compchar_prop = -1;

      size_t ri_count = 0;

      /* Don't break inside multibyte characters.  */
      memset (p, 0, n);

      while (s < s_end)
        {
          ucs4_t uc;
          int count = U_MBTOUC (&uc, s, s_end - s);
          int prop = uc_graphemeclusterbreak_property (uc);

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
              /* No break within emoji modifier sequences (GB10).  */
              else if ((last_compchar_prop == GBP_EB
                        || last_compchar_prop == GBP_EBG)
                       && prop == GBP_EM)
                /* *p = 0 */;
              /* No break within emoji zwj sequences (GB11).  */
              else if (last_char_prop == GBP_ZWJ
                       && (prop == GBP_GAZ
                           || prop == GBP_EBG))
                /* *p = 0 */;
              /* No break between RI if there is an odd number of RI
                 characters before (GB12, GB13).  */
              else if (prop == GBP_RI)
                {
                  if (ri_count % 2 == 0)
                    *p = 1;
                  /* else *p = 0; */
                }
              /* Break everywhere (GBP999).  */
              else
                *p = 1;
            }

          last_char_prop = prop;

          if (!(prop == GBP_EXTEND
                && (last_compchar_prop == GBP_EB
                    || last_compchar_prop == GBP_EBG)))
            last_compchar_prop = prop;

          if (prop == GBP_RI)
            ri_count++;
          else
            ri_count = 0;

          s += count;
          p += count;
        }
    }
}
