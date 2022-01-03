/* Locale dependent transformation for case insensitive comparison of strings.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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
#include "unicase.h"

#include <errno.h>
#include <stdlib.h>

#include "uniconv.h"

char *
ulc_casexfrm (const char *s, size_t n, const char *iso639_language,
              uninorm_t nf,
              char *resultbuf, size_t *lengthp)
{
  uint8_t convbuf[2048 / sizeof (uint8_t)];
  uint8_t *conv;
  size_t conv_length;
  char *result;

  /* Convert the string to UTF-8.  */
  conv_length = sizeof (convbuf) / sizeof (uint8_t);
  conv =
    u8_conv_from_encoding (locale_charset (), iconveh_error, s, n, NULL,
                           convbuf, &conv_length);
  if (conv == NULL)
    /* errno is set here.  */
    return NULL;

  /* Case-fold and normalize.  */
  result = u8_casexfrm (conv, conv_length, iso639_language, nf,
                        resultbuf, lengthp);
  if (result == NULL)
    {
      if (conv != convbuf)
        {
          int saved_errno = errno;
          free (conv);
          errno = saved_errno;
        }
      return NULL;
    }

  if (conv != convbuf)
    free (conv);
  return result;
}
