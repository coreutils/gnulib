/* sha3.h - Functions to calculate SHA-3 hashes as specified by FIPS-202.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Collin Funk <collin.funk1@gmail.com>, 2025.  */

#ifndef SHA3_H
# define SHA3_H 1

# include <stdio.h>
# include <stdint.h>

# include "u64.h"

# ifdef __cplusplus
extern "C" {
# endif

/* Digest sizes in bytes.  */
enum { SHA3_224_DIGEST_SIZE = 224 / 8 };
enum { SHA3_256_DIGEST_SIZE = 256 / 8 };
enum { SHA3_384_DIGEST_SIZE = 384 / 8 };
enum { SHA3_512_DIGEST_SIZE = 512 / 8 };

/* Block sizes in bytes.  */
enum { SHA3_224_BLOCK_SIZE = 1152 / 8 };
enum { SHA3_256_BLOCK_SIZE = 1088 / 8 };
enum { SHA3_384_BLOCK_SIZE = 832 / 8 };
enum { SHA3_512_BLOCK_SIZE = 576 / 8 };

/* Structure to save state of computation between the single steps.  */
struct sha3_ctx
{
  u64 state[25];
  uint8_t buffer[144]; /* Up to BLOCKLEN in use.  */
  size_t buflen;       /* ≥ 0, ≤ BLOCKLEN  */
  size_t digestlen;    /* One of SHA3_{224,256,384,512}_DIGEST_SIZE.  */
  size_t blocklen;     /* One of SHA3_{224,256,384,512}_BLOCK_SIZE.  */
};

/* Initialize structure containing state of computation.  */
extern void sha3_224_init_ctx (struct sha3_ctx *ctx);
extern void sha3_256_init_ctx (struct sha3_ctx *ctx);
extern void sha3_384_init_ctx (struct sha3_ctx *ctx);
extern void sha3_512_init_ctx (struct sha3_ctx *ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is necessary that LEN is a multiple of the BLOCKLEN member of CTX!!!  */
extern void sha3_process_block (const void *buffer, size_t len,
                                struct sha3_ctx *ctx);

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is NOT required that LEN is a multiple of the BLOCKLEN member of CTX.  */
extern void sha3_process_bytes (const void *buffer, size_t len,
                                struct sha3_ctx *ctx);

/* Process the remaining bytes in the buffer and put result from CTX in RESBUF.
   The result is always in little endian byte order, so that a byte-wise output
   yields to the wanted ASCII representation of the message digest.  */
extern void *sha3_finish_ctx (struct sha3_ctx *ctx, void *restrict resbuf);

/* Put result from CTX in RESBUF.  The result is always in little endian byte
   order, so that a byte-wise output yields to the wanted ASCII representation
   of the message digest.  */
extern void *sha3_read_ctx (const struct sha3_ctx *ctx,
                            void *restrict resbuf);

/* Compute a SHA-3 message digest for LEN bytes beginning at BUFFER.
   The result is always in little endian byte order, so that a byte-wise
   output yields to the wanted ASCII representation of the message
   digest.  */
extern void *sha3_224_buffer (const char *buffer, size_t len,
                              void *restrict resblock);
extern void *sha3_256_buffer (const char *buffer, size_t len,
                              void *restrict resblock);
extern void *sha3_384_buffer (const char *buffer, size_t len,
                              void *restrict resblock);
extern void *sha3_512_buffer (const char *buffer, size_t len,
                              void *restrict resblock);

# ifdef __cplusplus
}
# endif

#endif
