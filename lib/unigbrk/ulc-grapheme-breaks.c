/* Grapheme cluster breaks function.
   Copyright (C) 2001-2003, 2006-2022 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010,
   based on code written by Bruno Haible <bruno@clisp.org>, 2009.

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

#include <config.h>

/* Specification.  */
#include "unigbrk.h"

#include <stdlib.h>
#include <string.h>

#include "c-ctype.h"
#include "c-strcaseeq.h"
#include "localcharset.h"
#include "uniconv.h"

static int
is_utf8_encoding (const char *encoding)
{
  if (STRCASEEQ (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0, 0))
    return 1;
  return 0;
}

#if C_CTYPE_ASCII
/* Assume that every ASCII character starts a new grapheme, which is often
   true, except that CR-LF is a single grapheme. */
static void
ascii_grapheme_breaks (const char *s, size_t n, char *p)
{
  size_t i;

  p[0] = 1;
  for (i = 1; i < n; i++)
    {
      bool is_ascii = c_isprint (s[i]) || c_isspace (s[i]);
      p[i] = is_ascii && (s[i] != '\n' || s[i - 1] != '\r');
    }
}
#endif

/* Grapheme boundaries in a string in an arbitrary encoding.

   We convert the input string to Unicode.

   The standardized Unicode encodings are UTF-8, UCS-2, UCS-4, UTF-16,
   UTF-16BE, UTF-16LE, UTF-7.  UCS-2 supports only characters up to
   \U0000FFFF.  UTF-16 and variants support only characters up to
   \U0010FFFF.  UTF-7 is way too complex and not supported by glibc-2.1.
   UCS-4 specification leaves doubts about endianness and byte order mark.
   glibc currently interprets it as big endian without byte order mark,
   but this is not backed by an RFC.  So we use UTF-8. It supports
   characters up to \U7FFFFFFF and is unambiguously defined.  */

void
ulc_grapheme_breaks (const char *s, size_t n, char *p)
{
  if (n > 0)
    {
      const char *encoding = locale_charset ();

      if (is_utf8_encoding (encoding))
        u8_grapheme_breaks ((const uint8_t *) s, n, p);
      else
        {
          /* Convert the string to UTF-8 and build a translation table
             from offsets into s to offsets into the translated string.  */
          size_t *offsets = (size_t *) malloc (n * sizeof (size_t));

          if (offsets != NULL)
            {
              uint8_t *t;
              size_t m;

              t = u8_conv_from_encoding (encoding, iconveh_question_mark,
                                         s, n, offsets, NULL, &m);
              if (t != NULL)
                {
                  char *q = (char *) (m > 0 ? malloc (m) : NULL);

                  if (m == 0 || q != NULL)
                    {
                      size_t i;

                      /* Determine the grapheme breaks of the UTF-8 string.  */
                      u8_grapheme_breaks (t, m, q);

                      /* Translate the result back to the original string.  */
                      memset (p, 0, n);
                      for (i = 0; i < n; i++)
                        if (offsets[i] != (size_t)(-1))
                          p[i] = q[offsets[i]];

                      free (q);
                      free (t);
                      free (offsets);
                      return;
                    }
                  free (t);
                }
              free (offsets);
            }

          /* Impossible to convert. */
#if C_CTYPE_ASCII
          /* Fall back to ASCII as best we can. */
          ascii_grapheme_breaks (s, n, p);
#else
          /* We cannot make any assumptions. */
          p[0] = 1;
          memset (p + 1, 0, n - 1);
#endif
        }
    }
}
