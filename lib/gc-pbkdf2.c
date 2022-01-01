/* gc-pbkdf2.c --- Password-Based Key Derivation Function a'la PKCS#5
   Copyright (C) 2002-2006, 2009-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson. */

#include <config.h>

#include "gc.h"

#include <stdlib.h>
#include <string.h>

typedef Gc_rc (*gc_prf_func) (const void *key, size_t keylen,
                              const void *in, size_t inlen, char *resbuf);

static Gc_rc
gc_pbkdf2_prf (gc_prf_func prf, size_t hLen,
               const char *P, size_t Plen,
               const char *S, size_t Slen,
               unsigned int c,
               char *DK, size_t dkLen)
{
  char U[GC_MAX_DIGEST_SIZE];
  char T[GC_MAX_DIGEST_SIZE];
  unsigned int u;
  unsigned int l;
  unsigned int r;
  unsigned int i;
  unsigned int k;
  int rc;
  char *tmp;
  size_t tmplen = Slen + 4;

  if (c == 0)
    return GC_PKCS5_INVALID_ITERATION_COUNT;

  if (dkLen == 0)
    return GC_PKCS5_INVALID_DERIVED_KEY_LENGTH;

  if (dkLen > 4294967295U)
    return GC_PKCS5_DERIVED_KEY_TOO_LONG;

  l = ((dkLen - 1) / hLen) + 1;
  r = dkLen - (l - 1) * hLen;

  tmp = malloc (tmplen);
  if (tmp == NULL)
    return GC_MALLOC_ERROR;

  memcpy (tmp, S, Slen);

  for (i = 1; i <= l; i++)
    {
      memset (T, 0, hLen);

      for (u = 1; u <= c; u++)
        {
          if (u == 1)
            {
              tmp[Slen + 0] = (i & 0xff000000) >> 24;
              tmp[Slen + 1] = (i & 0x00ff0000) >> 16;
              tmp[Slen + 2] = (i & 0x0000ff00) >> 8;
              tmp[Slen + 3] = (i & 0x000000ff) >> 0;

              rc = prf (P, Plen, tmp, tmplen, U);
            }
          else
            rc = prf (P, Plen, U, hLen, U);

          if (rc != GC_OK)
            {
              free (tmp);
              return rc;
            }

          for (k = 0; k < hLen; k++)
            T[k] ^= U[k];
        }

      memcpy (DK + (i - 1) * hLen, T, i == l ? r : hLen);
    }

  free (tmp);

  return GC_OK;
}

Gc_rc
gc_pbkdf2_hmac (Gc_hash hash,
                const char *P, size_t Plen,
                const char *S, size_t Slen,
                unsigned int c, char *DK, size_t dkLen)
{
  gc_prf_func prf;
  size_t hLen;

  switch (hash)
    {
#if GNULIB_GC_HMAC_SHA1
    case GC_SHA1:
      prf = gc_hmac_sha1;
      hLen = GC_SHA1_DIGEST_SIZE;
      break;
#endif

#if GNULIB_GC_HMAC_SHA256
    case GC_SHA256:
      prf = gc_hmac_sha256;
      hLen = GC_SHA256_DIGEST_SIZE;
      break;
#endif

#if GNULIB_GC_HMAC_SHA512
    case GC_SHA512:
      prf = gc_hmac_sha512;
      hLen = GC_SHA512_DIGEST_SIZE;
      break;
#endif

    default:
      return GC_INVALID_HASH;
    }

  return gc_pbkdf2_prf (prf, hLen, P, Plen, S, Slen, c, DK, dkLen);
}
