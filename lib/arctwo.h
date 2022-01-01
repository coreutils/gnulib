/* arctwo.h --- The arctwo block cipher
 * Copyright (C) 2000-2003, 2005, 2009-2022 Free Software Foundation, Inc.
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

#ifndef ARCTWO_H
# define ARCTWO_H

# include <stddef.h>
# include <stdint.h>

typedef struct
{
  uint16_t S[64];
} arctwo_context;

#define ARCTWO_BLOCK_SIZE 8

/* Initialize CONTEXT using KEY of KEYLEN length.  If
   EFFECTIVE_KEYLEN, truncate the key (using a special algorithm) to
   only be of EFFECTIVE_KEYLEN bits.  Normally, you use
   EFFECTIVE_KEYLEN of 0, but see RFC 2268 for more information. */
void
arctwo_setkey_ekb (arctwo_context *context,
                   size_t keylen, const char *key, size_t effective_keylen);

#define arctwo_setkey(context,keylen,key) \
  arctwo_setkey_ekb (context, keylen, key, 8 * (keylen))

/* Encrypt INBUF of size LENGTH into OUTBUF.  LENGTH must be a
   multiple of ARCTWO_BLOCK_SIZE.  CONTEXT hold the encryption key,
   and must have been initialized with arctwo_setkey or
   arctwo_setkey_ekb. */
extern void
arctwo_encrypt (arctwo_context *context, const char *inbuf,
                char *restrict outbuf, size_t length);

/* Decrypt INBUF of size LENGTH into OUTBUF.  LENGTH must be a
   multiple of ARCTWO_BLOCK_SIZE.  CONTEXT hold the decryption key,
   and must have been initialized with arctwo_setkey or
   arctwo_setkey_ekb. */
extern void
arctwo_decrypt (arctwo_context *context, const char *inbuf,
                char *restrict outbuf, size_t length);

#endif /* ARCTWO_H */
