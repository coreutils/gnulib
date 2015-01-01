/* Determine name of the slave side of a pseudo-terminal.
   Copyright (C) 1998, 2002, 2010-2015 Free Software Foundation, Inc.

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
#include <fcntl.h>
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
# define __ptsname_r ptsname_r

#endif

#ifdef __sun
/* Get ioctl() and 'struct strioctl'.  */
# include <stropts.h>
/* Get ISPTM.  */
# include <sys/stream.h>
# include <sys/ptms.h>
/* Get the major, minor macros.  */
# include <sys/sysmacros.h>
# include <stdio.h>
#endif

#if defined _AIX || defined __osf__
/* Get ioctl(), ISPTM.  */
# include <sys/ioctl.h>
/* Get the major, minor macros.  */
# include <sys/sysmacros.h>
# include <stdio.h>
#endif


/* Store at most BUFLEN characters of the pathname of the slave pseudo
   terminal associated with the master FD is open on in BUF.
   Return 0 on success, otherwise an error number.  */
int
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

#if defined __sun /* Solaris */
  if (fstat (fd, &st) < 0)
    return errno;
  if (!(S_ISCHR (st.st_mode) && major (st.st_rdev) == 0))
    {
      errno = ENOTTY;
      return errno;
    }
  {
    /* Master ptys can be recognized through a STREAMS ioctl.  See
       "STREAMS-based Pseudo-Terminal Subsystem"
       <http://docs.oracle.com/cd/E18752_01/html/816-4855/termsub15-44781.html>
       and "STREAMS ioctl commands"
       <http://docs.oracle.com/cd/E18752_01/html/816-5177/streamio-7i.html>
     */
    struct strioctl ioctl_arg;
    ioctl_arg.ic_cmd = ISPTM;
    ioctl_arg.ic_timout = 0;
    ioctl_arg.ic_len = 0;
    ioctl_arg.ic_dp = NULL;

    if (ioctl (fd, I_STR, &ioctl_arg) < 0)
      {
        errno = ENOTTY;
        return errno;
      }
  }
  {
    char tmpbuf[9 + 10 + 1];
    int n = sprintf (tmpbuf, "/dev/pts/%u", minor (st.st_rdev));
    if (n >= buflen)
      {
        errno = ERANGE;
        return errno;
      }
    memcpy (buf, tmpbuf, n + 1);
  }
#elif defined _AIX || defined __osf__ /* AIX, OSF/1 */
  /* This implementation returns /dev/pts/N, like ptsname() does.
     Whereas the generic implementation below returns /dev/ttypN.
     Both are correct, but let's be consistent with ptsname().  */
  if (fstat (fd, &st) < 0)
    return errno;
  if (!S_ISCHR (st.st_mode))
    {
      errno = ENOTTY;
      return errno;
    }
  {
    int ret;
    int dev;
    char tmpbuf[9 + 10 + 1];
    int n;
# ifdef _AIX
    ret = ioctl (fd, ISPTM, &dev);
# endif
# ifdef __osf__
    ret = ioctl (fd, ISPTM, NULL);
    dev = ret;
# endif
    if (ret < 0)
      {
        errno = ENOTTY;
        return errno;
      }
    n = sprintf (tmpbuf, "/dev/pts/%u", minor (dev));
    if (n >= buflen)
      {
        errno = ERANGE;
        return errno;
      }
    memcpy (buf, tmpbuf, n + 1);
  }
#else
  if (!__isatty (fd))
    {
#if ISATTY_FAILS_WITHOUT_SETTING_ERRNO && defined F_GETFL /* IRIX, Solaris */
      /* Set errno.  */
      if (fcntl (fd, F_GETFL) != -1)
        errno = ENOTTY;
#else
      /* We rely on isatty to set errno properly (i.e. EBADF or ENOTTY).  */
#endif
      return errno;
    }

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

  if (strncmp(buf, "/dev/pts/", strlen("/dev/pts/")) != 0)
    buf[sizeof (_PATH_DEV) - 1] = 't';
#endif

  if (__stat (buf, &st) < 0)
    return errno;

  __set_errno (save_errno);
  return 0;
}
