/* Conversion UTF-16 to UCS-4.
   Copyright (C) 2001-2002, 2006-2007 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2001.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

#include <config.h>

/* Specification.  */
#include "utf16-ucs4.h"

int
u16_mbtouc_aux (ucs4_t *puc, const uint16_t *s, size_t n)
{
  uint16_t c = *s;

#if CONFIG_UNICODE_SAFETY
  if (c < 0xdc00)
#endif
    {
      if (n >= 2)
	{
#if CONFIG_UNICODE_SAFETY
	  if (s[1] >= 0xdc00 && s[1] < 0xe000)
#endif
	    {
	      *puc = 0x10000 + ((c - 0xd800) << 10) + (s[1] - 0xdc00);
	      return 2;
	    }
	  /* invalid multibyte character */
	}
      else
	{
	  /* incomplete multibyte character */
	}
    }
  /* invalid multibyte character */
  *puc = 0xfffd;
  return 1;
}
