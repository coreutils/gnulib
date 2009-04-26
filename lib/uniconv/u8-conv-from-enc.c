/* Conversion to UTF-8 from legacy encodings.
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
u8_conv_from_encoding (const char *fromcode,
		       enum iconv_ilseq_handler handler,
		       const char *src, size_t srclen,
		       size_t *offsets,
		       uint8_t **resultp, size_t *lengthp)
{
  if (STRCASEEQ (fromcode, "UTF-8", 'U','T','F','-','8',0,0,0,0))
    {
      /* Conversion from UTF-8 to UTF-8.  No need to go through iconv().  */
      uint8_t *result;

      if (u8_check ((const uint8_t *) src, srclen))
	{
	  errno = EILSEQ;
	  return -1;
	}

      if (offsets != NULL)
	{
	  size_t i;

	  for (i = 0; i < srclen; )
	    {
	      int count = u8_mblen ((const uint8_t *) src + i, srclen - i);
	      /* We can rely on count > 0 because of the previous u8_check.  */
	      if (count <= 0)
		abort ();
	      offsets[i] = i;
	      i++;
	      while (--count > 0)
		offsets[i++] = (size_t)(-1);
	    }
	}

      /* Memory allocation.  */
      if ((*resultp != NULL && *lengthp >= srclen) || srclen == 0)
	result = *resultp;
      else
	{
	  result = (uint8_t *) malloc (srclen);
	  if (result == NULL)
	    {
	      errno = ENOMEM;
	      return -1;
	    }
	}

      memcpy ((char *) result, src, srclen);
      *resultp = result;
      *lengthp = srclen;
      return 0;
    }
  else
    return mem_iconveha (src, srclen, fromcode, "UTF-8", true, handler,
			 offsets, (char **) resultp, lengthp);
}
