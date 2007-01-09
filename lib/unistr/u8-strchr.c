/* Search character in UTF-8 string.
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
u8_strchr (const uint8_t *s, ucs4_t uc)
{
  uint8_t c[6];

  if (uc < 0x80)
    {
      uint8_t c0 = uc;

      for (;; s++)
	{
	  if (*s == c0)
	    break;
	  if (*s == 0)
	    goto notfound;
	}
      return (uint8_t *) s;
    }
  else
    switch (u8_uctomb_aux (c, uc, 6))
      {
      case 2:
	if (*s == 0)
	  goto notfound;
	{
	  uint8_t c0 = c[0];
	  uint8_t c1 = c[1];

	  for (;; s++)
	    {
	      if (s[1] == 0)
		goto notfound;
	      if (*s == c0 && s[1] == c1)
		break;
	    }
	  return (uint8_t *) s;
	}

      case 3:
	if (*s == 0 || s[1] == 0)
	  goto notfound;
	{
	  uint8_t c0 = c[0];
	  uint8_t c1 = c[1];
	  uint8_t c2 = c[2];

	  for (;; s++)
	    {
	      if (s[2] == 0)
		goto notfound;
	      if (*s == c0 && s[1] == c1 && s[2] == c2)
		break;
	    }
	  return (uint8_t *) s;
	}

      case 4:
	if (*s == 0 || s[1] == 0 || s[2] == 0)
	  goto notfound;
	{
	  uint8_t c0 = c[0];
	  uint8_t c1 = c[1];
	  uint8_t c2 = c[2];
	  uint8_t c3 = c[3];

	  for (;; s++)
	    {
	      if (s[3] == 0)
		goto notfound;
	      if (*s == c0 && s[1] == c1 && s[2] == c2 && s[3] == c3)
		break;
	    }
	  return (uint8_t *) s;
	}
      }
notfound:
  return NULL;
}
