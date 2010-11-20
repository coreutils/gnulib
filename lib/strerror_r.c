/* strerror_r.c --- POSIX compatible system error routine

   Copyright (C) 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include <errno.h>

#if HAVE_DECL_STRERROR_R && !(__GLIBC__ >= 2 || defined __UCLIBC__) && !EXTEND_STRERROR_R

/* The system's strerror_r function is OK, except that its third argument
   is 'int', not 'size_t'.  */

# include <limits.h>

int
strerror_r (int errnum, char *buf, size_t buflen)
# undef strerror_r
{
  int ret;

  if (buflen > INT_MAX)
    buflen = INT_MAX;

# ifdef __hpux
  /* On HP-UX 11.31, strerror_r always fails when buflen < 80.  */
  {
    char stackbuf[80];

    if (buflen < sizeof (stackbuf))
      {
        ret = strerror_r (errnum, stackbuf, sizeof (stackbuf));
        if (ret == 0)
          {
            size_t len = strlen (stackbuf);

            if (len < buflen)
              memcpy (buf, stackbuf, len + 1);
            else
              ret = ERANGE;
          }
      }
    else
      ret = strerror_r (errnum, buf, buflen);
  }
# else
  ret = strerror_r (errnum, buf, buflen);
# endif

# ifdef _AIX
  /* On AIX 6.1, strerror_r returns -1 and sets errno to EINVAL
     if buflen <= 1.  */
  if (ret < 0 && errno == EINVAL && buflen <= 1)
    {
      /* Retry with a larger buffer.  */
      char largerbuf[10];
      ret = strerror_r (errnum, largerbuf, sizeof (largerbuf));
      if (ret < 0 && errno == EINVAL)
        {
          /* errnum was out of range.  */
          return EINVAL;
        }
      else
        {
          /* buf was too small.  */
          return ERANGE;
        }
    }
# endif

  /* Some old implementations may return (-1, EINVAL) instead of EINVAL.  */
  return (ret < 0 ? errno : ret);
}

#elif (__GLIBC__ >= 2 || defined __UCLIBC__) && HAVE___XPG_STRERROR_R /* glibc >= 2.3.4 */ && !EXTEND_STRERROR_R

int
strerror_r (int errnum, char *buf, size_t buflen)
{
  extern int __xpg_strerror_r (int errnum, char *buf, size_t buflen);

  int ret = __xpg_strerror_r (errnum, buf, buflen);
  return (ret < 0 ? errno : 0);
}

#else /* (__GLIBC__ >= 2 || defined __UCLIBC__ ? !HAVE___XPG_STRERROR_R : !HAVE_DECL_STRERROR_R) || EXTEND_STRERROR_R */

# include "glthread/lock.h"

/* Use strerror(), with locking.  */

/* This lock protects the buffer returned by strerror().  We assume that
   no other uses of strerror() exist in the program.  */
gl_lock_define_initialized(static, strerror_lock)

int
strerror_r (int errnum, char *buf, size_t buflen)
{
  gl_lock_lock (strerror_lock);

  {
    char *errmsg = strerror (errnum);
    size_t len = strlen (errmsg);
    int ret;

    if (len < buflen)
      {
        memcpy (buf, errmsg, len + 1);
        ret = 0;
      }
    else
      ret = ERANGE;

    gl_lock_unlock (strerror_lock);

    return ret;
  }
}

#endif
