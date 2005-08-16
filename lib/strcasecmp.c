/* Case-insensitive string comparison function.
   Copyright (C) 1998, 1999, 2005 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2005,
   based on earlier glibc code.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Specification.  */
#include "strcase.h"

#include <ctype.h>

#if HAVE_MBRTOWC

#include "strnlen1.h"

/* Like mbiter.h, except it doesn't look at the entire string.  */

#include "mbchar.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

struct mbiter_multi
{
  bool at_end;		/* true if the end of the string has been reached */
  bool in_shift;	/* true if next byte may not be interpreted as ASCII */
  mbstate_t state;	/* if in_shift: current shift state */
  bool next_done;	/* true if mbi_avail has already filled the following */
  struct mbchar cur;	/* the current character:
	const char *cur.ptr		pointer to current character
	The following are only valid after mbi_avail.
	size_t cur.bytes		number of bytes of current character
	bool cur.wc_valid		true if wc is a valid wide character
	wchar_t cur.wc			if wc_valid: the current character
	*/
};

static inline void
mbiter_multi_next (struct mbiter_multi *iter)
{
  if (iter->next_done)
    return;
  if (iter->in_shift)
    goto with_shift;
  /* Handle most ASCII characters quickly, without calling mbrtowc().  */
  if (is_basic (*iter->cur.ptr))
    {
      /* These characters are part of the basic character set.  ISO C 99
	 guarantees that their wide character code is identical to their
	 char code.  */
      iter->cur.bytes = 1;
      iter->cur.wc = *iter->cur.ptr;
      iter->cur.wc_valid = true;
    }
  else
    {
      assert (mbsinit (&iter->state));
      iter->in_shift = true;
    with_shift:
      iter->cur.bytes = mbrtowc (&iter->cur.wc, iter->cur.ptr,
				 strnlen1 (iter->cur.ptr, MB_CUR_MAX),
				 &iter->state);
      if (iter->cur.bytes == (size_t) -1)
	{
	  /* An invalid multibyte sequence was encountered.  */
	  iter->cur.bytes = 1;
	  iter->cur.wc_valid = false;
	  /* Whether to set iter->in_shift = false and reset iter->state
	     or not is not very important; the string is bogus anyway.  */
	}
      else if (iter->cur.bytes == (size_t) -2)
	{
	  /* An incomplete multibyte character at the end.  */
	  iter->cur.bytes = strlen (iter->cur.ptr) + 1;
	  iter->cur.wc_valid = false;
	  /* Whether to set iter->in_shift = false and reset iter->state
	     or not is not important; the string end is reached anyway.  */
	}
      else
	{
	  if (iter->cur.bytes == 0)
	    {
	      /* A null wide character was encountered.  */
	      iter->cur.bytes = 1;
	      assert (*iter->cur.ptr == '\0');
	      assert (iter->cur.wc == 0);
	    }
	  iter->cur.wc_valid = true;

	  /* When in the initial state, we can go back treating ASCII
	     characters more quickly.  */
	  if (mbsinit (&iter->state))
	    iter->in_shift = false;
	}
    }
  iter->next_done = true;
}

/* Iteration macros.  */
typedef struct mbiter_multi mbi_iterator_t;
#define mbi_init(iter, startptr) \
  ((iter).cur.ptr = (startptr), (iter).at_end = false, \
   (iter).in_shift = false, memset (&(iter).state, '\0', sizeof (mbstate_t)), \
   (iter).next_done = false)
#define mbi_avail(iter) \
  (!(iter).at_end && (mbiter_multi_next (&(iter)), true))
#define mbi_advance(iter) \
  ((mb_isnul ((iter).cur) ? ((iter).at_end = true) : 0), \
   (iter).cur.ptr += (iter).cur.bytes, (iter).next_done = false)

/* Access to the current character.  */
#define mbi_cur(iter) (iter).cur
#define mbi_cur_ptr(iter) (iter).cur.ptr

#endif

#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

/* Compare strings S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than, equal to or greater
   than S2.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!  */
int
strcasecmp (const char *s1, const char *s2)
{
  if (s1 == s2)
    return 0;

  /* Be careful not to look at the entire extent of s1 or s2 until needed.
     This is useful because when two strings differ, the difference is
     most often already in the very few first characters.  */
#if HAVE_MBRTOWC
  if (MB_CUR_MAX > 1)
    {
      mbi_iterator_t iter1;
      mbi_iterator_t iter2;

      mbi_init (iter1, s1);
      mbi_init (iter2, s2);

      while (mbi_avail (iter1) && mbi_avail (iter2))
	{
	  int cmp = mb_casecmp (mbi_cur (iter1), mbi_cur (iter2));

	  if (cmp != 0)
	    return cmp;

	  mbi_advance (iter1);
	  mbi_advance (iter2);
	}
      if (mbi_avail (iter1))
	/* s2 terminated before s1.  */
	return 1;
      if (mbi_avail (iter2))
	/* s1 terminated before s2.  */
	return -1;
      return 0;
    }
  else
#endif
    {
      const unsigned char *p1 = (const unsigned char *) s1;
      const unsigned char *p2 = (const unsigned char *) s2;
      unsigned char c1, c2;

      do
	{
	  c1 = TOLOWER (*p1);
	  c2 = TOLOWER (*p2);

	  if (c1 == '\0')
	    break;

	  ++p1;
	  ++p2;
	}
      while (c1 == c2);

      return c1 - c2;
    }
}
