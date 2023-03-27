/* Locate a substring in a wide string.
   Copyright (C) 1999, 2011-2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible, 1999, and Eric Blake, 2008.  */

#if NEED_LINEAR_WCSSTR

/* Use Two-Way algorithm, worst-case O(n+m).  */

# define RETURN_TYPE wchar_t *
# define AVAILABLE(h, h_l, j, n_l)                       \
   (!wmemchr ((h) + (h_l), L'\0', (j) + (n_l) - (h_l))   \
    && ((h_l) = (j) + (n_l)))
# include "wcs-two-way.h"

wchar_t *
wcsstr (const wchar_t *haystack_start, const wchar_t *needle_start)
{
  const wchar_t *haystack = haystack_start;
  const wchar_t *needle = needle_start;
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
    return (wchar_t *) haystack_start;

  /* Reduce the size of haystack using wcschr, since it has a smaller
     linear coefficient than the Two-Way algorithm.  */
  needle_len = needle - needle_start;
  haystack = wcschr (haystack_start + 1, *needle_start);
  if (!haystack || __builtin_expect (needle_len == 1, 0))
    return (wchar_t *) haystack;
  needle -= needle_len;
  haystack_len = (haystack > haystack_start + needle_len ? 1
                  : needle_len + haystack_start - haystack);

  /* Perform the search.  */
  return two_way_short_needle (haystack, haystack_len,
                               needle, needle_len);
}

#else

/* Use simple implementation, worst-case O(nm).  */

wchar_t *
wcsstr (const wchar_t *haystack, const wchar_t *needle)
{
  wchar_t n = needle[0];

  /* Is needle empty?  */
  if (n == (wchar_t)'\0')
    return (wchar_t *) haystack;

  /* Is needle nearly empty?  */
  if (needle[1] == (wchar_t)'\0')
    return wcschr (haystack, n);

  /* Search for needle's first character.  */
  for (; *haystack != (wchar_t)'\0'; haystack++)
    {
      if (*haystack == n)
        {
          /* Compare with needle's remaining characters.  */
          const wchar_t *hptr = haystack + 1;
          const wchar_t *nptr = needle + 1;
          for (;;)
            {
              if (*hptr != *nptr)
                break;
              hptr++; nptr++;
              if (*nptr == (wchar_t)'\0')
                return (wchar_t *) haystack;
            }
        }
    }

  return NULL;
}

#endif
