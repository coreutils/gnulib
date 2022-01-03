/* Locale-specific case-ignoring memory comparison.
   Copyright (C) 2001, 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "mbmemcasecoll.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Get tolower().  */
#include <ctype.h>

/* Get mbstate_t, mbrtowc(), wcrtomb().  */
#include <wchar.h>

/* Get towlower().  */
#include <wctype.h>

#include "malloca.h"
#include "memcmp2.h"
#include "memcoll.h"

#define TOLOWER(Ch) (isupper (Ch) ? tolower (Ch) : (Ch))

/* Apply towlower() to the multibyte character sequence in INBUF, storing the
   result as a multibyte character sequence in OUTBUF.  */
static size_t
apply_towlower (const char *inbuf, size_t inbufsize,
                char *outbuf, size_t outbufsize)
{
  char *outbuf_orig = outbuf;
  size_t remaining;

  remaining = inbufsize;
  while (remaining > 0)
    {
      wchar_t wc1;
      size_t n1;
      mbstate_t state;

      memset (&state, '\0', sizeof (mbstate_t));
      n1 = mbrtowc (&wc1, inbuf, remaining, &state);
      if (n1 == (size_t)(-2))
        break;
      if (n1 != (size_t)(-1))
        {
          wint_t wc2;

          if (n1 == 0) /* NUL character? */
            n1 = 1;

          wc2 = towlower (wc1);
          if (wc2 != wc1)
            {
              size_t n2;

              memset (&state, '\0', sizeof (mbstate_t));
              n2 = wcrtomb (outbuf, wc2, &state);
              if (n2 != (size_t)(-1))
                {
                  /* Store the translated multibyte character.  */
                  inbuf += n1;
                  remaining -= n1;
                  outbuf += n2;
                  continue;
                }
            }

          /* Nothing to translate. */
          memcpy (outbuf, inbuf, n1);
          inbuf += n1;
          remaining -= n1;
          outbuf += n1;
          continue;
        }

      /* Invalid multibyte character on input.
         Copy one byte without modification.  */
      *outbuf++ = *inbuf++;
      remaining -= 1;
    }
  /* Incomplete multibyte sequence on input.
     Pass it through unmodified.  */
  while (remaining > 0)
    {
      *outbuf++ = *inbuf++;
      remaining -= 1;
    }

  /* Verify the output buffer was large enough.  */
  if (outbuf - outbuf_orig > outbufsize)
    abort ();

  /* Return the number of written output bytes.  */
  return outbuf - outbuf_orig;
}

/* Apply tolower() to the unibyte character sequence in INBUF, storing the
   result as a unibyte character sequence in OUTBUF.  */
static void
apply_tolower (const char *inbuf, char *outbuf, size_t bufsize)
{
  for (; bufsize > 0; bufsize--)
    {
      *outbuf = TOLOWER ((unsigned char) *inbuf);
      inbuf++;
      outbuf++;
    }
}

int
mbmemcasecoll (const char *s1, size_t s1len, const char *s2, size_t s2len,
               bool hard_LC_COLLATE)
{
  char *t1;
  size_t t1len;
  char *t2;
  size_t t2len;
  char *memory;
  int cmp;

  if (MB_CUR_MAX > 1)
    {
      /* Application of towlower grows each character by a factor 2
         at most.  */
      t1len = 2 * s1len;
      t2len = 2 * s2len;
    }
  else
    {
      /* Application of tolower doesn't change the size.  */
      t1len = s1len;
      t2len = s2len;
    }
  /* Allocate memory for t1 and t2.  */
  memory = (char *) malloca (t1len + 1 + t2len + 1);
  if (memory == NULL)
    {
      errno = ENOMEM;
      return 0;
    }
  t1 = memory;
  t2 = memory + t1len + 1;

  /* Csae-fold the two argument strings.  */
  if (MB_CUR_MAX > 1)
    {
      t1len = apply_towlower (s1, s1len, t1, t1len);
      t2len = apply_towlower (s2, s2len, t2, t2len);
    }
  else
    {
      apply_tolower (s1, t1, s1len);
      apply_tolower (s2, t2, s2len);
    }

  /* Compare the two case-folded strings.  */
  if (hard_LC_COLLATE)
    cmp = memcoll (t1, t1len, t2, t2len);
  else
    {
      cmp = memcmp2 (t1, t1len, t2, t2len);
      errno = 0;
    }

  {
    int saved_errno = errno;
    freea (memory);
    errno = saved_errno;
  }

  return cmp;
}
