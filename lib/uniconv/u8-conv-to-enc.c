/* Conversion from UTF-8 to legacy encodings.
   Copyright (C) 2002, 2006-2007, 2009-2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>.  */

#include <config.h>

/* Specification.  */
#include "uniconv.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "c-strcaseeq.h"
#include "striconveha.h"
#include "unistr.h"

char *
u8_conv_to_encoding (const char *tocode,
                     enum iconv_ilseq_handler handler,
                     const uint8_t *src, size_t srclen,
                     size_t *offsets,
                     char *resultbuf, size_t *lengthp)
{
  if (STRCASEEQ (tocode, "UTF-8", 'U','T','F','-','8',0,0,0,0))
    {
      char *result;

      /* Conversion from UTF-8 to UTF-8.  No need to go through iconv().  */
#if CONFIG_UNICODE_SAFETY
      if (u8_check (src, srclen))
        {
          errno = EILSEQ;
          return NULL;
        }
#endif

      /* Memory allocation.  */
      if (resultbuf != NULL && *lengthp >= srclen)
        result = resultbuf;
      else
        {
          result = (char *) malloc (srclen > 0 ? srclen : 1);
          if (result == NULL)
            {
              errno = ENOMEM;
              return NULL;
            }
        }

      memcpy (result, (const char *) src, srclen);
      *lengthp = srclen;
      return result;
    }
  else
    {
      char *result = resultbuf;
      size_t length = *lengthp;

      if (mem_iconveha ((const char *) src, srclen,
                        "UTF-8", tocode,
                        handler == iconveh_question_mark, handler,
                        offsets, &result, &length) < 0)
        return NULL;

      if (result == NULL) /* when (resultbuf == NULL && length == 0)  */
        {
          result = (char *) malloc (1);
          if (result == NULL)
            {
              errno = ENOMEM;
              return NULL;
            }
        }
      *lengthp = length;
      return result;
    }
}
