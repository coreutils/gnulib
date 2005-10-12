/* gc-pbkdf2-sha1.c --- Password-Based Key Derivation Function a'la PKCS#5
   Copyright (C) 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Simon Josefsson.  The comments in this file are taken
   from RFC 2898.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "gc.h"

#include <stdlib.h>
#include <string.h>

/*
 * 5.2 PBKDF2
 *
 *  PBKDF2 applies a pseudorandom function (see Appendix B.1 for an
 *  example) to derive keys. The length of the derived key is essentially
 *  unbounded. (However, the maximum effective search space for the
 *  derived key may be limited by the structure of the underlying
 *  pseudorandom function. See Appendix B.1 for further discussion.)
 *  PBKDF2 is recommended for new applications.
 *
 *  PBKDF2 (P, S, c, dkLen)
 *
 *  Options:        PRF        underlying pseudorandom function (hLen
 *                             denotes the length in octets of the
 *                             pseudorandom function output)
 *
 *  Input:          P          password, an octet string (ASCII or UTF-8)
 *                  S          salt, an octet string
 *                  c          iteration count, a positive integer
 *                  dkLen      intended length in octets of the derived
 *                             key, a positive integer, at most
 *                             (2^32 - 1) * hLen
 *
 *  Output:         DK         derived key, a dkLen-octet string
 */

Gc_rc
gc_pbkdf2_sha1 (const char *P, size_t Plen,
		const char *S, size_t Slen,
		unsigned int c,
		char *DK, size_t dkLen)
{
  unsigned int hLen = 20;
  char U[20];
  char T[20];
  unsigned int u;
  unsigned int l;
  unsigned int r;
  unsigned int i;
  unsigned int k;
  int rc;

  if (c == 0)
    return GC_PKCS5_INVALID_ITERATION_COUNT;

  if (dkLen == 0)
    return GC_PKCS5_INVALID_DERIVED_KEY_LENGTH;

  /*
   *
   *  Steps:
   *
   *     1. If dkLen > (2^32 - 1) * hLen, output "derived key too long" and
   *        stop.
   */

  if (dkLen > 4294967295U)
    return GC_PKCS5_DERIVED_KEY_TOO_LONG;

  /*
   *     2. Let l be the number of hLen-octet blocks in the derived key,
   *        rounding up, and let r be the number of octets in the last
   *        block:
   *
   *                  l = CEIL (dkLen / hLen) ,
   *                  r = dkLen - (l - 1) * hLen .
   *
   *        Here, CEIL (x) is the "ceiling" function, i.e. the smallest
   *        integer greater than, or equal to, x.
   */

  l = dkLen / hLen;
  if (dkLen % hLen)
    l++;
  r = dkLen - (l - 1) * hLen;

  /*
   *     3. For each block of the derived key apply the function F defined
   *        below to the password P, the salt S, the iteration count c, and
   *        the block index to compute the block:
   *
   *                  T_1 = F (P, S, c, 1) ,
   *                  T_2 = F (P, S, c, 2) ,
   *                  ...
   *                  T_l = F (P, S, c, l) ,
   *
   *        where the function F is defined as the exclusive-or sum of the
   *        first c iterates of the underlying pseudorandom function PRF
   *        applied to the password P and the concatenation of the salt S
   *        and the block index i:
   *
   *                  F (P, S, c, i) = U_1 \xor U_2 \xor ... \xor U_c
   *
   *        where
   *
   *                  U_1 = PRF (P, S || INT (i)) ,
   *                  U_2 = PRF (P, U_1) ,
   *                  ...
   *                  U_c = PRF (P, U_{c-1}) .
   *
   *        Here, INT (i) is a four-octet encoding of the integer i, most
   *        significant octet first.
   *
   *     4. Concatenate the blocks and extract the first dkLen octets to
   *        produce a derived key DK:
   *
   *                  DK = T_1 || T_2 ||  ...  || T_l<0..r-1>
   *
   *     5. Output the derived key DK.
   *
   *  Note. The construction of the function F follows a "belt-and-
   *  suspenders" approach. The iterates U_i are computed recursively to
   *  remove a degree of parallelism from an opponent; they are exclusive-
   *  ored together to reduce concerns about the recursion degenerating
   *  into a small set of values.
   *
   */

  for (i = 1; i <= l; i++)
    {
      memset (T, 0, hLen);

      for (u = 1; u <= c; u++)
	{
	  if (u == 1)
	    {
	      char *tmp;
	      size_t tmplen = Slen + 4;

	      tmp = malloc (tmplen);
	      if (tmp == NULL)
		return GC_MALLOC_ERROR;

	      memcpy (tmp, S, Slen);
	      tmp[Slen + 0] = (i & 0xff000000) >> 24;
	      tmp[Slen + 1] = (i & 0x00ff0000) >> 16;
	      tmp[Slen + 2] = (i & 0x0000ff00) >> 8;
	      tmp[Slen + 3] = (i & 0x000000ff) >> 0;

	      rc = gc_hmac_sha1 (P, Plen, tmp, tmplen, U);

	      free (tmp);
	    }
	  else
	    rc = gc_hmac_sha1 (P, Plen, U, hLen, U);

	  if (rc != GC_OK)
	    return rc;

	  for (k = 0; k < hLen; k++)
	    T[k] ^= U[k];
	}

      memcpy (DK + (i - 1) * hLen, T, i == l ? r : hLen);
    }

  return GC_OK;
}
