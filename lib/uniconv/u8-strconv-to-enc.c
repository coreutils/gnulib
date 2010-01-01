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
u8_strconv_to_encoding (const uint8_t *string,
                        const char *tocode,
                        enum iconv_ilseq_handler handler)
{
  char *result;
  size_t length;

  if (STRCASEEQ (tocode, "UTF-8", 'U','T','F','-','8',0,0,0,0))
    {
      /* Conversion from UTF-8 to UTF-8.  No need to go through iconv().  */
      length = u8_strlen (string) + 1;
#if CONFIG_UNICODE_SAFETY
      if (u8_check (string, length))
        {
          errno = EILSEQ;
          return NULL;
        }
#endif
      result = (char *) malloc (length);
      if (result == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
      memcpy (result, (const char *) string, length);
      return result;
    }
  else
    {
      result = NULL;
      length = 0;
      if (mem_iconveha ((const char *) string, u8_strlen (string) + 1,
                        "UTF-8", tocode,
                        handler == iconveh_question_mark, handler,
                        NULL, &result, &length) < 0)
        return NULL;
      /* Verify the result has exactly one NUL byte, at the end.  */
      if (!(length > 0 && result[length-1] == '\0'
            && strlen (result) == length-1))
        {
          free (result);
          errno = EILSEQ;
          return NULL;
        }
      return result;
    }
}
