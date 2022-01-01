/* arctwo.c --- The RC2 cipher as described in RFC 2268.
 * Copyright (C) 2003-2006, 2008-2022 Free Software Foundation, Inc.
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
 */

/* Code from GnuTLS/Libgcrypt adapted for gnulib by Simon Josefsson. */

/* This implementation was written by Nikos Mavroyanopoulos for GNUTLS
 * as a Libgcrypt module (gnutls/lib/x509/rc2.c) and later adapted for
 * direct use by Libgcrypt by Werner Koch.  This implementation is
 * only useful for pkcs#12 decryption.
 *
 * The implementation here is based on Peter Gutmann's RRC.2 paper.
 */

#include <config.h>

#include "arctwo.h"

#include "bitrotate.h"

static const uint8_t arctwo_sbox[] = {
  217, 120, 249, 196, 25, 221, 181, 237,
  40, 233, 253, 121, 74, 160, 216, 157,
  198, 126, 55, 131, 43, 118, 83, 142,
  98, 76, 100, 136, 68, 139, 251, 162,
  23, 154, 89, 245, 135, 179, 79, 19,
  97, 69, 109, 141, 9, 129, 125, 50,
  189, 143, 64, 235, 134, 183, 123, 11,
  240, 149, 33, 34, 92, 107, 78, 130,
  84, 214, 101, 147, 206, 96, 178, 28,
  115, 86, 192, 20, 167, 140, 241, 220,
  18, 117, 202, 31, 59, 190, 228, 209,
  66, 61, 212, 48, 163, 60, 182, 38,
  111, 191, 14, 218, 70, 105, 7, 87,
  39, 242, 29, 155, 188, 148, 67, 3,
  248, 17, 199, 246, 144, 239, 62, 231,
  6, 195, 213, 47, 200, 102, 30, 215,
  8, 232, 234, 222, 128, 82, 238, 247,
  132, 170, 114, 172, 53, 77, 106, 42,
  150, 26, 210, 113, 90, 21, 73, 116,
  75, 159, 208, 94, 4, 24, 164, 236,
  194, 224, 65, 110, 15, 81, 203, 204,
  36, 145, 175, 80, 161, 244, 112, 57,
  153, 124, 58, 133, 35, 184, 180, 122,
  252, 2, 54, 91, 37, 85, 151, 49,
  45, 93, 250, 152, 227, 138, 146, 174,
  5, 223, 41, 16, 103, 108, 186, 201,
  211, 0, 230, 207, 225, 158, 168, 44,
  99, 22, 1, 63, 88, 226, 137, 169,
  13, 56, 52, 27, 171, 51, 255, 176,
  187, 72, 12, 95, 185, 177, 205, 46,
  197, 243, 219, 71, 229, 165, 156, 119,
  10, 166, 32, 104, 254, 127, 193, 173
};

/* C89 compliant way to cast 'char' to 'unsigned char'. */
static unsigned char
to_uchar (char ch)
{
  return ch;
}

void
arctwo_encrypt (arctwo_context *context, const char *inbuf,
                char *outbuf, size_t length)
{
  for (; length >= ARCTWO_BLOCK_SIZE; length -= ARCTWO_BLOCK_SIZE,
         inbuf += ARCTWO_BLOCK_SIZE, outbuf += ARCTWO_BLOCK_SIZE)
    {
      size_t i, j;
      uint16_t word0 = 0, word1 = 0, word2 = 0, word3 = 0;

      word0 = (word0 << 8) | to_uchar (inbuf[1]);
      word0 = (word0 << 8) | to_uchar (inbuf[0]);
      word1 = (word1 << 8) | to_uchar (inbuf[3]);
      word1 = (word1 << 8) | to_uchar (inbuf[2]);
      word2 = (word2 << 8) | to_uchar (inbuf[5]);
      word2 = (word2 << 8) | to_uchar (inbuf[4]);
      word3 = (word3 << 8) | to_uchar (inbuf[7]);
      word3 = (word3 << 8) | to_uchar (inbuf[6]);

      for (i = 0; i < 16; i++)
        {
          j = i * 4;
          /* For some reason I cannot combine those steps. */
          word0 += (word1 & ~word3) + (word2 & word3) + context->S[j];
          word0 = rotl16 (word0, 1);

          word1 += (word2 & ~word0) + (word3 & word0) + context->S[j + 1];
          word1 = rotl16 (word1, 2);

          word2 += (word3 & ~word1) + (word0 & word1) + context->S[j + 2];
          word2 = rotl16 (word2, 3);

          word3 += (word0 & ~word2) + (word1 & word2) + context->S[j + 3];
          word3 = rotl16 (word3, 5);

          if (i == 4 || i == 10)
            {
              word0 += context->S[word3 & 63];
              word1 += context->S[word0 & 63];
              word2 += context->S[word1 & 63];
              word3 += context->S[word2 & 63];
            }
        }

      outbuf[0] = word0 & 255;
      outbuf[1] = word0 >> 8;
      outbuf[2] = word1 & 255;
      outbuf[3] = word1 >> 8;
      outbuf[4] = word2 & 255;
      outbuf[5] = word2 >> 8;
      outbuf[6] = word3 & 255;
      outbuf[7] = word3 >> 8;
    }
}

void
arctwo_decrypt (arctwo_context *context, const char *inbuf,
                char *outbuf, size_t length)
{
  for (; length >= ARCTWO_BLOCK_SIZE; length -= ARCTWO_BLOCK_SIZE,
         inbuf += ARCTWO_BLOCK_SIZE, outbuf += ARCTWO_BLOCK_SIZE)
    {
      size_t i, j;
      uint16_t word0 = 0, word1 = 0, word2 = 0, word3 = 0;

      word0 = (word0 << 8) | to_uchar (inbuf[1]);
      word0 = (word0 << 8) | to_uchar (inbuf[0]);
      word1 = (word1 << 8) | to_uchar (inbuf[3]);
      word1 = (word1 << 8) | to_uchar (inbuf[2]);
      word2 = (word2 << 8) | to_uchar (inbuf[5]);
      word2 = (word2 << 8) | to_uchar (inbuf[4]);
      word3 = (word3 << 8) | to_uchar (inbuf[7]);
      word3 = (word3 << 8) | to_uchar (inbuf[6]);

      for (i = 16; i > 0; i--)
        {
          j = (i - 1) * 4;

          word3 = rotr16 (word3, 5);
          word3 -= (word0 & ~word2) + (word1 & word2) + context->S[j + 3];

          word2 = rotr16 (word2, 3);
          word2 -= (word3 & ~word1) + (word0 & word1) + context->S[j + 2];

          word1 = rotr16 (word1, 2);
          word1 -= (word2 & ~word0) + (word3 & word0) + context->S[j + 1];

          word0 = rotr16 (word0, 1);
          word0 -= (word1 & ~word3) + (word2 & word3) + context->S[j];

          if (i == 6 || i == 12)
            {
              word3 = word3 - context->S[word2 & 63];
              word2 = word2 - context->S[word1 & 63];
          word1 = word1 - context->S[word0 & 63];
          word0 = word0 - context->S[word3 & 63];
            }
        }

      outbuf[0] = word0 & 255;
      outbuf[1] = word0 >> 8;
      outbuf[2] = word1 & 255;
      outbuf[3] = word1 >> 8;
      outbuf[4] = word2 & 255;
      outbuf[5] = word2 >> 8;
      outbuf[6] = word3 & 255;
      outbuf[7] = word3 >> 8;
    }
}

void
arctwo_setkey_ekb (arctwo_context *context,
                   size_t keylen, const char *key, size_t effective_keylen)
{
  size_t i;
  uint8_t *S, x;

  if (keylen < 40 / 8 || effective_keylen > 1024)
    return;

  S = (uint8_t *) context->S;

  for (i = 0; i < keylen; i++)
    S[i] = (uint8_t) key[i];

  for (i = keylen; i < 128; i++)
    S[i] = arctwo_sbox[(S[i - keylen] + S[i - 1]) & 255];

  S[0] = arctwo_sbox[S[0]];

  /* Phase 2 - reduce effective key size to "bits". This was not
   * discussed in Gutmann's paper. I've copied that from the public
   * domain code posted in sci.crypt. */
  if (effective_keylen)
    {
      size_t len = (effective_keylen + 7) >> 3;
      i = 128 - len;
      x = arctwo_sbox[S[i] & (255 >> (7 & -effective_keylen))];
      S[i] = x;

      while (i--)
        {
          x = arctwo_sbox[x ^ S[i + len]];
          S[i] = x;
        }
    }

  /* Make the expanded key, endian independent. */
  for (i = 0; i < 64; i++)
    context->S[i] = ((uint16_t) S[i * 2] | (((uint16_t) S[i * 2 + 1]) << 8));
}
