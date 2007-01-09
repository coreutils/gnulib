/* Search character in piece of UTF-8 string.
   Copyright (C) 1999, 2002, 2006 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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
#include "unistr.h"

#include "ucs4-utf8.h"

uint8_t *
u8_chr (const uint8_t *s, size_t n, ucs4_t uc)
{
  uint8_t c[6];

  if (uc < 0x80)
    {
      uint8_t c0 = uc;

      for (; n > 0; s++, n--)
	{
	  if (*s == c0)
	    return (uint8_t *) s;
	}
    }
  else
    switch (u8_uctomb_aux (c, uc, 6))
      {
      case 2:
	if (n > 1)
	  {
	    uint8_t c0 = c[0];
	    uint8_t c1 = c[1];

	    for (n--; n > 0; s++, n--)
	      {
		if (*s == c0 && s[1] == c1)
		  return (uint8_t *) s;
	      }
	  }
	break;

      case 3:
	if (n > 2)
	  {
	    uint8_t c0 = c[0];
	    uint8_t c1 = c[1];
	    uint8_t c2 = c[2];

	    for (n -= 2; n > 0; s++, n--)
	      {
		if (*s == c0 && s[1] == c1 && s[2] == c2)
		  return (uint8_t *) s;
	      }
	  }
	break;

      case 4:
	if (n > 3)
	  {
	    uint8_t c0 = c[0];
	    uint8_t c1 = c[1];
	    uint8_t c2 = c[2];
	    uint8_t c3 = c[3];

	    for (n -= 3; n > 0; s++, n--)
	      {
		if (*s == c0 && s[1] == c1 && s[2] == c2 && s[3] == c3)
		  return (uint8_t *) s;
	      }
	  }
	break;
      }
  return NULL;
}
