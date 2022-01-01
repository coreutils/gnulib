/* des.h --- DES cipher implementation.
 * Copyright (C) 2005, 2007, 2009-2022 Free Software Foundation, Inc.
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

/* Adapted for gnulib by Simon Josefsson, based on Libgcrypt. */

#ifndef DES_H
# define DES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Encryption/Decryption context of DES
 */
typedef struct
{
  uint32_t encrypt_subkeys[32];
  uint32_t decrypt_subkeys[32];
} gl_des_ctx;

/*
 * Encryption/Decryption context of Triple-DES
 */
typedef struct
{
  uint32_t encrypt_subkeys[96];
  uint32_t decrypt_subkeys[96];
} gl_3des_ctx;

/* Check whether the 8 byte key is weak.  Does not check the parity
 * bits of the key but simple ignore them. */
extern bool
gl_des_is_weak_key (const char * key);

/*
 * DES
 * ---
 */

/* Fill a DES context CTX with subkeys calculated from 64bit KEY.
 * Does not check parity bits, but simply ignore them.  Does not check
 * for weak keys. */
extern void
gl_des_setkey (gl_des_ctx *ctx, const char * key);

/* Fill a DES context CTX with subkeys calculated from 64bit KEY, with
 * weak key checking.  Does not check parity bits, but simply ignore
 * them. */
extern bool
gl_des_makekey (gl_des_ctx *ctx, const char * key, size_t keylen);

/* Electronic Codebook Mode DES encryption/decryption of data
 * according to 'mode'. */
extern void
gl_des_ecb_crypt (gl_des_ctx *ctx, const char * from,  char * to, int mode);

#define gl_des_ecb_encrypt(ctx, from, to)  gl_des_ecb_crypt(ctx, from, to, 0)
#define gl_des_ecb_decrypt(ctx, from, to)  gl_des_ecb_crypt(ctx, from, to, 1)

/* Triple-DES
 * ----------
 */

/* Fill a Triple-DES context CTX with subkeys calculated from two
 * 64bit keys in KEY1 and KEY2.  Does not check the parity bits of the
 * keys, but simply ignore them.  Does not check for weak keys. */
extern void
gl_3des_set2keys (gl_3des_ctx *ctx,
                  const char * key1,
                  const char * key2);

/*
 * Fill a Triple-DES context CTX with subkeys calculated from three
 * 64bit keys in KEY1, KEY2 and KEY3.  Does not check the parity bits
 * of the keys, but simply ignore them.  Does not check for weak
 * keys. */
extern void
gl_3des_set3keys (gl_3des_ctx *ctx,
                  const char * key1,
                  const char * key2,
                  const char * key3);

/* Fill a Triple-DES context CTX with subkeys calculated from three
 * concatenated 64bit keys in KEY, with weak key checking.  Does not
 * check the parity bits of the keys, but simply ignore them. */
extern bool
gl_3des_makekey (gl_3des_ctx *ctx,
                 const char * key,
                 size_t keylen);

/* Electronic Codebook Mode Triple-DES encryption/decryption of data
 * according to 'mode'.  Sometimes this mode is named 'EDE' mode
 * (Encryption-Decryption-Encryption). */
extern void
gl_3des_ecb_crypt (gl_3des_ctx *ctx,
                   const char * from,
                   char * to,
                   int mode);

#define gl_3des_ecb_encrypt(ctx, from, to) gl_3des_ecb_crypt(ctx,from,to,0)
#define gl_3des_ecb_decrypt(ctx, from, to) gl_3des_ecb_crypt(ctx,from,to,1)

#endif /* DES_H */
