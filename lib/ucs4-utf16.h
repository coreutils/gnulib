/* Conversion UCS-4 to UTF-16.
   Copyright (C) 2002, 2005-2007 Free Software Foundation, Inc.
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

#ifndef _UCS4_UTF16_H
#define _UCS4_UTF16_H

#include <stddef.h>
#include "unitypes.h"

extern int u16_uctomb_aux (uint16_t *s, ucs4_t uc, int n);

/* Return the length (number of units) of the UTF-16 representation of uc,
   after storing it at S.  Return -1 upon failure, -2 if the number of
   available units, N, is too small.  */
static inline int
u16_uctomb (uint16_t *s, ucs4_t uc, int n)
{
  if (uc < 0xd800 && n > 0)
    {
      s[0] = uc;
      return 1;
    }
  else
    return u16_uctomb_aux (s, uc, n);
}

#endif /* _UCS4_UTF16_H */
