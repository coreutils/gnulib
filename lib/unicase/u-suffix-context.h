/* Case-mapping context of suffix UTF-8/UTF-16/UTF-32 string.
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

casing_suffix_context_t
FUNC1 (const UNIT *s, size_t n)
{
  return FUNC2 (s, n, unicase_empty_suffix_context);
}

casing_suffix_context_t
FUNC2 (const UNIT *s, size_t n, casing_suffix_context_t a_context)
{
  casing_suffix_context_t context;
  /* Evaluate all three conditions in a single pass through the string S.
     The three variables are -1 as long as the value of the condition has
     not been determined.  */
  ucs4_t first_char_except_ignorable = (ucs4_t)(-1);
  int scc_MORE_ABOVE = -1;
  int scc_BEFORE_DOT = -1;
  const UNIT *s_end = s + n;

  while (s < s_end)
    {
      ucs4_t uc;
      int count = U_MBTOUC_UNSAFE (&uc, s, s_end - s);

      if (first_char_except_ignorable == (ucs4_t)(-1))
        {
          if (!uc_is_case_ignorable (uc))
            first_char_except_ignorable = uc;
        }

      if (scc_MORE_ABOVE < 0)
        {
          int ccc = uc_combining_class (uc);
          if (ccc == UC_CCC_A)
            scc_MORE_ABOVE = SCC_MORE_ABOVE_MASK;
          else if (ccc == UC_CCC_NR)
            scc_MORE_ABOVE = 0;
        }

      if (scc_BEFORE_DOT < 0)
        {
          if (uc == 0x0307) /* COMBINING DOT ABOVE */
            scc_BEFORE_DOT = SCC_BEFORE_DOT_MASK;
          else
            {
              int ccc = uc_combining_class (uc);
              if (ccc == UC_CCC_A || ccc == UC_CCC_NR)
                scc_BEFORE_DOT = 0;
            }
        }

      if (first_char_except_ignorable != (ucs4_t)(-1)
          && (scc_MORE_ABOVE | scc_BEFORE_DOT) >= 0)
        /* All conditions have been determined.  */
        break;

      s += count;
    }

  /* For those conditions that have not been determined so far, use the
     value from the argument context.  */
  context.first_char_except_ignorable =
    (first_char_except_ignorable != (ucs4_t)(-1)
     ? first_char_except_ignorable
     : a_context.first_char_except_ignorable);
  context.bits =
    (scc_MORE_ABOVE >= 0
     ? scc_MORE_ABOVE
     : a_context.bits & SCC_MORE_ABOVE_MASK)
    | (scc_BEFORE_DOT >= 0
       ? scc_BEFORE_DOT
       : a_context.bits & SCC_BEFORE_DOT_MASK);
  return context;
}
