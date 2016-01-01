/* hmac-sha256.c -- hashed message authentication codes
   Copyright (C) 2005-2006, 2009-2016 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson.  */

#include <config.h>

#include "hmac.h"

#include "memxor.h"
#include "sha256.h"

#include <string.h>

#define IPAD 0x36
#define OPAD 0x5c

int
hmac_sha256 (const void *key, size_t keylen,
             const void *in, size_t inlen, void *resbuf)
{
  struct sha256_ctx inner;
  struct sha256_ctx outer;
  char optkeybuf[32];
  char block[64];
  char innerhash[32];

  /* Reduce the key's size, so that it becomes <= 64 bytes large.  */

  if (keylen > 64)
    {
      struct sha256_ctx keyhash;

      sha256_init_ctx (&keyhash);
      sha256_process_bytes (key, keylen, &keyhash);
      sha256_finish_ctx (&keyhash, optkeybuf);

      key = optkeybuf;
      keylen = 32;
    }

  /* Compute INNERHASH from KEY and IN.  */

  sha256_init_ctx (&inner);

  memset (block, IPAD, sizeof (block));
  memxor (block, key, keylen);

  sha256_process_block (block, 64, &inner);
  sha256_process_bytes (in, inlen, &inner);

  sha256_finish_ctx (&inner, innerhash);

  /* Compute result from KEY and INNERHASH.  */

  sha256_init_ctx (&outer);

  memset (block, OPAD, sizeof (block));
  memxor (block, key, keylen);

  sha256_process_block (block, 64, &outer);
  sha256_process_bytes (innerhash, 32, &outer);

  sha256_finish_ctx (&outer, resbuf);

  return 0;
}
