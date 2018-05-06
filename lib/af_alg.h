/* af_alg.h - Compute message digests from file streams.
   Copyright (C) 2018 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Matteo Croce <mcroce@redhat.com>, 2018.
   Documentation by Bruno Haible <bruno@clisp.org>, 2018.  */

/* Declare specific functions for computing message digests
   using the Linux kernel crypto API, if available.  This kernel API gives
   access to specialized crypto instructions (that would also be available
   in user space) or to crypto devices (not directly available in user space).

   For a more complete set of facilities that use the Linux kernel crypto API,
   look at libkcapi.  */

#ifndef AF_ALG_H
# define AF_ALG_H 1

# include <stdio.h>
# include <errno.h>

# ifdef __cplusplus
extern "C" {
# endif

# if HAVE_LINUX_IF_ALG_H

/* Compute a message digest of the contents of a file.
   STREAM is an open file stream.  Regular files are handled more efficiently.
   ALG is the message digest algorithm; see the file /proc/crypto.
   RESBLOCK points to a block of HASHLEN bytes, for the result.
   HASHLEN must be the length of the message digest, in bytes, in particular:

      alg    | hashlen
      -------+--------
      md5    | 16
      sha1   | 20
      sha224 | 28
      sha256 | 32
      sha384 | 48
      sha512 | 64

   If successful, fill RESBLOCK and return 0.
   Upon failure, return a negated error number.  */
int
afalg_stream (FILE *stream, const char *alg, void *resblock, ssize_t hashlen);

# else

static int
afalg_stream (FILE *stream, const char *alg, void *resblock, ssize_t hashlen)
{
  return -EAFNOSUPPORT;
}

# endif

# ifdef __cplusplus
}
# endif

#endif /* AF_ALG_H */
