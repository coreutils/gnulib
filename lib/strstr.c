/* Copyright (C) 1991,92,93,94,96,97,98,2000,2004,2007,2008 Free Software
   Foundation, Inc.
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

/* This particular implementation was written by Eric Blake, 2008.  */

#ifndef _LIBC
# include <config.h>
#endif

/* Specification of strstr.  */
#include <string.h>

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef _LIBC
# define __builtin_expect(expr, val)   (expr)
#endif

/* We use the Two-Way string matching algorithm, which guarantees
   linear complexity with constant space.  Additionally, for long
   needles, we also use a bad character shift table similar to the
   Boyer-Moore algorithm to achieve better performance.

   See http://www-igm.univ-mlv.fr/~lecroq/string/node26.html#SECTION00260
   and http://en.wikipedia.org/wiki/Boyer-Moore_string_search_algorithm
*/

/* Point at which computing a bad-byte shift table is likely to be
   worthwhile.  Small needles should not compute a table, since it
   adds (1 << CHAR_BIT) + NEEDLE_LEN computations of preparation for a
   speedup no greater than a factor of NEEDLE_LEN.  The larger the
   needle, the better the potential performance gain.  On the other
   hand, on non-POSIX systems with CHAR_BIT larger than eight, the
   memory required for the table is prohibitive.  */
#if CHAR_BIT < 10
# define LONG_NEEDLE_THRESHOLD 32U
#else
# define LONG_NEEDLE_THRESHOLD SIZE_MAX
#endif

#define MAX(a, b) ((a < b) ? (b) : (a))

/* Perform a critical factorization of NEEDLE, of length NEEDLE_LEN.
   Return the index of the first byte in the right half, and set
   *PERIOD to the global period of the right half.

   The global period of a string is the smallest index (possibly its
   length) at which all remaining bytes in the string are repetitions
   of the prefix (the last repetition may be a subset of the prefix).

   When NEEDLE is factored into two halves, a local period is the
   length of the smallest word that shares a suffix with the left half
   and shares a prefix with the right half.  All factorizations of a
   non-empty NEEDLE have a local period of at least 1 and no greater
   than NEEDLE_LEN.

   A critical factorization has the property that the local period
   equals the global period.  All strings have at least one critical
   factorization with the left half smaller than the global period.

   Given an ordered alphabet, a critical factorization can be computed
   in linear time, with 2 * NEEDLE_LEN comparisons, by computing the
   larger of two ordered maximal suffixes.  The ordered maximal
   suffixes are determined by lexicographic comparison of
   periodicity.  */
static size_t
critical_factorization (const unsigned char *needle, size_t needle_len,
			size_t *period)
{
  /* Index of last byte of left half, or SIZE_MAX.  */
  size_t max_suffix, max_suffix_rev;
  size_t j; /* Index into NEEDLE for current candidate suffix.  */
  size_t k; /* Offset into current period.  */
  size_t p; /* Intermediate period.  */
  unsigned char a, b; /* Current comparison bytes.  */

  /* Invariants:
     0 <= j < NEEDLE_LEN - 1
     -1 <= max_suffix{,_rev} < j (treating SIZE_MAX as if it were signed)
     min(max_suffix, max_suffix_rev) < global period of NEEDLE
     1 <= p <= global period of NEEDLE
     p == global period of the substring NEEDLE[max_suffix{,_rev}+1...j]
     1 <= k <= p
  */

  /* Perform lexicographic search.  */
  max_suffix = SIZE_MAX;
  j = 0;
  k = p = 1;
  while (j + k < needle_len)
    {
      a = needle[j + k];
      b = needle[max_suffix + k];
      if (a < b)
	{
	  /* Suffix is smaller, period is entire prefix so far.  */
	  j += k;
	  k = 1;
	  p = j - max_suffix;
	}
      else if (a == b)
	{
	  /* Advance through repetition of the current period.  */
	  if (k != p)
	    ++k;
	  else
	    {
	      j += p;
	      k = 1;
	    }
	}
      else /* b < a */
	{
	  /* Suffix is larger, start over from current location.  */
	  max_suffix = j++;
	  k = p = 1;
	}
    }
  *period = p;

  /* Perform reverse lexicographic search.  */
  max_suffix_rev = SIZE_MAX;
  j = 0;
  k = p = 1;
  while (j + k < needle_len)
    {
      a = needle[j + k];
      b = needle[max_suffix_rev + k];
      if (b < a)
	{
	  /* Suffix is smaller, period is entire prefix so far.  */
	  j += k;
	  k = 1;
	  p = j - max_suffix_rev;
	}
      else if (a == b)
	{
	  /* Advance through repetition of the current period.  */
	  if (k != p)
	    ++k;
	  else
	    {
	      j += p;
	      k = 1;
	    }
	}
      else /* a < b */
	{
	  /* Suffix is larger, start over from current location.  */
	  max_suffix_rev = j++;
	  k = p = 1;
	}
    }

  /* Choose the longer suffix.  Return the first byte of the right
     half, rather than the last byte of the left half.  */
  if (max_suffix_rev + 1 < max_suffix + 1)
    return max_suffix + 1;
  *period = p;
  return max_suffix_rev + 1;
}

/* Return the first location of NEEDLE within HAYSTACK, or NULL.  This
   method requires 0 < NEEDLE_LEN <= HAYSTACK_LEN, and is optimized
   for NEEDLE_LEN < LONG_NEEDLE_THRESHOLD.  Performance is linear,
   with 2 * NEEDLE_LEN comparisons in preparation, and at most 3 *
   HAYSTACK_LEN - NEEDLE_LEN comparisons in searching.  */
static char *
two_way_short_needle (const unsigned char *haystack, size_t haystack_len,
		      const unsigned char *needle, size_t needle_len)
{
  size_t i; /* Index into current byte of NEEDLE.  */
  size_t j; /* Index into current window of HAYSTACK.  */
  size_t period; /* The period of the right half of needle.  */
  size_t suffix; /* The index of the right half of needle.  */

  /* Factor the needle into two halves, such that the left half is
     smaller than the global period, and the right half is
     periodic (with a period as large as NEEDLE_LEN - suffix).  */
  suffix = critical_factorization (needle, needle_len, &period);

  /* Perform the search.  Each iteration compares the right half
     first.  */
  if (memcmp (needle, needle + period, suffix) == 0)
    {
      /* Entire needle is periodic; a mismatch can only advance by the
	 period, so use memory to avoid rescanning known occurrences
	 of the period.  */
      size_t memory = 0;
      j = 0;
      while (!memchr (&haystack[haystack_len], '\0',
		      j + needle_len - haystack_len)
	     && (haystack_len = j + needle_len))
	{
	  /* Scan for matches in right half.  */
	  i = MAX (suffix, memory);
	  while (i < needle_len && needle[i] == haystack[i + j])
	    ++i;
	  if (needle_len <= i)
	    {
	      /* Scan for matches in left half.  */
	      i = suffix - 1;
	      while (memory < i + 1 && needle[i] == haystack[i + j])
		--i;
	      if (i + 1 < memory + 1)
		return (char *) (haystack + j);
	      /* No match, so remember how many repetitions of period
		 on the right half were scanned.  */
	      j += period;
	      memory = needle_len - period;
	    }
	  else
	    {
	      j += i - suffix + 1;
	      memory = 0;
	    }
	}
    }
  else
    {
      /* The two halves of needle are distinct; no extra memory is
	 required, and any mismatch results in a maximal shift.  */
      period = MAX (suffix, needle_len - suffix) + 1;
      j = 0;
      while (!memchr (&haystack[haystack_len], '\0',
		      j + needle_len - haystack_len)
	     && (haystack_len = j + needle_len))
	{
	  /* Scan for matches in right half.  */
	  i = suffix;
	  while (i < needle_len && needle[i] == haystack[i + j])
	    ++i;
	  if (needle_len <= i)
	    {
	      /* Scan for matches in left half.  */
	      i = suffix - 1;
	      while (i != SIZE_MAX && needle[i] == haystack[i + j])
		--i;
	      if (i == SIZE_MAX)
		return (char *) (haystack + j);
	      j += period;
	    }
	  else
	    j += i - suffix + 1;
	}
    }
  return NULL;
}

/* Return the first location of NEEDLE within HAYSTACK, or NULL.  This
   method requires 0 < NEEDLE_LEN <= HAYSTACK_LEN, and is optimized
   for LONG_NEEDLE_THRESHOLD <= NEEDLE_LEN.  Performance is linear,
   with 3 * NEEDLE_LEN + (1U << CHAR_BIT) operations in preparation,
   and at most 3 * HAYSTACK_LEN - NEEDLE_LEN comparisons in searching.
   The extra initialization cost allows for as few as HAYSTACK_LEN +
   HAYSTACK_LEN / NEEDLE_LEN.  */
static char *
two_way_long_needle (const unsigned char *haystack, size_t haystack_len,
		     const unsigned char *needle, size_t needle_len)
{
  size_t i; /* Index into current byte of NEEDLE.  */
  size_t j; /* Index into current window of HAYSTACK.  */
  size_t period; /* The period of the right half of needle.  */
  size_t suffix; /* The index of the right half of needle.  */
  size_t shift_table[1U << CHAR_BIT]; /* See below.  */

  /* Factor the needle into two halves, such that the left half is
     smaller than the global period, and the right half is
     periodic (with a period as large as NEEDLE_LEN - suffix).  */
  suffix = critical_factorization (needle, needle_len, &period);

  /* Populate shift_table.  For each possible byte value c,
     shift_table[c] is the distance from the last occurrence of c to
     the end of NEEDLE, or NEEDLE_LEN if c is absent from the NEEDLE.
     shift_table[NEEDLE[NEEDLE_LEN - 1]] contains the only 0.  */
  for (i = 0; i < 1U << CHAR_BIT; i++)
    shift_table[i] = needle_len;
  for (i = 0; i < needle_len; i++)
    shift_table[needle[i]] = needle_len - i - 1;

  /* Perform the search.  Each iteration compares the right half
     first.  */
  if (memcmp (needle, needle + period, suffix) == 0)
    {
      /* Entire needle is periodic; a mismatch can only advance by the
	 period, so use memory to avoid rescanning known occurrences
	 of the period.  */
      size_t memory = 0;
      j = 0;
      while (!memchr (&haystack[haystack_len], '\0',
		      j + needle_len - haystack_len)
	     && (haystack_len = j + needle_len))
	{
	  /* Check the last byte first; if it does not match, then
	     shift to the next possible match location.  */
	  size_t shift = shift_table[haystack[j + needle_len - 1]];
	  if (0 < shift)
	    {
	      if (memory && shift < period)
		{
		  /* Since needle is periodic, but the last period has
		     a byte out of place, there can be no match until
		     after the mismatch.  */
		  shift = needle_len - period;
		  memory = 0;
		}
	      j += shift;
	      continue;
	    }
	  /* Scan for matches in right half.  The last byte has
	     already been matched, by virtue of the shift table.  */
	  i = MAX (suffix, memory);
	  while (i < needle_len - 1 && needle[i] == haystack[i + j])
	    ++i;
	  if (needle_len - 1 <= i)
	    {
	      /* Scan for matches in left half.  */
	      i = suffix - 1;
	      while (memory < i + 1 && needle[i] == haystack[i + j])
		--i;
	      if (i + 1 < memory + 1)
		return (char *) (haystack + j);
	      /* No match, so remember how many repetitions of period
		 on the right half were scanned.  */
	      j += period;
	      memory = needle_len - period;
	    }
	  else
	    {
	      j += i - suffix + 1;
	      memory = 0;
	    }
	}
    }
  else
    {
      /* The two halves of needle are distinct; no extra memory is
	 required, and any mismatch results in a maximal shift.  */
      period = MAX (suffix, needle_len - suffix) + 1;
      j = 0;
      while (!memchr (&haystack[haystack_len], '\0',
		      j + needle_len - haystack_len)
	     && (haystack_len = j + needle_len))
	{
	  /* Check the last byte first; if it does not match, then
	     shift to the next possible match location.  */
	  size_t shift = shift_table[haystack[j + needle_len - 1]];
	  if (0 < shift)
	    {
	      j += shift;
	      continue;
	    }
	  /* Scan for matches in right half.  The last byte has
	     already been matched, by virtue of the shift table.  */
	  i = suffix;
	  while (i < needle_len - 1 && needle[i] == haystack[i + j])
	    ++i;
	  if (needle_len - 1 <= i)
	    {
	      /* Scan for matches in left half.  */
	      i = suffix - 1;
	      while (i != SIZE_MAX && needle[i] == haystack[i + j])
		--i;
	      if (i == SIZE_MAX)
		return (char *) (haystack + j);
	      j += period;
	    }
	  else
	    j += i - suffix + 1;
	}
    }
  return NULL;
}

/* Return the first occurrence of NEEDLE in HAYSTACK.  Return HAYSTACK
   if NEEDLE is empty, otherwise NULL if NEEDLE is not found in
   HAYSTACK.  */
char *
strstr (const char *haystack_start, const char *needle_start)
{
  const char *haystack = haystack_start;
  const char *needle = needle_start;
  size_t needle_len; /* Length of NEEDLE.  */
  size_t haystack_len; /* Known minimum length of HAYSTACK.  */
  bool ok = true; /* True if NEEDLE is prefix of HAYSTACK.  */

  /* Determine length of NEEDLE, and in the process, make sure
     HAYSTACK is at least as long (no point processing all of a long
     NEEDLE if HAYSTACK is too short).  */
  while (*haystack && *needle)
    ok &= *haystack++ == *needle++;
  if (*needle)
    return NULL;
  if (ok)
    return (char *) haystack_start;

  /* Reduce the size of haystack using strchr, since it has a smaller
     linear coefficient than the Two-Way algorithm.  */
  needle_len = needle - needle_start;
  haystack = strchr (haystack_start + 1, *needle_start);
  if (!haystack || __builtin_expect (needle_len == 1, 0))
    return (char *) haystack;
  needle -= needle_len;
  haystack_len = (haystack > haystack_start + needle_len ? 1
		  : needle_len + haystack_start - haystack);

  /* Perform the search.  Abstract memory is considered to be an array
     of 'unsigned char' values, not an array of 'char' values.  See
     ISO C 99 section 6.2.6.1.  */
  if (needle_len < LONG_NEEDLE_THRESHOLD)
    return two_way_short_needle ((const unsigned char *) haystack,
				 haystack_len,
				 (const unsigned char *) needle, needle_len);
  return two_way_long_needle ((const unsigned char *) haystack, haystack_len,
			      (const unsigned char *) needle, needle_len);
}

#undef LONG_NEEDLE_THRESHOLD
#undef MAX
