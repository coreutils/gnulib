/* Search for some characters in UTF-32 string.
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

#define FUNC u32_strcspn
#define UNIT uint32_t
#define U_STRLEN u32_strlen
#define U_STRCHR u32_strchr

size_t
FUNC (const UNIT *str, const UNIT *reject)
{
  /* Optimize two cases.  */
  if (reject[0] == 0)
    return U_STRLEN (str);
  if (reject[1] == 0)
    {
      ucs4_t uc = reject[0];
      const UNIT *ptr = str;
      for (; *ptr != 0; ptr++)
	if (*ptr == uc)
	  break;
      return ptr - str;
    }
  /* General case.  */
  {
    const UNIT *ptr = str;
    for (; *ptr != 0; ptr++)
      if (U_STRCHR (reject, *ptr))
	break;
    return ptr - str;
  }
}
