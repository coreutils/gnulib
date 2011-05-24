/* strerror_r.c --- POSIX compatible system error routine

   Copyright (C) 2010-2011 Free Software Foundation, Inc.

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

/* Enable declaration of sys_nerr and sys_errlist in <errno.h> on NetBSD.  */
#define _NETBSD_SOURCE 1

/* Specification.  */
#include <string.h>

#include <errno.h>
#include <stdio.h>

#if GNULIB_defined_ESOCK /* native Windows platforms */
# if HAVE_WINSOCK2_H
#  include <winsock2.h>
# endif
#endif

/* Reasonable buffer size that should never trigger ERANGE; if this
   proves too small, we intentionally abort(), to remind us to fix
   this value as well as strerror-impl.h.  */
#define STACKBUF_LEN 256

#if (__GLIBC__ >= 2 || defined __UCLIBC__ || defined __CYGWIN__) && HAVE___XPG_STRERROR_R /* glibc >= 2.3.4, cygwin >= 1.7.9 */

# define USE_XPG_STRERROR_R 1

#elif HAVE_DECL_STRERROR_R && !(__GLIBC__ >= 2 || defined __UCLIBC__ || defined __CYGWIN__)

/* The system's strerror_r function is OK, except that its third argument
   is 'int', not 'size_t', or its return type is wrong.  */

# include <limits.h>

# define USE_SYSTEM_STRERROR_R 1

#else /* (__GLIBC__ >= 2 || defined __UCLIBC__ || defined __CYGWIN__ ? !HAVE___XPG_STRERROR_R : !HAVE_DECL_STRERROR_R) */

/* Use the system's strerror().  Exclude glibc and cygwin because the
   system strerror_r has the wrong return type, and cygwin 1.7.9
   strerror_r clobbers strerror.  */
# undef strerror

# define USE_SYSTEM_STRERROR 1

# if defined __NetBSD__ || defined __hpux || ((defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__) || defined __sgi || (defined __sun && !defined _LP64) || defined __CYGWIN__

/* No locking needed.  */

/* Get catgets internationalization functions.  */
#  if HAVE_CATGETS
#   include <nl_types.h>
#  endif

/* Get sys_nerr, sys_errlist on HP-UX (otherwise only declared in C++ mode).
   Get sys_nerr, sys_errlist on IRIX (otherwise only declared with _SGIAPI).  */
#  if defined __hpux || defined __sgi
extern int sys_nerr;
extern char *sys_errlist[];
#  endif

/* Get sys_nerr on Solaris.  */
#  if defined __sun && !defined _LP64
extern int sys_nerr;
#  endif

/* Get sys_nerr, sys_errlist on native Windows.  */
#  include <stdlib.h>

# else

#  include "glthread/lock.h"

/* This lock protects the buffer returned by strerror().  We assume that
   no other uses of strerror() exist in the program.  */
gl_lock_define_initialized(static, strerror_lock)

# endif

#endif

/* Copy as much of MSG into BUF as possible, without corrupting errno.
   Return 0 if MSG fit in BUFLEN, otherwise return ERANGE.  */
static int
safe_copy (char *buf, size_t buflen, const char *msg)
{
  size_t len = strlen (msg);
  int ret;

  if (len < buflen)
    {
      /* Although POSIX allows memcpy() to corrupt errno, we don't
         know of any implementation where this is a real problem.  */
      memcpy (buf, msg, len + 1);
      ret = 0;
    }
  else
    {
      memcpy (buf, msg, buflen - 1);
      buf[buflen - 1] = '\0';
      ret = ERANGE;
    }
  return ret;
}


int
strerror_r (int errnum, char *buf, size_t buflen)
#undef strerror_r
{
  /* Filter this out now, so that rest of this replacement knows that
     there is room for a non-empty message and trailing NUL.  */
  if (buflen <= 1)
    {
      if (buflen)
        *buf = '\0';
      return ERANGE;
    }
  *buf = '\0';

#if GNULIB_defined_ETXTBSY \
    || GNULIB_defined_ESOCK \
    || GNULIB_defined_ENOMSG \
    || GNULIB_defined_EIDRM \
    || GNULIB_defined_ENOLINK \
    || GNULIB_defined_EPROTO \
    || GNULIB_defined_EMULTIHOP \
    || GNULIB_defined_EBADMSG \
    || GNULIB_defined_EOVERFLOW \
    || GNULIB_defined_ENOTSUP \
    || GNULIB_defined_ESTALE \
    || GNULIB_defined_EDQUOT \
    || GNULIB_defined_ECANCELED
  {
    char const *msg = NULL;
    /* These error messages are taken from glibc/sysdeps/gnu/errlist.c.  */
    switch (errnum)
      {
# if GNULIB_defined_ETXTBSY
      case ETXTBSY:
        msg = "Text file busy";
        break;
# endif

# if GNULIB_defined_ESOCK /* native Windows platforms */
      /* EWOULDBLOCK is the same as EAGAIN.  */
      case EINPROGRESS:
        msg = "Operation now in progress";
        break;
      case EALREADY:
        msg = "Operation already in progress";
        break;
      case ENOTSOCK:
        msg = "Socket operation on non-socket";
        break;
      case EDESTADDRREQ:
        msg = "Destination address required";
        break;
      case EMSGSIZE:
        msg = "Message too long";
        break;
      case EPROTOTYPE:
        msg = "Protocol wrong type for socket";
        break;
      case ENOPROTOOPT:
        msg = "Protocol not available";
        break;
      case EPROTONOSUPPORT:
        msg = "Protocol not supported";
        break;
      case ESOCKTNOSUPPORT:
        msg = "Socket type not supported";
        break;
      case EOPNOTSUPP:
        msg = "Operation not supported";
        break;
      case EPFNOSUPPORT:
        msg = "Protocol family not supported";
        break;
      case EAFNOSUPPORT:
        msg = "Address family not supported by protocol";
        break;
      case EADDRINUSE:
        msg = "Address already in use";
        break;
      case EADDRNOTAVAIL:
        msg = "Cannot assign requested address";
        break;
      case ENETDOWN:
        msg = "Network is down";
        break;
      case ENETUNREACH:
        msg = "Network is unreachable";
        break;
      case ENETRESET:
        msg = "Network dropped connection on reset";
        break;
      case ECONNABORTED:
        msg = "Software caused connection abort";
        break;
      case ECONNRESET:
        msg = "Connection reset by peer";
        break;
      case ENOBUFS:
        msg = "No buffer space available";
        break;
      case EISCONN:
        msg = "Transport endpoint is already connected";
        break;
      case ENOTCONN:
        msg = "Transport endpoint is not connected";
        break;
      case ESHUTDOWN:
        msg = "Cannot send after transport endpoint shutdown";
        break;
      case ETOOMANYREFS:
        msg = "Too many references: cannot splice";
        break;
      case ETIMEDOUT:
        msg = "Connection timed out";
        break;
      case ECONNREFUSED:
        msg = "Connection refused";
        break;
      case ELOOP:
        msg = "Too many levels of symbolic links";
        break;
      case EHOSTDOWN:
        msg = "Host is down";
        break;
      case EHOSTUNREACH:
        msg = "No route to host";
        break;
      case EPROCLIM:
        msg = "Too many processes";
        break;
      case EUSERS:
        msg = "Too many users";
        break;
      case EDQUOT:
        msg = "Disk quota exceeded";
        break;
      case ESTALE:
        msg = "Stale NFS file handle";
        break;
      case EREMOTE:
        msg = "Object is remote";
        break;
#  if HAVE_WINSOCK2_H
      /* WSA_INVALID_HANDLE maps to EBADF */
      /* WSA_NOT_ENOUGH_MEMORY maps to ENOMEM */
      /* WSA_INVALID_PARAMETER maps to EINVAL */
      case WSA_OPERATION_ABORTED:
        msg = "Overlapped operation aborted";
        break;
      case WSA_IO_INCOMPLETE:
        msg = "Overlapped I/O event object not in signaled state";
        break;
      case WSA_IO_PENDING:
        msg = "Overlapped operations will complete later";
        break;
      /* WSAEINTR maps to EINTR */
      /* WSAEBADF maps to EBADF */
      /* WSAEACCES maps to EACCES */
      /* WSAEFAULT maps to EFAULT */
      /* WSAEINVAL maps to EINVAL */
      /* WSAEMFILE maps to EMFILE */
      /* WSAEWOULDBLOCK maps to EWOULDBLOCK */
      /* WSAEINPROGRESS is EINPROGRESS */
      /* WSAEALREADY is EALREADY */
      /* WSAENOTSOCK is ENOTSOCK */
      /* WSAEDESTADDRREQ is EDESTADDRREQ */
      /* WSAEMSGSIZE is EMSGSIZE */
      /* WSAEPROTOTYPE is EPROTOTYPE */
      /* WSAENOPROTOOPT is ENOPROTOOPT */
      /* WSAEPROTONOSUPPORT is EPROTONOSUPPORT */
      /* WSAESOCKTNOSUPPORT is ESOCKTNOSUPPORT */
      /* WSAEOPNOTSUPP is EOPNOTSUPP */
      /* WSAEPFNOSUPPORT is EPFNOSUPPORT */
      /* WSAEAFNOSUPPORT is EAFNOSUPPORT */
      /* WSAEADDRINUSE is EADDRINUSE */
      /* WSAEADDRNOTAVAIL is EADDRNOTAVAIL */
      /* WSAENETDOWN is ENETDOWN */
      /* WSAENETUNREACH is ENETUNREACH */
      /* WSAENETRESET is ENETRESET */
      /* WSAECONNABORTED is ECONNABORTED */
      /* WSAECONNRESET is ECONNRESET */
      /* WSAENOBUFS is ENOBUFS */
      /* WSAEISCONN is EISCONN */
      /* WSAENOTCONN is ENOTCONN */
      /* WSAESHUTDOWN is ESHUTDOWN */
      /* WSAETOOMANYREFS is ETOOMANYREFS */
      /* WSAETIMEDOUT is ETIMEDOUT */
      /* WSAECONNREFUSED is ECONNREFUSED */
      /* WSAELOOP is ELOOP */
      /* WSAENAMETOOLONG maps to ENAMETOOLONG */
      /* WSAEHOSTDOWN is EHOSTDOWN */
      /* WSAEHOSTUNREACH is EHOSTUNREACH */
      /* WSAENOTEMPTY maps to ENOTEMPTY */
      /* WSAEPROCLIM is EPROCLIM */
      /* WSAEUSERS is EUSERS */
      /* WSAEDQUOT is EDQUOT */
      /* WSAESTALE is ESTALE */
      /* WSAEREMOTE is EREMOTE */
      case WSASYSNOTREADY:
        msg = "Network subsystem is unavailable";
        break;
      case WSAVERNOTSUPPORTED:
        msg = "Winsock.dll version out of range";
        break;
      case WSANOTINITIALISED:
        msg = "Successful WSAStartup not yet performed";
        break;
      case WSAEDISCON:
        msg = "Graceful shutdown in progress";
        break;
      case WSAENOMORE: case WSA_E_NO_MORE:
        msg = "No more results";
        break;
      case WSAECANCELLED: case WSA_E_CANCELLED:
        msg = "Call was canceled";
        break;
      case WSAEINVALIDPROCTABLE:
        msg = "Procedure call table is invalid";
        break;
      case WSAEINVALIDPROVIDER:
        msg = "Service provider is invalid";
        break;
      case WSAEPROVIDERFAILEDINIT:
        msg = "Service provider failed to initialize";
        break;
      case WSASYSCALLFAILURE:
        msg = "System call failure";
        break;
      case WSASERVICE_NOT_FOUND:
        msg = "Service not found";
        break;
      case WSATYPE_NOT_FOUND:
        msg = "Class type not found";
        break;
      case WSAEREFUSED:
        msg = "Database query was refused";
        break;
      case WSAHOST_NOT_FOUND:
        msg = "Host not found";
        break;
      case WSATRY_AGAIN:
        msg = "Nonauthoritative host not found";
        break;
      case WSANO_RECOVERY:
        msg = "Nonrecoverable error";
        break;
      case WSANO_DATA:
        msg = "Valid name, no data record of requested type";
        break;
      /* WSA_QOS_* omitted */
#  endif
# endif

# if GNULIB_defined_ENOMSG
      case ENOMSG:
        msg = "No message of desired type";
        break;
# endif

# if GNULIB_defined_EIDRM
      case EIDRM:
        msg = "Identifier removed";
        break;
# endif

# if GNULIB_defined_ENOLINK
      case ENOLINK:
        msg = "Link has been severed";
        break;
# endif

# if GNULIB_defined_EPROTO
      case EPROTO:
        msg = "Protocol error";
        break;
# endif

# if GNULIB_defined_EMULTIHOP
      case EMULTIHOP:
        msg = "Multihop attempted";
        break;
# endif

# if GNULIB_defined_EBADMSG
      case EBADMSG:
        msg = "Bad message";
        break;
# endif

# if GNULIB_defined_EOVERFLOW
      case EOVERFLOW:
        msg = "Value too large for defined data type";
        break;
# endif

# if GNULIB_defined_ENOTSUP
      case ENOTSUP:
        msg = "Not supported";
        break;
# endif

# if GNULIB_defined_ESTALE
      case ESTALE:
        msg = "Stale NFS file handle";
        break;
# endif

# if GNULIB_defined_EDQUOT
      case EDQUOT:
        msg = "Disk quota exceeded";
        break;
# endif

# if GNULIB_defined_ECANCELED
      case ECANCELED:
        msg = "Operation canceled";
        break;
# endif
      }

    if (msg)
      return safe_copy (buf, buflen, msg);
  }
#endif

  {
    int ret;
    int saved_errno = errno;

#if USE_XPG_STRERROR_R

    {
      extern int __xpg_strerror_r (int errnum, char *buf, size_t buflen);

      ret = __xpg_strerror_r (errnum, buf, buflen);
      if (ret < 0)
        ret = errno;
      if (!*buf)
        {
          /* glibc 2.13 would not touch buf on err, so we have to fall
             back to GNU strerror_r which always returns a thread-safe
             untruncated string to (partially) copy into our buf.  */
          safe_copy (buf, buflen, strerror_r (errnum, buf, buflen));
        }
    }

#elif USE_SYSTEM_STRERROR_R

    if (buflen > INT_MAX)
      buflen = INT_MAX;

# ifdef __hpux
    /* On HP-UX 11.31, strerror_r always fails when buflen < 80; it
       also fails to change buf on EINVAL.  */
    {
      char stackbuf[80];

      if (buflen < sizeof stackbuf)
        {
          ret = strerror_r (errnum, stackbuf, sizeof stackbuf);
          if (ret == 0)
            ret = safe_copy (buf, buflen, stackbuf);
        }
      else
        ret = strerror_r (errnum, buf, buflen);
    }
# else
    /* Solaris 10 does not populate buf on ERANGE.  */
    ret = strerror_r (errnum, buf, buflen);
    if (ret == ERANGE && !*buf)
      {
        char stackbuf[STACKBUF_LEN];

        /* strerror-impl.h is also affected if our choice of stackbuf
           size is not large enough.  */
        if (strerror_r (errnum, stackbuf, sizeof stackbuf) == ERANGE)
          abort ();
        safe_copy (buf, buflen, stackbuf);
      }
# endif

# ifdef _AIX
    /* AIX returns 0 rather than ERANGE when truncating strings; try
       again until we are sure we got the entire string.  */
    if (!ret && strlen (buf) == buflen - 1)
      {
        char stackbuf[STACKBUF_LEN];
        size_t len;
        strerror_r (errnum, stackbuf, sizeof stackbuf);
        len = strlen (stackbuf);
        /* stackbuf should have been large enough.  */
        if (len + 1 == sizeof stackbuf)
          abort ();
        if (buflen <= len)
          ret = ERANGE;
      }
# endif

    /* Some old implementations may return (-1, EINVAL) instead of EINVAL.  */
    if (ret < 0)
      ret = errno;

    /* FreeBSD rejects 0; see http://austingroupbugs.net/view.php?id=382.  */
    if (errnum == 0 && ret == EINVAL)
      ret = safe_copy (buf, buflen, "Success");

#else /* USE_SYSTEM_STRERROR */

    /* Try to do what strerror (errnum) does, but without clobbering the
       buffer used by strerror().  */

# if defined __NetBSD__ || defined __hpux || ((defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__) || defined __CYGWIN__ /* NetBSD, HP-UX, native Win32, Cygwin */

    /* NetBSD:        sys_nerr, sys_errlist are declared through _NETBSD_SOURCE
                      and <errno.h> above.
       HP-UX:         sys_nerr, sys_errlist are declared explicitly above.
       native Win32:  sys_nerr, sys_errlist are declared in <stdlib.h>.
       Cygwin:        sys_nerr, sys_errlist are declared in <errno.h>.  */
    if (errnum >= 0 && errnum < sys_nerr)
      {
#  if HAVE_CATGETS && (defined __NetBSD__ || defined __hpux)
#   if defined __NetBSD__
        nl_catd catd = catopen ("libc", NL_CAT_LOCALE);
        const char *errmsg =
          (catd != (nl_catd)-1
           ? catgets (catd, 1, errnum, sys_errlist[errnum])
           : sys_errlist[errnum]);
#   endif
#   if defined __hpux
        nl_catd catd = catopen ("perror", NL_CAT_LOCALE);
        const char *errmsg =
          (catd != (nl_catd)-1
           ? catgets (catd, 1, 1 + errnum, sys_errlist[errnum])
           : sys_errlist[errnum]);
#   endif
#  else
        const char *errmsg = sys_errlist[errnum];
#  endif
        if (errmsg == NULL || *errmsg == '\0')
          ret = EINVAL;
        else
          ret = safe_copy (buf, buflen, errmsg);
#  if HAVE_CATGETS && (defined __NetBSD__ || defined __hpux)
        if (catd != (nl_catd)-1)
          catclose (catd);
#  endif
      }
    else
      ret = EINVAL;

# elif defined __sgi || (defined __sun && !defined _LP64) /* IRIX, Solaris <= 9 32-bit */

    /* For a valid error number, the system's strerror() function returns
       a pointer to a not copied string, not to a buffer.  */
    if (errnum >= 0 && errnum < sys_nerr)
      {
        char *errmsg = strerror (errnum);

        if (errmsg == NULL || *errmsg == '\0')
          ret = EINVAL;
        else
          ret = safe_copy (buf, buflen, errmsg);
      }
    else
      ret = EINVAL;

# else

    gl_lock_lock (strerror_lock);

    {
      char *errmsg = strerror (errnum);

      /* For invalid error numbers, strerror() on
           - IRIX 6.5 returns NULL,
           - HP-UX 11 returns an empty string.  */
      if (errmsg == NULL || *errmsg == '\0')
        ret = EINVAL;
      else
        ret = safe_copy (buf, buflen, errmsg);
    }

    gl_lock_unlock (strerror_lock);

# endif

#endif

    if (ret == EINVAL && !*buf)
      snprintf (buf, buflen, "Unknown error %d", errnum);

    errno = saved_errno;
    return ret;
  }
}
