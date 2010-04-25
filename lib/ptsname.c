/* Determine name of the slave side of a pseudo-terminal.
   Copyright (C) 1998, 2002, 2010 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _LIBC
# include <paths.h>
#else
# ifndef _PATH_TTY
#  define _PATH_TTY "/dev/tty"
# endif
# ifndef _PATH_DEV
#  define _PATH_DEV "/dev/"
# endif

# define __set_errno(e) errno = (e)
# define __isatty isatty
# define __stat stat
# define __ttyname_r ttyname_r

static int __ptsname_r (int fd, char *buf, size_t buflen);
#endif


/* Static buffer for `ptsname'.  */
static char buffer[sizeof (_PATH_TTY) + 2];


/* Return the pathname of the pseudo terminal slave associated with
   the master FD is open on, or NULL on errors.
   The returned storage is good until the next call to this function.  */
char *
ptsname (int fd)
{
  return __ptsname_r (fd, buffer, sizeof (buffer)) != 0 ? NULL : buffer;
}


/* Store at most BUFLEN characters of the pathname of the slave pseudo
   terminal associated with the master FD is open on in BUF.
   Return 0 on success, otherwise an error number.  */
static int
__ptsname_r (int fd, char *buf, size_t buflen)
{
  int save_errno = errno;
  int err;
  struct stat st;

  if (buf == NULL)
    {
      __set_errno (EINVAL);
      return EINVAL;
    }

  if (!__isatty (fd))
    /* We rely on isatty to set errno properly (i.e. EBADF or ENOTTY).  */
    return errno;

  if (buflen < strlen (_PATH_TTY) + 3)
    {
      __set_errno (ERANGE);
      return ERANGE;
    }

  err = __ttyname_r (fd, buf, buflen);
  if (err != 0)
    {
      __set_errno (err);
      return errno;
    }

  buf[sizeof (_PATH_DEV) - 1] = 't';

  if (__stat (buf, &st) < 0)
    return errno;

  __set_errno (save_errno);
  return 0;
}
