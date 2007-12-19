/* Copyright (C) 1991,92,93,94,96,97,98,2000,2004,2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _LIBC
# include <config.h>
#endif

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "malloca.h"

#ifndef _LIBC
# define __builtin_expect(expr, val)   (expr)
#endif

/* Knuth-Morris-Pratt algorithm.
   See http://en.wikipedia.org/wiki/Knuth-Morris-Pratt_algorithm
   Return a boolean indicating success.  */

static bool
knuth_morris_pratt (const char *haystack, const char *last_haystack,
                    const char *needle, size_t m,
                    const char **resultp)
{
  /* Allocate the table.  */
  size_t *table = (size_t *) malloca (m * sizeof (size_t));
  if (table == NULL)
    return false;
  /* Fill the table.
     For 0 < i < m:
       0 < table[i] <= i is defined such that
       rhaystack[0..i-1] == needle[0..i-1] and rhaystack[i] != needle[i]
       implies
       forall 0 <= x < table[i]: rhaystack[x..x+m-1] != needle[0..m-1],
       and table[i] is as large as possible with this property.
     table[0] remains uninitialized.  */
  {
    size_t i, j;

    table[1] = 1;
    j = 0;
    for (i = 2; i < m; i++)
      {
	unsigned char b = (unsigned char) needle[i - 1];

	for (;;)
	  {
	    if (b == (unsigned char) needle[j])
	      {
		table[i] = i - ++j;
		break;
	      }
	    if (j == 0)
	      {
		table[i] = i;
		break;
	      }
	    j = j - table[j];
	  }
      }
  }

  /* Search, using the table to accelerate the processing.  */
  {
    size_t j;
    const char *rhaystack;
    const char *phaystack;

    *resultp = NULL;
    j = 0;
    rhaystack = haystack;
    phaystack = haystack;
    /* Invariant: phaystack = rhaystack + j.  */
    while (phaystack != last_haystack)
      if ((unsigned char) needle[j] == (unsigned char) *phaystack)
	{
	  j++;
	  phaystack++;
	  if (j == m)
	    {
	      /* The entire needle has been found.  */
	      *resultp = rhaystack;
	      break;
	    }
	}
      else if (j > 0)
	{
	  /* Found a match of needle[0..j-1], mismatch at needle[j].  */
	  rhaystack += table[j];
	  j -= table[j];
	}
      else
	{
	  /* Found a mismatch at needle[0] already.  */
	  rhaystack++;
	  phaystack++;
	}
  }

  freea (table);
  return true;
}

/* Return the first occurrence of NEEDLE in HAYSTACK.  Return HAYSTACK
   if NEEDLE_LEN is 0, otherwise NULL if NEEDLE is not found in
   HAYSTACK.  */
void *
memmem (const void *haystack, size_t haystack_len,
        const void *needle, size_t needle_len)
{
  /* Operating with void * is awkward.  */
  const char *Haystack = (const char *) haystack;
  const char *Needle = (const char *) needle;
  const char *last_haystack = Haystack + haystack_len;
  const char *last_needle = Needle + needle_len;

  if (needle_len == 0)
    /* The first occurrence of the empty string is deemed to occur at
       the beginning of the string.  */
    return (void *) haystack;

  /* Sanity check, otherwise the loop might search through the whole
     memory.  */
  if (__builtin_expect (haystack_len < needle_len, 0))
    return NULL;

  /* Use optimizations in memchr when possible.  */
  if (__builtin_expect (needle_len == 1, 0))
    return memchr (haystack, (unsigned char) *Needle, haystack_len);

  /* Minimizing the worst-case complexity:
     Let n = haystack_len, m = needle_len.
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
  {
    bool try_kmp = true;
    size_t outer_loop_count = 0;
    size_t comparison_count = 0;

    /* Speed up the following searches of needle by caching its first
       character.  */
    char b = *Needle++;

    for (;; Haystack++)
      {
        if (Haystack == last_haystack)
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
            if (comparison_count >= needle_len)
              {
                /* Try the Knuth-Morris-Pratt algorithm.  */
                const char *result;
                if (knuth_morris_pratt (Haystack, last_haystack,
                                        Needle - 1, needle_len, &result))
                  return (void *) result;
                try_kmp = false;
              }
          }

        outer_loop_count++;
        comparison_count++;
        if (*Haystack == b)
          /* The first character matches.  */
          {
            const char *rhaystack = Haystack + 1;
            const char *rneedle = Needle;

            for (;; rhaystack++, rneedle++)
              {
                if (rneedle == last_needle)
                  /* Found a match.  */
                  return (void *) Haystack;
                if (rhaystack == last_haystack)
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

  return NULL;
}
