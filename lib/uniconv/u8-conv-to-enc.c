/* Conversion from UTF-8 to legacy encodings.
   Copyright (C) 2002, 2006-2007, 2009 Free Software Foundation, Inc.

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

int
u8_conv_to_encoding (const char *tocode,
		     enum iconv_ilseq_handler handler,
		     const uint8_t *src, size_t srclen,
		     size_t *offsets,
		     char **resultp, size_t *lengthp)
{
  char *result;

  if (STRCASEEQ (tocode, "UTF-8", 'U','T','F','-','8',0,0,0,0))
    {
      /* Conversion from UTF-8 to UTF-8.  No need to go through iconv().  */
#if CONFIG_UNICODE_SAFETY
      if (u8_check (src, srclen))
	{
	  errno = EILSEQ;
	  return -1;
	}
#endif

      /* Memory allocation.  */
      if ((*resultp != NULL && *lengthp >= srclen) || srclen == 0)
	result = *resultp;
      else
	{
	  result = (char *) malloc (srclen);
	  if (result == NULL)
	    {
	      errno = ENOMEM;
	      return -1;
	    }
	}

      memcpy (result, (const char *) src, srclen);
      *resultp = result;
      *lengthp = srclen;
      return 0;
    }
  else
    return mem_iconveha ((const char *) src, srclen,
			 "UTF-8", tocode,
			 handler == iconveh_question_mark, handler,
			 offsets, resultp, lengthp);
}
