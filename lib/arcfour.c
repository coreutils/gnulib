/* arcfour.c --- The arcfour stream cipher
 * Copyright (C) 2000-2003, 2005-2006, 2009-2022 Free Software Foundation, Inc.
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/* Code from Libgcrypt adapted for gnulib by Simon Josefsson. */

/*
 * For a description of the algorithm, see:
 *   Bruce Schneier: Applied Cryptography. John Wiley & Sons, 1996.
 *   ISBN 0-471-11709-9. Pages 397 ff.
 */

#include <config.h>

#include "arcfour.h"

void
arcfour_stream (arcfour_context * context, const char *inbuf, char *outbuf,
                size_t length)
{
  uint8_t i = context->idx_i;
  uint8_t j = context->idx_j;
  char *sbox = context->sbox;

  for (; length > 0; length--)
    {
      char t;

      i++;
      j += sbox[i];
      t = sbox[i];
      sbox[i] = sbox[j];
      sbox[j] = t;
      *outbuf++ = (*inbuf++
                   ^ sbox[(0U + sbox[i] + sbox[j]) % ARCFOUR_SBOX_SIZE]);
    }

  context->idx_i = i;
  context->idx_j = j;
}

void
arcfour_setkey (arcfour_context * context, const char *key, size_t keylen)
{
  size_t i, j, k;
  char *sbox = context->sbox;

  context->idx_i = context->idx_j = 0;
  for (i = 0; i < ARCFOUR_SBOX_SIZE; i++)
    sbox[i] = i;
  for (i = j = k = 0; i < ARCFOUR_SBOX_SIZE; i++)
    {
      char t;
      j = (j + sbox[i] + key[k]) % ARCFOUR_SBOX_SIZE;
      t = sbox[i];
      sbox[i] = sbox[j];
      sbox[j] = t;
      if (++k == keylen)
        k = 0;
    }
}
