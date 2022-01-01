/* Substring test for UTF-8/UTF-16/UTF-32 strings.  -*- coding: utf-8 -*-
   Copyright (C) 1999, 2002, 2006, 2010-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002, 2005.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation; either version 3, or (at your
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

UNIT *
FUNC (const UNIT *haystack, const UNIT *needle)
{
  UNIT first = needle[0];

  /* Is needle empty?  */
  if (first == 0)
    return (UNIT *) haystack;

  /* Is needle nearly empty (only one unit)?  */
  if (needle[1] == 0)
    return U_STRCHR (haystack, first);

#ifdef U_STRMBTOUC
  /* Is needle nearly empty (only one character)?  */
  {
    ucs4_t first_uc;
    int count = U_STRMBTOUC (&first_uc, needle);
    if (count > 0 && needle[count] == 0)
      return U_STRCHR (haystack, first_uc);
  }
#endif

#if UNIT_IS_UINT8_T
  return (uint8_t *) strstr ((const char *) haystack, (const char *) needle);
#else
  {
    /* Minimizing the worst-case complexity:
       Let n = U_STRLEN(haystack), m = U_STRLEN(needle).
       The naïve algorithm is O(n*m) worst-case.
       The Knuth-Morris-Pratt algorithm is O(n) worst-case but it needs a
       memory allocation.
       To achieve linear complexity and yet amortize the cost of the
       memory allocation, we activate the Knuth-Morris-Pratt algorithm
       only once the naïve algorithm has already run for some time; more
       precisely, when
         - the outer loop count is >= 10,
         - the average number of comparisons per outer loop is >= 5,
         - the total number of comparisons is >= m.
       But we try it only once.  If the memory allocation attempt failed,
       we don't retry it.  */
    bool try_kmp = true;
    size_t outer_loop_count = 0;
    size_t comparison_count = 0;
    size_t last_ccount = 0;                  /* last comparison count */
    const UNIT *needle_last_ccount = needle; /* = needle + last_ccount */

    /* Speed up the following searches of needle by caching its first
       character.  */
    UNIT b = *needle++;

    for (;; haystack++)
      {
        if (*haystack == 0)
          /* No match.  */
          return NULL;

        /* See whether it's advisable to use an asymptotically faster
           algorithm.  */
        if (try_kmp
            && outer_loop_count >= 10
            && comparison_count >= 5 * outer_loop_count)
          {
            /* See if needle + comparison_count now reaches the end of
               needle.  */
            if (needle_last_ccount != NULL)
              {
                needle_last_ccount +=
                  U_STRNLEN (needle_last_ccount,
                             comparison_count - last_ccount);
                if (*needle_last_ccount == 0)
                  needle_last_ccount = NULL;
                last_ccount = comparison_count;
              }
            if (needle_last_ccount == NULL)
              {
                /* Try the Knuth-Morris-Pratt algorithm.  */
                const UNIT *result;
                bool success =
                  knuth_morris_pratt (haystack,
                                      needle - 1, U_STRLEN (needle - 1),
                                      &result);
                if (success)
                  return (UNIT *) result;
                try_kmp = false;
              }
          }

        outer_loop_count++;
        comparison_count++;
        if (*haystack == b)
          /* The first character matches.  */
          {
            const UNIT *rhaystack = haystack + 1;
            const UNIT *rneedle = needle;

            for (;; rhaystack++, rneedle++)
              {
                if (*rneedle == 0)
                  /* Found a match.  */
                  return (UNIT *) haystack;
                if (*rhaystack == 0)
                  /* No match.  */
                  return NULL;
                comparison_count++;
                if (*rhaystack != *rneedle)
                  /* Nothing in this round.  */
                  break;
              }
          }
      }
  }
#endif
}
