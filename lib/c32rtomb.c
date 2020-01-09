/* Convert 32-bit wide character to multibyte character.
   Copyright (C) 2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <uchar.h>

#include <errno.h>
#include <wchar.h>

#include "localcharset.h"
#include "streq.h"

#ifndef FALLTHROUGH
# if __GNUC__ < 7
#  define FALLTHROUGH ((void) 0)
# else
#  define FALLTHROUGH __attribute__ ((__fallthrough__))
# endif
#endif

size_t
c32rtomb (char *s, char32_t wc, mbstate_t *ps)
#undef c32rtomb
{
#if HAVE_WORKING_MBRTOC32

# if C32RTOMB_RETVAL_BUG
  if (s == NULL)
    /* We know the NUL wide character corresponds to the NUL character.  */
    return 1;
# endif

  return c32rtomb (s, wc, ps);

#elif _GL_LARGE_CHAR32_T

  if (s == NULL)
    return wcrtomb (NULL, 0, ps);
  else
    {
      /* Special-case all encodings that may produce wide character values
         > WCHAR_MAX.  */
      const char *encoding = locale_charset ();
      if (STREQ_OPT (encoding, "UTF-8", 'U', 'T', 'F', '-', '8', 0, 0, 0, 0))
        {
          /* Special-case the UTF-8 encoding.  Assume that the wide-character
             encoding in a UTF-8 locale is UCS-2 or, equivalently, UTF-16.  */
          if (wc < 0x80)
            {
              s[0] = (unsigned char) wc;
              return 1;
            }
          else
            {
              int count;

              if (wc < 0x800)
                count = 2;
              else if (wc < 0x10000)
                {
                  if (wc < 0xd800 || wc >= 0xe000)
                    count = 3;
                  else
                    {
                      errno = EILSEQ;
                      return (size_t)(-1);
                    }
                }
              else if (wc < 0x110000)
                count = 4;
              else
                {
                  errno = EILSEQ;
                  return (size_t)(-1);
                }

              switch (count) /* note: code falls through cases! */
                {
                case 4: s[3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
                  FALLTHROUGH;
                case 3: s[2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
                  FALLTHROUGH;
                case 2: s[1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
              /*case 1:*/ s[0] = wc;
                }
              return count;
            }
        }
      else
        {
          if ((wchar_t) wc == wc)
            return wcrtomb (s, (wchar_t) wc, ps);
          else
            {
              errno = EILSEQ;
              return (size_t)(-1);
            }
        }
    }

#else

  /* char32_t and wchar_t are equivalent.  */
  return wcrtomb (s, (wchar_t) wc, ps);

#endif
}
