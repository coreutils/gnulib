/* gc-gl-common.c --- Common gnulib internal crypto interface functions
 * Copyright (C) 2002, 2003, 2004, 2005  Simon Josefsson
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

/* Note: This file is only built if GC uses internal functions. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/* Get prototype. */
#include <gc.h>

#include <stdlib.h>
#include <string.h>

/* For randomize. */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#ifdef GC_USE_MD5
# include "md5.h"
#endif
#ifdef GC_USE_SHA1
# include "sha1.h"
#endif
#ifdef GC_USE_HMAC_MD5
# include "hmac.h"
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

/* Randomness. */

static Gc_rc
randomize (int level, char *data, size_t datalen)
{
  int fd;
  const char *device;
  size_t len = 0;
  int rc;

  switch (level)
    {
    case 0:
      device = NAME_OF_NONCE_DEVICE;
      break;

    case 1:
      device = NAME_OF_PSEUDO_RANDOM_DEVICE;
      break;

    default:
      device = NAME_OF_RANDOM_DEVICE;
      break;
    }

  fd = open (device, O_RDONLY);
  if (fd < 0)
    return GC_RANDOM_ERROR;

  do
    {
      ssize_t tmp;

      tmp = read (fd, data, datalen);

      if (tmp < 0)
	{
	  int save_errno = errno;
	  close (fd);
	  errno = save_errno;
	  return GC_RANDOM_ERROR;
	}

      len += tmp;
    }
  while (len < datalen);

  rc = close (fd);
  if (rc < 0)
    return GC_RANDOM_ERROR;

  return GC_OK;
}

Gc_rc
gc_nonce (char *data, size_t datalen)
{
  return randomize (0, data, datalen);
}

Gc_rc
gc_pseudo_random (char *data, size_t datalen)
{
  return randomize (1, data, datalen);
}

Gc_rc
gc_random (char *data, size_t datalen)
{
  return randomize (2, data, datalen);
}

/* Memory allocation. */

void
gc_set_allocators (gc_malloc_t func_malloc,
		   gc_malloc_t secure_malloc,
		   gc_secure_check_t secure_check,
		   gc_realloc_t func_realloc, gc_free_t func_free)
{
  return;
}

/* Hashes. */

Gc_rc
gc_hash_buffer (Gc_hash hash, const void *in, size_t inlen, char *resbuf)
{
  switch (hash)
    {
#ifdef GC_USE_MD5
    case GC_MD5:
      md5_buffer (in, inlen, resbuf);
      break;
#endif

#ifdef GC_USE_SHA1
    case GC_SHA1:
      sha1_buffer (in, inlen, resbuf);
      break;
#endif

    default:
      return GC_INVALID_HASH;
    }

  return GC_OK;
}

#ifdef GC_USE_MD5
Gc_rc
gc_md5 (const void *in, size_t inlen, void *resbuf)
{
  md5_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GC_USE_SHA1
Gc_rc
gc_sha1 (const void *in, size_t inlen, void *resbuf)
{
  sha1_buffer (in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GC_USE_HMAC_MD5
Gc_rc
gc_hmac_md5 (const void *key, size_t keylen,
	     const void *in, size_t inlen, char *resbuf)
{
  hmac_md5 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif

#ifdef GC_USE_HMAC_SHA1
Gc_rc
gc_hmac_sha1 (const void *key, size_t keylen,
	      const void *in, size_t inlen, char *resbuf)
{
  hmac_sha1 (key, keylen, in, inlen, resbuf);
  return GC_OK;
}
#endif
