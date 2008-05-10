/* Line breaking auxiliary functions.
   Copyright (C) 2001-2003, 2006-2008 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

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

#include <config.h>

/* Specification.  */
#include "unilbrk/ulc-common.h"

#include <stdlib.h>

#include "c-ctype.h"
#include "streq.h"

int
is_utf8_encoding (const char *encoding)
{
  if (STREQ (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0 ,0))
    return 1;
  return 0;
}

#if HAVE_ICONV

# include <errno.h>

size_t
iconv_string_length (iconv_t cd, const char *s, size_t n)
{
# define TMPBUFSIZE 4096
  size_t count = 0;
  char tmpbuf[TMPBUFSIZE];
  const char *inptr = s;
  size_t insize = n;

  while (insize > 0)
    {
      char *outptr = tmpbuf;
      size_t outsize = TMPBUFSIZE;
      size_t res = iconv (cd, (ICONV_CONST char **) &inptr, &insize, &outptr, &outsize);
      if (res == (size_t)(-1) && errno != E2BIG
# if !defined _LIBICONV_VERSION && !defined __GLIBC__
	  /* Irix iconv() inserts a NUL byte if it cannot convert.
	     NetBSD iconv() inserts a question mark if it cannot convert.
	     Only GNU libiconv and GNU libc are known to prefer to fail rather
	     than doing a lossy conversion.  */
	  || res > 0
# endif
	 )
	return (size_t)(-1);
      count += outptr - tmpbuf;
    }
  /* Avoid glibc-2.1 bug and Solaris 7 through 9 bug.  */
# if defined _LIBICONV_VERSION \
     || !((__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) || defined __sun)
  {
    char *outptr = tmpbuf;
    size_t outsize = TMPBUFSIZE;
    size_t res = iconv (cd, NULL, NULL, &outptr, &outsize);
    if (res == (size_t)(-1))
      return (size_t)(-1);
    count += outptr - tmpbuf;
  }
  /* Return to the initial state.  */
  iconv (cd, NULL, NULL, NULL, NULL);
# endif
  return count;
# undef TMPBUFSIZE
}

void
iconv_string_keeping_offsets (iconv_t cd, const char *s, size_t n,
			      size_t *offtable, char *t, size_t m)
{
  size_t i;
  const char *s_end;
  const char *inptr;
  char *outptr;
  size_t outsize;
  /* Avoid glibc-2.1 bug.  */
# if !defined _LIBICONV_VERSION && (__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1)
  const size_t extra = 1;
# else
  const size_t extra = 0;
# endif

  for (i = 0; i < n; i++)
    offtable[i] = (size_t)(-1);

  s_end = s + n;
  inptr = s;
  outptr = t;
  outsize = m + extra;
  while (inptr < s_end)
    {
      const char *saved_inptr;
      size_t insize;
      size_t res;

      offtable[inptr - s] = outptr - t;

      saved_inptr = inptr;
      res = (size_t)(-1);
      for (insize = 1; inptr + insize <= s_end; insize++)
	{
	  res = iconv (cd, (ICONV_CONST char **) &inptr, &insize, &outptr, &outsize);
	  if (!(res == (size_t)(-1) && errno == EINVAL))
	    break;
	  /* We expect that no input bytes have been consumed so far.  */
	  if (inptr != saved_inptr)
	    abort ();
	}
      /* After we verified the convertibility and computed the translation's
	 size m, there shouldn't be any conversion error here. */
      if (res == (size_t)(-1)
# if !defined _LIBICONV_VERSION && !defined __GLIBC__
	  /* Irix iconv() inserts a NUL byte if it cannot convert.
	     NetBSD iconv() inserts a question mark if it cannot convert.
	     Only GNU libiconv and GNU libc are known to prefer to fail rather
	     than doing a lossy conversion.  */
	  || res > 0
# endif
	 )
	abort ();
    }
  /* Avoid glibc-2.1 bug and Solaris 7 bug.  */
# if defined _LIBICONV_VERSION \
     || !((__GLIBC__ - 0 == 2 && __GLIBC_MINOR__ - 0 <= 1) || defined __sun)
  if (iconv (cd, NULL, NULL, &outptr, &outsize) == (size_t)(-1))
    abort ();
# endif
  /* We should have produced exactly m output bytes.  */
  if (outsize != extra)
    abort ();
}

#endif /* HAVE_ICONV */

#if C_CTYPE_ASCII

/* Tests whether a string is entirely ASCII.  Returns 1 if yes.
   Returns 0 if the string is in an 8-bit encoding or an ISO-2022 encoding.  */
int
is_all_ascii (const char *s, size_t n)
{
  for (; n > 0; s++, n--)
    {
      unsigned char c = (unsigned char) *s;

      if (!(c_isprint (c) || c_isspace (c)))
	return 0;
    }
  return 1;
}

#endif /* C_CTYPE_ASCII */
