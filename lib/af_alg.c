/* af_alg.c - Functions to compute message digest from file streams using
   Linux kernel crypto API.
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

int
afalg_stream (FILE *stream, const char *alg, void *resblock, ssize_t hashlen)
{
  int cfd = socket (AF_ALG, SOCK_SEQPACKET, 0);
  if (cfd < 0)
    return -EAFNOSUPPORT;

  struct sockaddr_alg salg = {
    .salg_family = AF_ALG,
    .salg_type = "hash",
  };
  /* Avoid calling both strcpy and strlen.  */
  for (int i = 0; (salg.salg_name[i] = alg[i]); i++)
    if (i == sizeof salg.salg_name - 1)
      return -EINVAL;

  int ret = bind (cfd, (struct sockaddr *) &salg, sizeof salg);
  if (ret != 0)
    {
      ret = -EAFNOSUPPORT;
      goto out_cfd;
    }

  int ofd = accept (cfd, NULL, 0);
  if (ofd < 0)
    {
      ret = -EAFNOSUPPORT;
      goto out_cfd;
    }

  /* if file is a regular file, attempt sendfile to pipe the data.  */
  int fd = fileno (stream);
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
          ret = -EIO;
#else
          ret = -errno;
#endif
          goto out_cfd;
        }

      off_t nbytes = st.st_size - lseek (fd, 0, SEEK_CUR);
      /* On Linux < 4.9, the value for an empty stream is wrong (all zeroes).
         See <https://patchwork.kernel.org/patch/9434741/>.  */
      if (nbytes <= 0)
        {
          ret = -EAFNOSUPPORT;
          goto out_ofd;
        }
      if (sendfile (ofd, fd, NULL, nbytes) != nbytes)
        {
          ret = -EIO;
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
              ret = -EIO;
              goto out_ofd;
            }
        }
      if (ferror (stream))
        {
          ret = -EIO;
          goto out_ofd;
        }
      /* On Linux < 4.9, the value for an empty stream is wrong (all zeroes).
         See <https://patchwork.kernel.org/patch/9434741/>.  */
      if (!non_empty)
        {
          ret = -EAFNOSUPPORT;
          goto out_ofd;
        }
    }

  if (read (ofd, resblock, hashlen) != hashlen)
    ret = -EIO;
  else
    ret = 0;

out_ofd:
  close (ofd);
out_cfd:
  close (cfd);
  return ret;
}

#endif
