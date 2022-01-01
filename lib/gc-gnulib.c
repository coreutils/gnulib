/* gc-gnulib.c --- Common gnulib internal crypto interface functions
 * Copyright (C) 2002-2022 Free Software Foundation, Inc.
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

/* Note: This file is only built if GC uses internal functions. */

#include <config.h>

/* Get prototype. */
#include "gc.h"

#include <stdlib.h>
#include <string.h>

/* For randomize. */
#if GNULIB_GC_RANDOM
#include <unistd.h>
#include <errno.h>
#include <sys/random.h>
#endif

/* Hashes. */
#if GNULIB_GC_MD2
# include "md2.h"
#endif
#if GNULIB_GC_MD4
# include "md4.h"
#endif
#if GNULIB_GC_MD5
# include "md5.h"
#endif
#if GNULIB_GC_SHA1
# include "sha1.h"
#endif
#if GNULIB_GC_SHA256
# include "sha256.h"
#endif
#if GNULIB_GC_SHA512
# include "sha512.h"
#endif
#if GNULIB_GC_SM3
# include "sm3.h"
#endif
#if GNULIB_GC_HMAC_MD5 || GNULIB_GC_HMAC_SHA1 || GNULIB_GC_HMAC_SHA256 || GNULIB_GC_HMAC_SHA512
# include "hmac.h"
#endif

/* Ciphers. */
#if GNULIB_GC_ARCFOUR
# include "arcfour.h"
#endif
#if GNULIB_GC_ARCTWO
# include "arctwo.h"
#endif
#if GNULIB_GC_DES
# include "des.h"
#endif
#if GNULIB_GC_RIJNDAEL
# include "rijndael-api-fst.h"
#endif

Gc_rc
gc_init (void)
{
  return GC_OK;
}

void
gc_done (void)
{
  return;
}

#if GNULIB_GC_RANDOM

/* Overwrite BUFFER with random data, under the control of getrandom
   FLAGS.  BUFFER contains LENGTH bytes.  Inspired by getentropy,
   however LENGTH is not restricted to 256.  Return 0 on success, -1
   (setting errno) on failure.  */
static int
randomize (void *buffer, size_t length, unsigned int flags)
{
  char *buf = buffer;

  for (;;)
    {
      ssize_t bytes;
      if (length == 0)
        return GC_OK;
      while ((bytes = getrandom (buf, length, flags)) < 0)
        if (errno != EINTR)
          return GC_RANDOM_ERROR;
      if (bytes == 0)
        break;
      buf += bytes;
      length -= bytes;
    }

  return GC_RANDOM_ERROR;
}

Gc_rc
gc_nonce (char *data, size_t datalen)
{
  return randomize (data, datalen, 0);
}

Gc_rc
gc_pseudo_random (char *data, size_t datalen)
{
  return randomize (data, datalen, 0);
}

Gc_rc
gc_random (char *data, size_t datalen)
{
  return randomize (data, datalen, GRND_RANDOM);
}

#endif

/* Memory allocation. */

void
gc_set_allocators (gc_malloc_t func_malloc,
                   gc_malloc_t secure_malloc,
                   gc_secure_check_t secure_check,
                   gc_realloc_t func_realloc, gc_free_t func_free)
{
  return;
}

/* Ciphers. */

typedef struct _gc_cipher_ctx
{
  Gc_cipher alg;
  Gc_cipher_mode mode;
#if GNULIB_GC_ARCTWO
  arctwo_context arctwoContext;
  char arctwoIV[ARCTWO_BLOCK_SIZE];
#endif
#if GNULIB_GC_ARCFOUR
  arcfour_context arcfourContext;
#endif
#if GNULIB_GC_DES
  gl_des_ctx desContext;
#endif
#if GNULIB_GC_RIJNDAEL
  rijndaelKeyInstance aesEncKey;
  rijndaelKeyInstance aesDecKey;
  rijndaelCipherInstance aesContext;
#endif
} _gc_cipher_ctx;

Gc_rc
gc_cipher_open (Gc_cipher alg, Gc_cipher_mode mode,
                gc_cipher_handle * outhandle)
{
  _gc_cipher_ctx *ctx;
  Gc_rc rc = GC_OK;

  ctx = calloc (sizeof (*ctx), 1);
  if (!ctx)
    return GC_MALLOC_ERROR;

  ctx->alg = alg;
  ctx->mode = mode;

  switch (alg)
    {
#if GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (mode)
        {
        case GC_ECB:
        case GC_CBC:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#if GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      switch (mode)
        {
        case GC_STREAM:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#if GNULIB_GC_DES
    case GC_DES:
      switch (mode)
        {
        case GC_ECB:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

#if GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      switch (mode)
        {
        case GC_ECB:
        case GC_CBC:
          break;

        default:
          rc = GC_INVALID_CIPHER;
        }
      break;
#endif

    default:
      rc = GC_INVALID_CIPHER;
    }

  if (rc == GC_OK)
    *outhandle = ctx;
  else
    free (ctx);

  return rc;
}

Gc_rc
gc_cipher_setkey (gc_cipher_handle handle, size_t keylen, const char *key)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#if GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      arctwo_setkey (&ctx->arctwoContext, keylen, key);
      break;
#endif

#if GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_setkey (&ctx->arcfourContext, key, keylen);
      break;
#endif

#if GNULIB_GC_DES
    case GC_DES:
      if (keylen != 8)
        return GC_INVALID_CIPHER;
      gl_des_setkey (&ctx->desContext, key);
      break;
#endif

#if GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        rijndael_rc rc;
        size_t i;
        char keyMaterial[RIJNDAEL_MAX_KEY_SIZE + 1];

        for (i = 0; i < keylen; i++)
          sprintf (&keyMaterial[2 * i], "%02x", key[i] & 0xFF);

        rc = rijndaelMakeKey (&ctx->aesEncKey, RIJNDAEL_DIR_ENCRYPT,
                              keylen * 8, keyMaterial);
        if (rc < 0)
          return GC_INVALID_CIPHER;

        rc = rijndaelMakeKey (&ctx->aesDecKey, RIJNDAEL_DIR_DECRYPT,
                              keylen * 8, keyMaterial);
        if (rc < 0)
          return GC_INVALID_CIPHER;

        rc = rijndaelCipherInit (&ctx->aesContext, RIJNDAEL_MODE_ECB, NULL);
        if (rc < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}

Gc_rc
gc_cipher_setiv (gc_cipher_handle handle, size_t ivlen, const char *iv)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#if GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      if (ivlen != ARCTWO_BLOCK_SIZE)
        return GC_INVALID_CIPHER;
      memcpy (ctx->arctwoIV, iv, ivlen);
      break;
#endif

#if GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      switch (ctx->mode)
        {
        case GC_ECB:
          /* Doesn't use IV. */
          break;

        case GC_CBC:
          {
            rijndael_rc rc;
            size_t i;
            char ivMaterial[2 * RIJNDAEL_MAX_IV_SIZE + 1];

            for (i = 0; i < ivlen; i++)
              sprintf (&ivMaterial[2 * i], "%02x", iv[i] & 0xFF);

            rc = rijndaelCipherInit (&ctx->aesContext, RIJNDAEL_MODE_CBC,
                                     ivMaterial);
            if (rc < 0)
              return GC_INVALID_CIPHER;
          }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}

Gc_rc
gc_cipher_encrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#if GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (ctx->mode)
        {
        case GC_ECB:
          arctwo_encrypt (&ctx->arctwoContext, data, data, len);
          break;

        case GC_CBC:
          for (; len >= ARCTWO_BLOCK_SIZE; len -= ARCTWO_BLOCK_SIZE,
               data += ARCTWO_BLOCK_SIZE)
            {
              size_t i;
              for (i = 0; i < ARCTWO_BLOCK_SIZE; i++)
                data[i] ^= ctx->arctwoIV[i];
              arctwo_encrypt (&ctx->arctwoContext, data, data,
                              ARCTWO_BLOCK_SIZE);
              memcpy (ctx->arctwoIV, data, ARCTWO_BLOCK_SIZE);
            }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

#if GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_stream (&ctx->arcfourContext, data, data, len);
      break;
#endif

#if GNULIB_GC_DES
    case GC_DES:
      for (; len >= 8; len -= 8, data += 8)
        gl_des_ecb_encrypt (&ctx->desContext, data, data);
      break;
#endif

#if GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        int nblocks;

        nblocks = rijndaelBlockEncrypt (&ctx->aesContext, &ctx->aesEncKey,
                                        data, 8 * len, data);
        if (nblocks < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}

Gc_rc
gc_cipher_decrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  _gc_cipher_ctx *ctx = handle;

  switch (ctx->alg)
    {
#if GNULIB_GC_ARCTWO
    case GC_ARCTWO40:
      switch (ctx->mode)
        {
        case GC_ECB:
          arctwo_decrypt (&ctx->arctwoContext, data, data, len);
          break;

        case GC_CBC:
          for (; len >= ARCTWO_BLOCK_SIZE; len -= ARCTWO_BLOCK_SIZE,
               data += ARCTWO_BLOCK_SIZE)
            {
              char tmpIV[ARCTWO_BLOCK_SIZE];
              size_t i;
              memcpy (tmpIV, data, ARCTWO_BLOCK_SIZE);
              arctwo_decrypt (&ctx->arctwoContext, data, data,
                              ARCTWO_BLOCK_SIZE);
              for (i = 0; i < ARCTWO_BLOCK_SIZE; i++)
                data[i] ^= ctx->arctwoIV[i];
              memcpy (ctx->arctwoIV, tmpIV, ARCTWO_BLOCK_SIZE);
            }
          break;

        default:
          return GC_INVALID_CIPHER;
        }
      break;
#endif

#if GNULIB_GC_ARCFOUR
    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      arcfour_stream (&ctx->arcfourContext, data, data, len);
      break;
#endif

#if GNULIB_GC_DES
    case GC_DES:
      for (; len >= 8; len -= 8, data += 8)
        gl_des_ecb_decrypt (&ctx->desContext, data, data);
      break;
#endif

#if GNULIB_GC_RIJNDAEL
    case GC_AES128:
    case GC_AES192:
    case GC_AES256:
      {
        int nblocks;

        nblocks = rijndaelBlockDecrypt (&ctx->aesContext, &ctx->aesDecKey,
                                        data, 8 * len, data);
        if (nblocks < 0)
          return GC_INVALID_CIPHER;
      }
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  return GC_OK;
}

Gc_rc
gc_cipher_close (gc_cipher_handle handle)
{
  _gc_cipher_ctx *ctx = handle;

  free (ctx);

  return GC_OK;
}

/* Hashes. */

#define MAX_DIGEST_SIZE 64

typedef struct _gc_hash_ctx
{
  Gc_hash alg;
  Gc_hash_mode mode;
  char hash[MAX_DIGEST_SIZE];
#if GNULIB_GC_MD2
  struct md2_ctx md2Context;
#endif
#if GNULIB_GC_MD4
  struct md4_ctx md4Context;
#endif
#if GNULIB_GC_MD5
  struct md5_ctx md5Context;
#endif
#if GNULIB_GC_SHA1
  struct sha1_ctx sha1Context;
#endif
#if GNULIB_GC_SHA256
  struct sha256_ctx sha256Context;
#endif
#if GNULIB_GC_SHA512
  struct sha512_ctx sha512Context;
#endif
#if GNULIB_GC_SM3
  struct sm3_ctx sm3Context;
#endif
} _gc_hash_ctx;

Gc_rc
gc_hash_open (Gc_hash hash, Gc_hash_mode mode, gc_hash_handle * outhandle)
{
  _gc_hash_ctx *ctx;
  Gc_rc rc = GC_OK;

  if (mode != 0)
    return GC_INVALID_HASH;

  ctx = calloc (sizeof (*ctx), 1);
  if (!ctx)
    return GC_MALLOC_ERROR;

  ctx->alg = hash;
  ctx->mode = mode;

  switch (hash)
    {
#if GNULIB_GC_MD2
    case GC_MD2:
      /* Not needed, because ctx is already zero-initialized.  */
      /*md2_init_ctx (&ctx->md2Context);*/
      break;
#endif

#if GNULIB_GC_MD4
    case GC_MD4:
      md4_init_ctx (&ctx->md4Context);
      break;
#endif

#if GNULIB_GC_MD5
    case GC_MD5:
      md5_init_ctx (&ctx->md5Context);
      break;
#endif

#if GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_init_ctx (&ctx->sha1Context);
      break;
#endif

#if GNULIB_GC_SHA256
    case GC_SHA256:
      sha256_init_ctx (&ctx->sha256Context);
      break;
#endif

#if GNULIB_GC_SHA512
    case GC_SHA512:
      sha512_init_ctx (&ctx->sha512Context);
      break;
#endif

#if GNULIB_GC_SM3
    case GC_SM3:
      sm3_init_ctx (&ctx->sm3Context);
      break;
#endif

    default:
      rc = GC_INVALID_HASH;
      break;
    }

  if (rc == GC_OK)
    *outhandle = ctx;
  else
    free (ctx);

  return rc;
}

Gc_rc
gc_hash_clone (gc_hash_handle handle, gc_hash_handle * outhandle)
{
  _gc_hash_ctx *in = handle;
  _gc_hash_ctx *out;

  *outhandle = out = calloc (sizeof (*out), 1);
  if (!out)
    return GC_MALLOC_ERROR;

  memcpy (out, in, sizeof (*out));

  return GC_OK;
}

size_t
gc_hash_digest_length (Gc_hash hash)
{
  size_t len;

  switch (hash)
    {
    case GC_MD2:
      len = GC_MD2_DIGEST_SIZE;
      break;

    case GC_MD4:
      len = GC_MD4_DIGEST_SIZE;
      break;

    case GC_MD5:
      len = GC_MD5_DIGEST_SIZE;
      break;

    case GC_RMD160:
      len = GC_RMD160_DIGEST_SIZE;
      break;

    case GC_SHA1:
      len = GC_SHA1_DIGEST_SIZE;
      break;

    case GC_SHA256:
      len = GC_SHA256_DIGEST_SIZE;
      break;

    case GC_SHA512:
      len = GC_SHA512_DIGEST_SIZE;
      break;

    case GC_SM3:
      len = GC_SM3_DIGEST_SIZE;
      break;

    default:
      return 0;
    }

  return len;
}

void
gc_hash_write (gc_hash_handle handle, size_t len, const char *data)
{
  _gc_hash_ctx *ctx = handle;

  switch (ctx->alg)
    {
#if GNULIB_GC_MD2
    case GC_MD2:
      md2_process_bytes (data, len, &ctx->md2Context);
      break;
#endif

#if GNULIB_GC_MD4
    case GC_MD4:
      md4_process_bytes (data, len, &ctx->md4Context);
      break;
#endif

#if GNULIB_GC_MD5
    case GC_MD5:
      md5_process_bytes (data, len, &ctx->md5Context);
      break;
#endif

#if GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_process_bytes (data, len, &ctx->sha1Context);
      break;
#endif

#if GNULIB_GC_SHA256
    case GC_SHA256:
      sha256_process_bytes (data, len, &ctx->sha256Context);
      break;
#endif

#if GNULIB_GC_SHA512
    case GC_SHA512:
      sha512_process_bytes (data, len, &ctx->sha512Context);
      break;
#endif

#if GNULIB_GC_SM3
    case GC_SM3:
      sm3_process_bytes (data, len, &ctx->sm3Context);
      break;
#endif

    default:
      break;
    }
}

const char *
gc_hash_read (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;
  const char *ret = NULL;

  switch (ctx->alg)
    {
#if GNULIB_GC_MD2
    case GC_MD2:
      md2_finish_ctx (&ctx->md2Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_MD4
    case GC_MD4:
      md4_finish_ctx (&ctx->md4Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_MD5
    case GC_MD5:
      md5_finish_ctx (&ctx->md5Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_finish_ctx (&ctx->sha1Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_SHA256
    case GC_SHA256:
      sha256_finish_ctx (&ctx->sha256Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_SHA512
    case GC_SHA512:
      sha512_finish_ctx (&ctx->sha512Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

#if GNULIB_GC_SM3
    case GC_SM3:
      sm3_finish_ctx (&ctx->sm3Context, ctx->hash);
      ret = ctx->hash;
      break;
#endif

    default:
      return NULL;
    }

  return ret;
}

void
gc_hash_close (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;

  free (ctx);
}

Gc_rc
gc_hash_buffer (Gc_hash hash, const void *in, size_t inlen, char *resbuf)
{
  switch (hash)
    {
#if GNULIB_GC_MD2
    case GC_MD2:
      md2_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_MD4
    case GC_MD4:
      md4_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_MD5
    case GC_MD5:
      md5_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_SHA1
    case GC_SHA1:
      sha1_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_SHA256
    case GC_SHA256:
      sha256_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_SHA512
    case GC_SHA512:
      sha512_buffer (in, inlen, resbuf);
      break;
#endif

#if GNULIB_GC_SM3
    case GC_SM3:
      sm3_buffer (in, inlen, resbuf);
      break;
#endif

    default:
      return GC_INVALID_HASH;
    }

  return GC_OK;
}

#if GNULIB_GC_MD2
Gc_rc
gc_md2 (const void *in, size_t inlen, void *resbuf)
{
  md2_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_MD4
Gc_rc
gc_md4 (const void *in, size_t inlen, void *resbuf)
{
  md4_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_MD5
Gc_rc
gc_md5 (const void *in, size_t inlen, void *resbuf)
{
  md5_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_SHA1
Gc_rc
gc_sha1 (const void *in, size_t inlen, void *resbuf)
{
  sha1_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_SHA256
Gc_rc
gc_sha256 (const void *in, size_t inlen, void *resbuf)
{
  sha256_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_SHA512
Gc_rc
gc_sha512 (const void *in, size_t inlen, void *resbuf)
{
  sha512_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_SM3
Gc_rc
gc_sm3 (const void *in, size_t inlen, void *resbuf)
{
  sm3_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_MD5
Gc_rc
gc_hmac_md5 (const void *key, size_t keylen,
             const void *in, size_t inlen, char *resbuf)
{
  hmac_md5 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA1
Gc_rc
gc_hmac_sha1 (const void *key, size_t keylen,
              const void *in, size_t inlen, char *resbuf)
{
  hmac_sha1 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA256
Gc_rc
gc_hmac_sha256 (const void *key, size_t keylen,
                const void *in, size_t inlen, char *resbuf)
{
  hmac_sha256 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA512
Gc_rc
gc_hmac_sha512 (const void *key, size_t keylen,
                const void *in, size_t inlen, char *resbuf)
{
  hmac_sha512 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif
