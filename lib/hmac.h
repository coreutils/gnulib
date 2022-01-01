/* hmac.h -- hashed message authentication codes
   Copyright (C) 2005, 2009-2022 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson.  */

#ifndef HMAC_H
# define HMAC_H 1

#include <stddef.h>

/* Compute Hashed Message Authentication Code with MD5, as described
   in RFC 2104, over BUFFER data of BUFLEN bytes using the KEY of
   KEYLEN bytes, writing the output to pre-allocated 16 byte minimum
   RESBUF buffer.  Return 0 on success.  */
int
hmac_md5 (const void *key, size_t keylen,
          const void *buffer, size_t buflen, void *restrict resbuf);

/* Compute Hashed Message Authentication Code with SHA-1, over BUFFER
   data of BUFLEN bytes using the KEY of KEYLEN bytes, writing the
   output to pre-allocated 20 byte minimum RESBUF buffer.  Return 0 on
   success.  */
int
hmac_sha1 (const void *key, size_t keylen,
           const void *in, size_t inlen, void *restrict resbuf);

/* Compute Hashed Message Authentication Code with SHA-256, over BUFFER
   data of BUFLEN bytes using the KEY of KEYLEN bytes, writing the
   output to pre-allocated 32 byte minimum RESBUF buffer.  Return 0 on
   success.  */
int
hmac_sha256 (const void *key, size_t keylen,
             const void *in, size_t inlen, void *restrict resbuf);

/* Compute Hashed Message Authentication Code with SHA-512, over BUFFER
   data of BUFLEN bytes using the KEY of KEYLEN bytes, writing the
   output to pre-allocated 64 byte minimum RESBUF buffer.  Return 0 on
   success.  */
int
hmac_sha512 (const void *key, size_t keylen,
             const void *in, size_t inlen, void *restrict resbuf);

#endif /* HMAC_H */
