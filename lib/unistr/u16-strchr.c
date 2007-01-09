/* Search character in UTF-16 string.
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

#include "ucs4-utf16.h"

uint16_t *
u16_strchr (const uint16_t *s, ucs4_t uc)
{
  uint16_t c[2];

  if (uc < 0x10000)
    {
      uint16_t c0 = uc;

      for (;; s++)
	{
	  if (*s == c0)
	    break;
	  if (*s == 0)
	    goto notfound;
	}
      return (uint16_t *) s;
    }
  else
    switch (u16_uctomb_aux (c, uc, 2))
      {
      case 2:
	if (*s == 0)
	  goto notfound;
	{
	  uint16_t c0 = c[0];
	  uint16_t c1 = c[1];

	  for (;; s++)
	    {
	      if (s[1] == 0)
		goto notfound;
	      if (*s == c0 && s[1] == c1)
		break;
	    }
	  return (uint16_t *) s;
	}
      }
notfound:
  return NULL;
}
