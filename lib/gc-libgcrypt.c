/* gc-libgcrypt.c --- Crypto wrappers around Libgcrypt for GC.
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

/* Note: This file is only built if GC uses Libgcrypt. */

#include <config.h>

/* Get prototype. */
#include "gc.h"

#include <stdlib.h>
#include <string.h>

/* Get libgcrypt API. */
#include <gcrypt.h>
#if GNULIB_GC_MD2
/* libgcrypt declares GCRY_MD_MD2 but does not actually implement the
   MD2 algorithm.  Therefore take the implementation from gnulib.  */
# include "md2.h"
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
# include "sm3.h"
#endif

#include <assert.h>

#ifndef MIN_GCRYPT_VERSION
# define MIN_GCRYPT_VERSION "1.4.4"
#endif

/* Initialization. */

Gc_rc
gc_init (void)
{
  gcry_error_t err;

  err = gcry_control (GCRYCTL_ANY_INITIALIZATION_P);
  if (err == GPG_ERR_NO_ERROR)
    {
      if (gcry_control (GCRYCTL_DISABLE_SECMEM, NULL, 0))
        return GC_INIT_ERROR;

      if (gcry_check_version (MIN_GCRYPT_VERSION) == NULL)
        return GC_INIT_ERROR;

      err = gcry_control (GCRYCTL_INITIALIZATION_FINISHED, NULL, 0);
      if (err != GPG_ERR_NO_ERROR)
        return GC_INIT_ERROR;
    }

  return GC_OK;
}

void
gc_done (void)
{
  return;
}

#if GNULIB_GC_RANDOM

/* Randomness. */

Gc_rc
gc_nonce (char *data, size_t datalen)
{
  gcry_create_nonce ((unsigned char *) data, datalen);
  return GC_OK;
}

Gc_rc
gc_pseudo_random (char *data, size_t datalen)
{
  gcry_randomize ((unsigned char *) data, datalen, GCRY_STRONG_RANDOM);
  return GC_OK;
}

Gc_rc
gc_random (char *data, size_t datalen)
{
  gcry_randomize ((unsigned char *) data, datalen, GCRY_VERY_STRONG_RANDOM);
  return GC_OK;
}

#endif

/* Memory allocation. */

void
gc_set_allocators (gc_malloc_t func_malloc,
                   gc_malloc_t secure_malloc,
                   gc_secure_check_t secure_check,
                   gc_realloc_t func_realloc, gc_free_t func_free)
{
  gcry_set_allocation_handler (func_malloc, secure_malloc, secure_check,
                               func_realloc, func_free);
}

/* Ciphers. */

Gc_rc
gc_cipher_open (Gc_cipher alg, Gc_cipher_mode mode,
                gc_cipher_handle * outhandle)
{
  int gcryalg, gcrymode;
  gcry_error_t err;

  switch (alg)
    {
    case GC_AES128:
      gcryalg = GCRY_CIPHER_RIJNDAEL;
      break;

    case GC_AES192:
      gcryalg = GCRY_CIPHER_RIJNDAEL;
      break;

    case GC_AES256:
      gcryalg = GCRY_CIPHER_RIJNDAEL256;
      break;

    case GC_3DES:
      gcryalg = GCRY_CIPHER_3DES;
      break;

    case GC_DES:
      gcryalg = GCRY_CIPHER_DES;
      break;

    case GC_ARCFOUR128:
    case GC_ARCFOUR40:
      gcryalg = GCRY_CIPHER_ARCFOUR;
      break;

    case GC_ARCTWO40:
      gcryalg = GCRY_CIPHER_RFC2268_40;
      break;

#ifdef HAVE_CAMELLIA
    case GC_CAMELLIA128:
      gcryalg = GCRY_CIPHER_CAMELLIA128;
      break;

    case GC_CAMELLIA256:
      gcryalg = GCRY_CIPHER_CAMELLIA256;
      break;
#endif

    default:
      return GC_INVALID_CIPHER;
    }

  switch (mode)
    {
    case GC_ECB:
      gcrymode = GCRY_CIPHER_MODE_ECB;
      break;

    case GC_CBC:
      gcrymode = GCRY_CIPHER_MODE_CBC;
      break;

    case GC_STREAM:
      gcrymode = GCRY_CIPHER_MODE_STREAM;
      break;

    default:
      return GC_INVALID_CIPHER;
    }

  err = gcry_cipher_open ((gcry_cipher_hd_t *) outhandle,
                          gcryalg, gcrymode, 0);
  if (gcry_err_code (err))
    return GC_INVALID_CIPHER;

  return GC_OK;
}

Gc_rc
gc_cipher_setkey (gc_cipher_handle handle, size_t keylen, const char *key)
{
  gcry_error_t err;

  err = gcry_cipher_setkey ((gcry_cipher_hd_t) handle, key, keylen);
  if (gcry_err_code (err))
    return GC_INVALID_CIPHER;

  return GC_OK;
}

Gc_rc
gc_cipher_setiv (gc_cipher_handle handle, size_t ivlen, const char *iv)
{
  gcry_error_t err;

  err = gcry_cipher_setiv ((gcry_cipher_hd_t) handle, iv, ivlen);
  if (gcry_err_code (err))
    return GC_INVALID_CIPHER;

  return GC_OK;
}

Gc_rc
gc_cipher_encrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  if (gcry_cipher_encrypt ((gcry_cipher_hd_t) handle,
                           data, len, NULL, len) != 0)
    return GC_INVALID_CIPHER;

  return GC_OK;
}

Gc_rc
gc_cipher_decrypt_inline (gc_cipher_handle handle, size_t len, char *data)
{
  if (gcry_cipher_decrypt ((gcry_cipher_hd_t) handle,
                           data, len, NULL, len) != 0)
    return GC_INVALID_CIPHER;

  return GC_OK;
}

Gc_rc
gc_cipher_close (gc_cipher_handle handle)
{
  gcry_cipher_close (handle);

  return GC_OK;
}

/* Hashes. */

/* Maximum of GC_MD2_DIGEST_SIZE and GC_SM3_DIGEST_SIZE.  */
#define MAX_DIGEST_SIZE 32

typedef struct _gc_hash_ctx {
  Gc_hash alg;
  Gc_hash_mode mode;
  gcry_md_hd_t gch;
#if GNULIB_GC_MD2 || (GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3)
  char hash[MAX_DIGEST_SIZE];
#endif
#if GNULIB_GC_MD2
  struct md2_ctx md2Context;
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
  struct sm3_ctx sm3Context;
#endif
} _gc_hash_ctx;

Gc_rc
gc_hash_open (Gc_hash hash, Gc_hash_mode mode, gc_hash_handle * outhandle)
{
  _gc_hash_ctx *ctx;
  int gcryalg = 0, gcrymode = 0;
  gcry_error_t err;
  Gc_rc rc = GC_OK;

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
      gcryalg = GCRY_MD_NONE;
      break;
#endif

    case GC_MD4:
      gcryalg = GCRY_MD_MD4;
      break;

    case GC_MD5:
      gcryalg = GCRY_MD_MD5;
      break;

    case GC_SHA1:
      gcryalg = GCRY_MD_SHA1;
      break;

    case GC_SHA256:
      gcryalg = GCRY_MD_SHA256;
      break;

    case GC_SHA384:
      gcryalg = GCRY_MD_SHA384;
      break;

    case GC_SHA512:
      gcryalg = GCRY_MD_SHA512;
      break;

    case GC_SHA224:
      gcryalg = GCRY_MD_SHA224;
      break;

    case GC_RMD160:
      gcryalg = GCRY_MD_RMD160;
      break;

#if GNULIB_GC_SM3
    case GC_SM3:
# if LIBGCRYPT_HAS_MD_SM3
      gcryalg = GCRY_MD_SM3;
# else
      sm3_init_ctx (&ctx->sm3Context);
      gcryalg = GCRY_MD_NONE;
# endif
      break;
#endif

    default:
      rc = GC_INVALID_HASH;
    }

  switch (mode)
    {
    case GC_NULL:
      gcrymode = 0;
      break;

    case GC_HMAC:
      gcrymode = GCRY_MD_FLAG_HMAC;
      break;

    default:
      rc = GC_INVALID_HASH;
    }

  if (rc == GC_OK && gcryalg != GCRY_MD_NONE)
    {
      err = gcry_md_open (&ctx->gch, gcryalg, gcrymode);
      if (gcry_err_code (err))
        rc = GC_INVALID_HASH;
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
  int err;

  *outhandle = out = calloc (sizeof (*out), 1);
  if (!out)
    return GC_MALLOC_ERROR;

  memcpy (out, in, sizeof (*out));

  err = gcry_md_copy (&out->gch, in->gch);
  if (err)
    {
      free (out);
      return GC_INVALID_HASH;
    }

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

    case GC_SHA384:
      len = GC_SHA384_DIGEST_SIZE;
      break;

    case GC_SHA512:
      len = GC_SHA512_DIGEST_SIZE;
      break;

    case GC_SHA224:
      len = GC_SHA224_DIGEST_SIZE;
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
gc_hash_hmac_setkey (gc_hash_handle handle, size_t len, const char *key)
{
  _gc_hash_ctx *ctx = handle;
#if GNULIB_GC_MD2
  if (ctx->alg != GC_MD2)
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
    if (ctx->alg != GC_SM3)
#endif
      gcry_md_setkey (ctx->gch, key, len);
}

void
gc_hash_write (gc_hash_handle handle, size_t len, const char *data)
{
  _gc_hash_ctx *ctx = handle;

#if GNULIB_GC_MD2
  if (ctx->alg == GC_MD2)
    md2_process_bytes (data, len, &ctx->md2Context);
  else
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
  if (ctx->alg == GC_SM3)
    sm3_process_bytes (data, len, &ctx->sm3Context);
  else
#endif
    gcry_md_write (ctx->gch, data, len);
}

const char *
gc_hash_read (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;
  const char *digest;

#if GNULIB_GC_MD2
  if (ctx->alg == GC_MD2)
    {
      md2_finish_ctx (&ctx->md2Context, ctx->hash);
      digest = ctx->hash;
    }
  else
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
  if (ctx->alg == GC_SM3)
    {
      sm3_finish_ctx (&ctx->sm3Context, ctx->hash);
      digest = ctx->hash;
    }
  else
#endif
    {
      gcry_md_final (ctx->gch);
      digest = (const char *) gcry_md_read (ctx->gch, 0);
    }

  return digest;
}

void
gc_hash_close (gc_hash_handle handle)
{
  _gc_hash_ctx *ctx = handle;

#if GNULIB_GC_MD2
  if (ctx->alg != GC_MD2)
#endif
#if GNULIB_GC_SM3 && !LIBGCRYPT_HAS_MD_SM3
    if (ctx->alg != GC_SM3)
#endif
      gcry_md_close (ctx->gch);

  free (ctx);
}

Gc_rc
gc_hash_buffer (Gc_hash hash, const void *in, size_t inlen, char *resbuf)
{
  int gcryalg;

  switch (hash)
    {
#if GNULIB_GC_MD2
    case GC_MD2:
      md2_buffer (in, inlen, resbuf);
      return GC_OK;
#endif

#if GNULIB_GC_MD4
    case GC_MD4:
      gcryalg = GCRY_MD_MD4;
      break;
#endif

#if GNULIB_GC_MD5
    case GC_MD5:
      gcryalg = GCRY_MD_MD5;
      break;
#endif

#if GNULIB_GC_SHA1
    case GC_SHA1:
      gcryalg = GCRY_MD_SHA1;
      break;
#endif

#if GNULIB_GC_SHA256
    case GC_SHA256:
      gcryalg = GCRY_MD_SHA256;
      break;
#endif

#if GNULIB_GC_SHA384
    case GC_SHA384:
      gcryalg = GCRY_MD_SHA384;
      break;
#endif

#if GNULIB_GC_SHA512
    case GC_SHA512:
      gcryalg = GCRY_MD_SHA512;
      break;
#endif

#if GNULIB_GC_SHA224
    case GC_SHA224:
      gcryalg = GCRY_MD_SHA224;
      break;
#endif

#if GNULIB_GC_RMD160
    case GC_RMD160:
      gcryalg = GCRY_MD_RMD160;
      break;
#endif

#if GNULIB_GC_SM3
    case GC_SM3:
# if !LIBGCRYPT_HAS_MD_SM3
      sm3_buffer (in, inlen, resbuf);
      return GC_OK;
# else
      gcryalg = GCRY_MD_SM3;
      break;
# endif
#endif

    default:
      return GC_INVALID_HASH;
    }

  gcry_md_hash_buffer (gcryalg, resbuf, in, inlen);

  return GC_OK;
}

/* One-call interface. */

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
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_MD4);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_MD4_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_MD4, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_MD4);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
}
#endif

#if GNULIB_GC_MD5
Gc_rc
gc_md5 (const void *in, size_t inlen, void *resbuf)
{
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_MD5);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_MD5_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_MD5, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_MD5);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
}
#endif

#if GNULIB_GC_SHA1
Gc_rc
gc_sha1 (const void *in, size_t inlen, void *resbuf)
{
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_SHA1);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_SHA1_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_SHA1, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_SHA1);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
}
#endif

#if GNULIB_GC_SHA256
Gc_rc
gc_sha256 (const void *in, size_t inlen, void *resbuf)
{
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_SHA256);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_SHA256_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_SHA256, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_SHA256);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
}
#endif

#if GNULIB_GC_SHA512
Gc_rc
gc_sha512 (const void *in, size_t inlen, void *resbuf)
{
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_SHA512);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_SHA512_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_SHA512, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_SHA512);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
}
#endif

#if GNULIB_GC_SM3
Gc_rc
gc_sm3  (const void *in, size_t inlen, void *resbuf)
{
# if !LIBGCRYPT_HAS_MD_SM3
  sm3_buffer (in, inlen, resbuf);
  return GC_OK;
# else
  size_t outlen = gcry_md_get_algo_dlen (GCRY_MD_SM3);
  gcry_md_hd_t hd;
  gpg_error_t err;
  unsigned char *p;

  assert (outlen == GC_SM3_DIGEST_SIZE);

  err = gcry_md_open (&hd, GCRY_MD_SM3, 0);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  gcry_md_write (hd, in, inlen);

  p = gcry_md_read (hd, GCRY_MD_SM3);
  if (p == NULL)
    {
      gcry_md_close (hd);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, p, outlen);

  gcry_md_close (hd);

  return GC_OK;
# endif
}
#endif

#if GNULIB_GC_HMAC_MD5
Gc_rc
gc_hmac_md5 (const void *key, size_t keylen,
             const void *in, size_t inlen, char *resbuf)
{
  size_t hlen = gcry_md_get_algo_dlen (GCRY_MD_MD5);
  gcry_md_hd_t mdh;
  unsigned char *hash;
  gpg_error_t err;

  assert (hlen == GC_MD5_DIGEST_SIZE);

  err = gcry_md_open (&mdh, GCRY_MD_MD5, GCRY_MD_FLAG_HMAC);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  err = gcry_md_setkey (mdh, key, keylen);
  if (err != GPG_ERR_NO_ERROR)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  gcry_md_write (mdh, in, inlen);

  hash = gcry_md_read (mdh, GCRY_MD_MD5);
  if (hash == NULL)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, hash, hlen);

  gcry_md_close (mdh);

  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA1
Gc_rc
gc_hmac_sha1 (const void *key, size_t keylen,
              const void *in, size_t inlen, char *resbuf)
{
  size_t hlen = gcry_md_get_algo_dlen (GCRY_MD_SHA1);
  gcry_md_hd_t mdh;
  unsigned char *hash;
  gpg_error_t err;

  assert (hlen == GC_SHA1_DIGEST_SIZE);

  err = gcry_md_open (&mdh, GCRY_MD_SHA1, GCRY_MD_FLAG_HMAC);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  err = gcry_md_setkey (mdh, key, keylen);
  if (err != GPG_ERR_NO_ERROR)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  gcry_md_write (mdh, in, inlen);

  hash = gcry_md_read (mdh, GCRY_MD_SHA1);
  if (hash == NULL)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, hash, hlen);

  gcry_md_close (mdh);

  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA256
Gc_rc
gc_hmac_sha256 (const void *key, size_t keylen,
             const void *in, size_t inlen, char *resbuf)
{
  size_t hlen = gcry_md_get_algo_dlen (GCRY_MD_SHA256);
  gcry_md_hd_t mdh;
  unsigned char *hash;
  gpg_error_t err;

  assert (hlen == GC_SHA256_DIGEST_SIZE);

  err = gcry_md_open (&mdh, GCRY_MD_SHA256, GCRY_MD_FLAG_HMAC);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  err = gcry_md_setkey (mdh, key, keylen);
  if (err != GPG_ERR_NO_ERROR)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  gcry_md_write (mdh, in, inlen);

  hash = gcry_md_read (mdh, GCRY_MD_SHA256);
  if (hash == NULL)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, hash, hlen);

  gcry_md_close (mdh);

  return GC_OK;
}
#endif

#if GNULIB_GC_HMAC_SHA512
Gc_rc
gc_hmac_sha512 (const void *key, size_t keylen,
              const void *in, size_t inlen, char *resbuf)
{
  size_t hlen = gcry_md_get_algo_dlen (GCRY_MD_SHA512);
  gcry_md_hd_t mdh;
  unsigned char *hash;
  gpg_error_t err;

  assert (hlen == GC_SHA512_DIGEST_SIZE);

  err = gcry_md_open (&mdh, GCRY_MD_SHA512, GCRY_MD_FLAG_HMAC);
  if (err != GPG_ERR_NO_ERROR)
    return GC_INVALID_HASH;

  err = gcry_md_setkey (mdh, key, keylen);
  if (err != GPG_ERR_NO_ERROR)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  gcry_md_write (mdh, in, inlen);

  hash = gcry_md_read (mdh, GCRY_MD_SHA512);
  if (hash == NULL)
    {
      gcry_md_close (mdh);
      return GC_INVALID_HASH;
    }

  memcpy (resbuf, hash, hlen);

  gcry_md_close (mdh);

  return GC_OK;
}
#endif
