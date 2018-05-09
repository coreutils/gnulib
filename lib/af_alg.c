/* af_alg.c - Compute message digests from file streams and buffers.
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

/* Written by Matteo Croce <mcroce@redhat.com>, 2018.  */

#include <config.h>

#if USE_LINUX_CRYPTO_API

#include "af_alg.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <linux/if_alg.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/socket.h>

#include "sys-limits.h"

#define BLOCKSIZE 32768

/* Return a newly created socket for ALG.
   On error, return a negative error number.  */
static int
alg_socket (char const *alg)
{
  struct sockaddr_alg salg = {
    .salg_family = AF_ALG,
    .salg_type = "hash",
  };
  /* Avoid calling both strcpy and strlen.  */
  for (int i = 0; (salg.salg_name[i] = alg[i]); i++)
    if (i == sizeof salg.salg_name - 1)
      return -EINVAL;

  int cfd = socket (AF_ALG, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
  if (cfd < 0)
    return -EAFNOSUPPORT;
  int ofd = (bind (cfd, (struct sockaddr *) &salg, sizeof salg) == 0
             ? accept4 (cfd, NULL, 0, SOCK_CLOEXEC)
             : -1);
  close (cfd);
  return ofd < 0 ? -EAFNOSUPPORT : ofd;
}

int
afalg_buffer (const char *buffer, size_t len, const char *alg,
              void *resblock, ssize_t hashlen)
{
  /* On Linux < 4.9, the value for an empty stream is wrong (all zeroes).
     See <https://patchwork.kernel.org/patch/9308641/>.  */
  if (len == 0)
    return -EAFNOSUPPORT;

  int ofd = alg_socket (alg);
  if (ofd < 0)
    return ofd;

  int result;

  do
    {
      ssize_t size = (len > BLOCKSIZE ? BLOCKSIZE : len);
      if (send (ofd, buffer, size, MSG_MORE) != size)
        {
          result = -EIO;
          goto out_ofd;
        }
      buffer += size;
      len -= size;
    }
  while (len > 0);

  if (read (ofd, resblock, hashlen) != hashlen)
    result = -EIO;
  else
    result = 0;

out_ofd:
  close (ofd);
  return result;
}

int
afalg_stream (FILE *stream, const char *alg,
              void *resblock, ssize_t hashlen)
{
  int ofd = alg_socket (alg);
  if (ofd < 0)
    return ofd;

  /* if file is a regular file, attempt sendfile to pipe the data.  */
  int fd = fileno (stream);
  int result;
  struct stat st;
  if (fstat (fd, &st) == 0
      && (S_ISREG (st.st_mode) || S_TYPEISSHM (&st) || S_TYPEISTMO (&st))
      && 0 < st.st_size && st.st_size <= SYS_BUFSIZE_MAX)
    {
      /* Make sure the offset of fileno (stream) reflects how many bytes
         have been read from stream before this function got invoked.
         Note: fflush on an input stream after ungetc does not work as expected
         on some platforms.  Therefore this situation is not supported here.  */
      if (fflush (stream))
        {
#if defined _WIN32 && ! defined __CYGWIN__
          result = -EIO;
#else
          result = -errno;
#endif
          goto out_ofd;
        }

      off_t nbytes = st.st_size - lseek (fd, 0, SEEK_CUR);
      /* On Linux < 4.9, the value for an empty stream is wrong (all zeroes).
         See <https://patchwork.kernel.org/patch/9308641/>.  */
      if (nbytes <= 0)
        {
          result = -EAFNOSUPPORT;
          goto out_ofd;
        }
      if (sendfile (ofd, fd, NULL, nbytes) != nbytes)
        {
          result = -EIO;
          goto out_ofd;
        }
    }
  else
    {
      /* sendfile not possible, do a classic read-write loop.  */
      int non_empty = 0;
      ssize_t size;
      char buf[BLOCKSIZE];
      while ((size = fread (buf, 1, sizeof buf, stream)))
        {
          non_empty = 1;
          if (send (ofd, buf, size, MSG_MORE) != size)
            {
              result = -EIO;
              goto out_ofd;
            }
        }
      if (ferror (stream))
        {
          result = -EIO;
          goto out_ofd;
        }
      /* On Linux < 4.9, the value for an empty stream is wrong (all zeroes).
         See <https://patchwork.kernel.org/patch/9308641/>.  */
      if (!non_empty)
        {
          result = -EAFNOSUPPORT;
          goto out_ofd;
        }
    }

  if (read (ofd, resblock, hashlen) != hashlen)
    result = -EIO;
  else
    result = 0;

out_ofd:
  close (ofd);
  return result;
}

#endif
