/* Copyright (C) 1991, 1993, 1996, 1997, 1999, 2000, 2003, 2004, 2006,
   2008 Free Software Foundation, Inc.

   Based on strlen implementation by Torbjorn Granlund (tege@sics.se),
   with help from Dan Sahlin (dan@sics.se) and
   commentary by Jim Blandy (jimb@ai.mit.edu);
   adaptation to memchr suggested by Dick Karpinski (dick@cca.ucsf.edu),
   and implemented in glibc by Roland McGrath (roland@ai.mit.edu).
   Extension to memchr2 implemented by Eric Blake (ebb9@byu.net).

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include "memchr2.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "intprops.h"

/* Return the first address of either C1 or C2 (treated as unsigned
   char) that occurs within N bytes of the memory region S.  If
   neither byte appears, return NULL.  */
void *
memchr2 (void const *s, int c1_in, int c2_in, size_t n)
{
  /* On 32-bit hardware, choosing longword to be a 32-bit unsigned
     long instead of a 64-bit uintmax_t tends to give better
     performance.  On 64-bit hardware, unsigned long is generally 64
     bits already.  Change this typedef to experiment with
     performance.  */
  typedef unsigned long longword;

  const unsigned char *char_ptr;
  const longword *longword_ptr;
  longword longword1;
  longword longword2;
  longword magic_bits;
  longword charmask1;
  longword charmask2;
  unsigned char c1;
  unsigned char c2;
  int i;

  c1 = (unsigned char) c1_in;
  c2 = (unsigned char) c2_in;

  if (c1 == c2)
    return memchr (s, c1, n);

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       n > 0 && (size_t) char_ptr % sizeof longword1 != 0;
       --n, ++char_ptr)
    if (*char_ptr == c1 || *char_ptr == c2)
      return (void *) char_ptr;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to any size longwords.  */

  longword_ptr = (const longword *) char_ptr;
  magic_bits = 0x01010101;
  charmask1 = c1 | (c1 << 8);
  charmask2 = c2 | (c2 << 8);
  charmask1 |= charmask1 << 16;
  charmask2 |= charmask2 << 16;
  if (0xffffffffU < TYPE_MAXIMUM (longword))
    {
      magic_bits |= magic_bits << 31 << 1;
      charmask1 |= charmask1 << 31 << 1;
      charmask2 |= charmask2 << 31 << 1;
      if (8 < sizeof longword1)
	for (i = 64; i < sizeof longword1 * 8; i *= 2)
	  {
	    magic_bits |= magic_bits << i;
	    charmask1 |= charmask1 << i;
	    charmask2 |= charmask2 << i;
	  }
    }

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.

     We first use an xor to convert target bytes into a NUL byte,
     since the test for a zero byte is more efficient.  For all byte
     values except 0x00 and 0x80, subtracting 1 from the byte will
     leave the most significant bit unchanged.  So detecting 0 is
     simply a matter of subtracting from all bytes in parallel, and
     checking for a most significant bit that changed to 1.  */

  while (n >= sizeof longword1)
    {
      longword1 = *longword_ptr ^ charmask1;
      longword2 = *longword_ptr ^ charmask2;

      if (((((longword1 - magic_bits) & ~longword1)
	    | ((longword2 - magic_bits) & ~longword2))
	   & (magic_bits << 7)) != 0)
	break;
      longword_ptr++;
      n -= sizeof longword1;
    }

  char_ptr = (const unsigned char *) longword_ptr;

  while (n-- > 0)
    {
      if (*char_ptr == c1 || *char_ptr == c2)
	return (void *) char_ptr;
      ++char_ptr;
    }

  return NULL;
}
