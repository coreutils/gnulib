/* Conversion UTF-8 to UCS-4.
   Copyright (C) 2001-2002, 2005-2007 Free Software Foundation, Inc.
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

#ifndef _UTF8_UCS4_H
#define _UTF8_UCS4_H

#include <stddef.h>
#include "unitypes.h"

extern int u8_mbtouc_aux (ucs4_t *puc, const uint8_t *s, size_t n);

/* Return the length (number of units) of the first character in S, putting
   its 'ucs4_t' representation in *PUC.
   The number of available units, N, must be > 0.  */
static inline int
u8_mbtouc (ucs4_t *puc, const uint8_t *s, size_t n)
{
  uint8_t c = *s;

  if (c < 0x80)
    {
      *puc = c;
      return 1;
    }
  else
    return u8_mbtouc_aux (puc, s, n);
}

#endif /* _UTF8_UCS4_H */
