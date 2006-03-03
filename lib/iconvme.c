/* Recode strings between character sets, using iconv.
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Get prototype. */
#include "iconvme.h"

/* Get malloc. */
#include <stdlib.h>

/* Get strcmp. */
#include <string.h>

/* Get errno. */
#include <errno.h>

#ifdef _LIBC
# define HAVE_ICONV 1
#else
/* Get strdup. */
# include "strdup.h"
#endif

#if HAVE_ICONV
/* Get iconv etc. */
# include <iconv.h>
/* Get MB_LEN_MAX, CHAR_BIT.  */
# include <limits.h>
#endif

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif

/* Convert a zero-terminated string STR from the FROM_CODSET code set
   to the TO_CODESET code set.  The returned string is allocated using
   malloc, and must be dellocated by the caller using free.  On
   failure, NULL is returned and errno holds the error reason.  Note
   that if TO_CODESET uses \0 for anything but to terminate the
   string, the caller of this function may have difficulties finding
   out the length of the output string.  */
char *
iconv_string (const char *str, const char *from_codeset,
	      const char *to_codeset)
{
  char *dest = NULL;
#if HAVE_ICONV
  iconv_t cd;
#endif

  if (strcmp (to_codeset, from_codeset) == 0)
    return strdup (str);

#if HAVE_ICONV
  cd = iconv_open (to_codeset, from_codeset);
  if (cd == (iconv_t) -1)
    return NULL;

  dest = iconv_alloc (cd, str);

  {
    int save_errno = errno;

    if (iconv_close (cd) < 0 && dest)
      {
	int save_errno2 = errno;
	/* If we didn't have a real error before, make sure we restore
	   the iconv_close error below. */
	free (dest);
	dest = NULL;
	errno = save_errno2;
      }
    else
      errno = save_errno;
  }
#else
  errno = ENOSYS;
#endif

  return dest;
}

/* Convert a zero-terminated string STR using iconv descriptor CD.
   The returned string is allocated using malloc, and must be
   dellocated by the caller using free.  On failure, NULL is returned
   and errno holds the error reason.  Note that if the target
   character set uses \0 for anything but to terminate the string,
   the caller of this function may have difficulties finding
   out the length of the output string.  */
#if HAVE_ICONV
char *
iconv_alloc (iconv_t cd, const char *str)
{
  char *dest;
  char *p = (char *) str;
  char *outp;
  size_t inbytes_remaining = strlen (p);
  /* Guess the maximum length the output string can have.  */
  size_t outbuf_size = inbytes_remaining + 1;
  size_t outbytes_remaining;
  size_t err;
  int have_error = 0;

  /* Use a worst-case output size guess, so long as that wouldn't be
     too large for comfort.  It's OK if the guess is wrong so long as
     it's nonzero.  */
  size_t approx_sqrt_SIZE_MAX = SIZE_MAX >> (sizeof (size_t) * CHAR_BIT / 2);
  if (outbuf_size <= approx_sqrt_SIZE_MAX / MB_LEN_MAX)
    outbuf_size *= MB_LEN_MAX;
  outbytes_remaining = outbuf_size - 1;

  outp = dest = (char *) malloc (outbuf_size);
  if (dest == NULL)
    return NULL;

again:
  err = iconv (cd, &p, &inbytes_remaining, &outp, &outbytes_remaining);

  if (err == (size_t) -1)
    {
      switch (errno)
	{
	case EINVAL:
	  /* Incomplete text, do not report an error */
	  break;

	case E2BIG:
	  {
	    size_t used = outp - dest;
	    size_t newsize = outbuf_size * 2;
	    char *newdest;

	    if (newsize <= outbuf_size)
	      {
		errno = ENOMEM;
		have_error = 1;
		goto out;
	      }
	    newdest = (char *) realloc (dest, newsize);
	    if (newdest == NULL)
	      {
		have_error = 1;
		goto out;
	      }
	    dest = newdest;
	    outbuf_size = newsize;

	    outp = dest + used;
	    outbytes_remaining = outbuf_size - used - 1;	/* -1 for NUL */

	    goto again;
	  }
	  break;

	case EILSEQ:
	  have_error = 1;
	  break;

	default:
	  have_error = 1;
	  break;
	}
    }

  *outp = '\0';

out:
  if (have_error && dest)
    {
      int save_errno = errno;
      free (dest);
      errno = save_errno;
      dest = NULL;
    }

  return dest;
}
#endif
