/* Check UTF-16 string.
   Copyright (C) 2002, 2006 Free Software Foundation, Inc.
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

const uint16_t *
u16_check (const uint16_t *s, size_t n)
{
  const uint16_t *s_end = s + n;

  while (s < s_end)
    {
      /* Keep in sync with utf16-ucs4.h and utf16-ucs4.c.  */
      uint16_t c = *s;

      if (c < 0xd800 || c >= 0xe000)
	{
	  s++;
	  continue;
	}
      if (c < 0xdc00)
	{
	  if (s + 2 <= s_end
	      && s[1] >= 0xdc00 && s[1] < 0xe000)
	    {
	      s += 2;
	      continue;
	    }
	}
      /* invalid or incomplete multibyte character */
      return s;
    }
  return NULL;
}
