/* Copy UTF-8/UTF-16/UTF-32 string.
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

#include <stdlib.h>
#include <string.h>

UNIT *
FUNC (const UNIT *s)
{
  size_t n = U_STRLEN (s) + 1;
  UNIT *dest;

  dest = (UNIT *) malloc (n * sizeof (UNIT));
  if (dest != NULL)
    {
#if 0
      UNIT *destptr = dest;

      for (; n > 0; n--)
	*destptr++ = *s++;
#else
      memcpy ((char *) dest, (const char *) s, n * sizeof (UNIT));
#endif
    }
  return dest;
}
