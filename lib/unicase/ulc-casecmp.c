/* Case and normalization insensitive comparison of strings.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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
#include "unicase.h"

#include <errno.h>
#include <stdlib.h>

#include "minmax.h"
#include "uninorm.h"
#include "uniconv.h"
#include "unistr.h"

static uint8_t *
ulc_u8_casefold (const char *s, size_t n, const char *iso639_language,
                 uninorm_t nf,
                 uint8_t *resultbuf, size_t *lengthp)
{
  uint8_t convbuf[2048 / sizeof (uint8_t)];
  uint8_t *conv;
  size_t conv_length;
  uint8_t *result;

  /* Convert the string to UTF-8.  */
  conv_length = sizeof (convbuf) / sizeof (uint8_t);
  conv =
    u8_conv_from_encoding (locale_charset (), iconveh_error, s, n, NULL,
                           convbuf, &conv_length);
  if (conv == NULL)
    /* errno is set here.  */
    return NULL;

  /* Case-fold and normalize.  */
  result = u8_casefold (conv, conv_length, iso639_language, nf,
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

#define FUNC ulc_casecmp
#define UNIT uint8_t
#define SRC_UNIT char
#define U_CASEFOLD ulc_u8_casefold
#define U_CMP2 u8_cmp2
#include "u-casecmp.h"
