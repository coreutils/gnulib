/* sha3-stream.c - Functions to compute the SHA-3 message digest of files as
   specified by FIPS-202.

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

#include <config.h>

/* Specification.  */
#include "sha3.h"

#include <stdlib.h>

#if USE_UNLOCKED_IO
# include "unlocked-io.h"
#endif

#include "af_alg.h"

#define BLOCKSIZE 31824
#if (BLOCKSIZE % 144 != 0 && BLOCKSIZE % 136 != 0 && BLOCKSIZE % 104 != 0 \
     && BLOCKSIZE % 72 != 0)
# error "invalid BLOCKSIZE"
#endif

/* Compute message digest for bytes read from STREAM using algorithm ALG.
   Write the message digest into RESBLOCK, which contains HASHLEN bytes.
   The initial operation is INIT_CTX.  Return zero if and only if
   successful.  */
static int
sha3_xxx_stream (FILE *stream, char const *alg, void *resblock,
                 ssize_t hashlen, bool (*init_ctx) (struct sha3_ctx *))
{
  switch (afalg_stream (stream, alg, resblock, hashlen))
    {
    case 0: return 0;
    case -EIO: return 1;
    }

  char *buffer = malloc (BLOCKSIZE + 72);
  if (!buffer)
    return 1;

  struct sha3_ctx ctx;
  if (! init_ctx (&ctx))
    {
      free (buffer);
      sha3_free_ctx (&ctx);
      return 1;
    }
  size_t sum;

  /* Iterate over full file contents.  */
  while (1)
    {
      /* We read the file in blocks of BLOCKSIZE bytes.  One call of the
         computation function processes the whole buffer so that with the
         next round of the loop another block can be read.  */
      size_t n;
      sum = 0;

      /* Read block.  Take care for partial reads.  */
      while (1)
        {
          /* Either process a partial fread() from this loop,
             or the fread() in afalg_stream may have gotten EOF.
             We need to avoid a subsequent fread() as EOF may
             not be sticky.  For details of such systems, see:
             https://sourceware.org/PR1190  */
          if (feof (stream))
            goto process_partial_block;

          n = fread (buffer + sum, 1, BLOCKSIZE - sum, stream);

          sum += n;

          if (sum == BLOCKSIZE)
            break;

          if (n == 0)
            {
              /* Check for the error flag IFF N == 0, so that we don't
                 exit the loop after a partial read due to e.g., EAGAIN
                 or EWOULDBLOCK.  */
              if (ferror (stream))
                {
                  free (buffer);
                  sha3_free_ctx (&ctx);
                  return 1;
                }
              goto process_partial_block;
            }
        }

      /* Process buffer with BLOCKSIZE bytes.  Note that
                        BLOCKSIZE % ctx.blocklen == 0
       */
      if (! sha3_process_block (buffer, BLOCKSIZE, &ctx))
        {
          free (buffer);
          sha3_free_ctx (&ctx);
          return 1;
        }
    }

 process_partial_block:;

  /* Process any remaining bytes.  */
  if (sum > 0)
    {
      if (! sha3_process_bytes (buffer, sum, &ctx))
        {
          free (buffer);
          sha3_free_ctx (&ctx);
          return 1;
        }
    }

  free (buffer);

  /* Construct result in desired memory.  */
  if (sha3_finish_ctx (&ctx, resblock) == NULL)
    {
      sha3_free_ctx (&ctx);
      return 1;
    }

  sha3_free_ctx (&ctx);
  return 0;
}

int
sha3_224_stream (FILE *stream, void *resblock)
{
  return sha3_xxx_stream (stream, "sha3-224", resblock, SHA3_224_DIGEST_SIZE,
                          sha3_224_init_ctx);
}

int
sha3_256_stream (FILE *stream, void *resblock)
{
  return sha3_xxx_stream (stream, "sha3-256", resblock, SHA3_256_DIGEST_SIZE,
                          sha3_256_init_ctx);
}

int
sha3_384_stream (FILE *stream, void *resblock)
{
  return sha3_xxx_stream (stream, "sha3-384", resblock, SHA3_384_DIGEST_SIZE,
                          sha3_384_init_ctx);
}

int
sha3_512_stream (FILE *stream, void *resblock)
{
  return sha3_xxx_stream (stream, "sha3-512", resblock, SHA3_512_DIGEST_SIZE,
                          sha3_512_init_ctx);
}
