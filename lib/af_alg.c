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

#ifdef HAVE_LINUX_IF_ALG_H

#include <unistd.h>
#include <string.h>
#include <linux/if_alg.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/socket.h>

#include "af_alg.h"

#include "sys-limits.h"

#define BLOCKSIZE 32768

int
afalg_stream (FILE * stream, const char *alg, void *resblock, ssize_t hashlen)
{
  struct sockaddr_alg salg = {
    .salg_family = AF_ALG,
    .salg_type = "hash",
  };
  int ret, cfd, ofd;
  struct stat st;

  cfd = socket (AF_ALG, SOCK_SEQPACKET, 0);
  if (cfd < 0)
    return -EAFNOSUPPORT;

  /* avoid calling both strcpy and strlen.  */
  for (int i = 0; (salg.salg_name[i] = alg[i]); i++)
    if (i == sizeof salg.salg_name - 1)
      return -EINVAL;

  ret = bind (cfd, (struct sockaddr *) &salg, sizeof salg);
  if (ret < 0)
    {
      ret = -EAFNOSUPPORT;
      goto out_cfd;
    }

  ofd = accept (cfd, NULL, 0);
  if (ofd < 0)
    {
      ret = -EAFNOSUPPORT;
      goto out_cfd;
    }

  /* if file is a regular file, attempt sendfile to pipe the data.  */
  if (!fstat (fileno (stream), &st)
      && (S_ISREG (st.st_mode) || S_TYPEISSHM (&st) || S_TYPEISTMO (&st))
      && 0 < st.st_size && st.st_size <= SYS_BUFSIZE_MAX)
    {
      if (sendfile (ofd, fileno (stream), NULL, st.st_size) != st.st_size)
        {
          ret = -EIO;
          goto out_ofd;
        }
      else
        ret = 0;
    }
  else
    {
      /* sendfile not possible, do a classic read-write loop.  */
      ssize_t size;
      char buf[BLOCKSIZE];
      while ((size = fread (buf, 1, sizeof buf, stream)))
        {
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
