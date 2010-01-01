/* Fill UTF-8 string.
   Copyright (C) 2002, 2006, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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
#include "unistr.h"

#define FUNC u8_set
#define UNIT uint8_t
#define IS_SINGLE_UNIT(uc) (uc < 0x80)

#include <errno.h>
#include <string.h>

UNIT *
FUNC (UNIT *s, ucs4_t uc, size_t n)
{
  if (n > 0)
    {
      if (IS_SINGLE_UNIT (uc))
        memset ((char *) s, uc, n);
      else
        {
          errno = EILSEQ;
          return NULL;
        }
    }
  return s;
}
