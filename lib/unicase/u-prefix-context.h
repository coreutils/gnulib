/* Case-mapping context of prefix UTF-8/UTF-16/UTF-32 string.
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

casing_prefix_context_t
FUNC1 (const UNIT *s, size_t n)
{
  return FUNC2 (s, n, unicase_empty_prefix_context);
}

casing_prefix_context_t
FUNC2 (const UNIT *s, size_t n, casing_prefix_context_t a_context)
{
#if 0
  /* Forward iteration.  Slow for long strings.  */
  casing_prefix_context_t context = a_context;
  const UNIT *s_end = s + n;

  while (s < s_end)
    {
      ucs4_t uc;
      int count = U_MBTOUC_UNSAFE (&uc, s, s_end - s);

      if (!uc_is_case_ignorable (uc))
        context.last_char_except_ignorable = uc;

      {
        int ccc = uc_combining_class (uc);
        if (ccc == UC_CCC_A || ccc == UC_CCC_NR)
          context.last_char_normal_or_above = uc;
      }

      s += count;
    }

  return context;
#else
  /* Iterate backwards, only as far as needed.  */
  casing_prefix_context_t context;
  ucs4_t last_char_except_ignorable = (ucs4_t)(-1);
  ucs4_t last_char_normal_or_above = (ucs4_t)(-1);
  const UNIT *p = s + n;

  for (;;)
    {
      ucs4_t uc;
      p = U_PREV (&uc, p, s);
      if (p == NULL)
        break;

      if (last_char_except_ignorable == (ucs4_t)(-1))
        {
          if (!uc_is_case_ignorable (uc))
            last_char_except_ignorable = uc;
        }

      if (last_char_normal_or_above == (ucs4_t)(-1))
        {
          int ccc = uc_combining_class (uc);
          if (ccc == UC_CCC_A || ccc == UC_CCC_NR)
            last_char_normal_or_above = uc;
        }

      if (last_char_except_ignorable != (ucs4_t)(-1)
          && last_char_normal_or_above != (ucs4_t)(-1))
        break;
    }
  context.last_char_except_ignorable =
    (last_char_except_ignorable != (ucs4_t)(-1)
     ? last_char_except_ignorable
     : a_context.last_char_except_ignorable);
  context.last_char_normal_or_above =
    (last_char_normal_or_above != (ucs4_t)(-1)
     ? last_char_normal_or_above
     : a_context.last_char_normal_or_above);

  return context;
#endif
}
