/* Iteration over all valid error numbers.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include "errno-iter.h"

#include <errno.h>

void
errno_iterate (errno_iterate_callback_fn callback, void *data)
{
  /* Keep this list in sync with strerrorname_np.c !  */

  /* Error codes specified by ISO C.  */
  if (callback (data, EDOM)) return;
  if (callback (data, EILSEQ)) return;
  if (callback (data, ERANGE)) return;

  /* Error codes specified by POSIX.
     <https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/errno.h.html>  */
  #if defined E2BIG
  if (callback (data, E2BIG)) return;
  #endif
  #if defined EACCES
  if (callback (data, EACCES)) return;
  #endif
  #if defined EADDRINUSE
  if (callback (data, EADDRINUSE)) return;
  #endif
  #if defined EADDRNOTAVAIL
  if (callback (data, EADDRNOTAVAIL)) return;
  #endif
  #if defined EAFNOSUPPORT
  if (callback (data, EAFNOSUPPORT)) return;
  #endif
  #if defined EAGAIN
  if (callback (data, EAGAIN)) return;
  #endif
  #if defined EALREADY
  if (callback (data, EALREADY)) return;
  #endif
  #if defined EBADF
  if (callback (data, EBADF)) return;
  #endif
  #if defined EBADMSG
  if (callback (data, EBADMSG)) return;
  #endif
  #if defined EBUSY
  if (callback (data, EBUSY)) return;
  #endif
  #if defined ECANCELED
  if (callback (data, ECANCELED)) return;
  #endif
  #if defined ECHILD
  if (callback (data, ECHILD)) return;
  #endif
  #if defined ECONNABORTED
  if (callback (data, ECONNABORTED)) return;
  #endif
  #if defined ECONNREFUSED
  if (callback (data, ECONNREFUSED)) return;
  #endif
  #if defined ECONNRESET
  if (callback (data, ECONNRESET)) return;
  #endif
  #if defined EDEADLK
  if (callback (data, EDEADLK)) return;
  #endif
  #if defined EDESTADDRREQ
  if (callback (data, EDESTADDRREQ)) return;
  #endif
  #if defined EDQUOT
  if (callback (data, EDQUOT)) return;
  #endif
  #if defined EEXIST
  if (callback (data, EEXIST)) return;
  #endif
  #if defined EFAULT
  if (callback (data, EFAULT)) return;
  #endif
  #if defined EFBIG
  if (callback (data, EFBIG)) return;
  #endif
  #if defined EHOSTUNREACH
  if (callback (data, EHOSTUNREACH)) return;
  #endif
  #if defined EIDRM
  if (callback (data, EIDRM)) return;
  #endif
  #if defined EINPROGRESS
  if (callback (data, EINPROGRESS)) return;
  #endif
  #if defined EINTR
  if (callback (data, EINTR)) return;
  #endif
  #if defined EINVAL
  if (callback (data, EINVAL)) return;
  #endif
  #if defined EIO
  if (callback (data, EIO)) return;
  #endif
  #if defined EISCONN
  if (callback (data, EISCONN)) return;
  #endif
  #if defined EISDIR
  if (callback (data, EISDIR)) return;
  #endif
  #if defined ELOOP
  if (callback (data, ELOOP)) return;
  #endif
  #if defined EMFILE
  if (callback (data, EMFILE)) return;
  #endif
  #if defined EMLINK
  if (callback (data, EMLINK)) return;
  #endif
  #if defined EMSGSIZE
  if (callback (data, EMSGSIZE)) return;
  #endif
  #if defined EMULTIHOP
  if (callback (data, EMULTIHOP)) return;
  #endif
  #if defined ENAMETOOLONG
  if (callback (data, ENAMETOOLONG)) return;
  #endif
  #if defined ENETDOWN
  if (callback (data, ENETDOWN)) return;
  #endif
  #if defined ENETRESET
  if (callback (data, ENETRESET)) return;
  #endif
  #if defined ENETUNREACH
  if (callback (data, ENETUNREACH)) return;
  #endif
  #if defined ENFILE
  if (callback (data, ENFILE)) return;
  #endif
  #if defined ENOBUFS
  if (callback (data, ENOBUFS)) return;
  #endif
  #if defined ENODATA
  if (callback (data, ENODATA)) return;
  #endif
  #if defined ENODEV
  if (callback (data, ENODEV)) return;
  #endif
  #if defined ENOENT
  if (callback (data, ENOENT)) return;
  #endif
  #if defined ENOEXEC
  if (callback (data, ENOEXEC)) return;
  #endif
  #if defined ENOLCK
  if (callback (data, ENOLCK)) return;
  #endif
  #if defined ENOLINK
  if (callback (data, ENOLINK)) return;
  #endif
  #if defined ENOMEM
  if (callback (data, ENOMEM)) return;
  #endif
  #if defined ENOMSG
  if (callback (data, ENOMSG)) return;
  #endif
  #if defined ENOPROTOOPT
  if (callback (data, ENOPROTOOPT)) return;
  #endif
  #if defined ENOSPC
  if (callback (data, ENOSPC)) return;
  #endif
  #if defined ENOSR
  if (callback (data, ENOSR)) return;
  #endif
  #if defined ENOSTR
  if (callback (data, ENOSTR)) return;
  #endif
  #if defined ENOSYS
  if (callback (data, ENOSYS)) return;
  #endif
  #if defined ENOTCONN
  if (callback (data, ENOTCONN)) return;
  #endif
  #if defined ENOTDIR
  if (callback (data, ENOTDIR)) return;
  #endif
  #if defined ENOTEMPTY && ENOTEMPTY != EEXIST
  if (callback (data, ENOTEMPTY)) return;
  #endif
  #if defined ENOTRECOVERABLE
  if (callback (data, ENOTRECOVERABLE)) return;
  #endif
  #if defined ENOTSOCK
  if (callback (data, ENOTSOCK)) return;
  #endif
  #if defined ENOTSUP && ENOTSUP != EOPNOTSUPP
  if (callback (data, ENOTSUP)) return;
  #endif
  #if defined ENOTTY
  if (callback (data, ENOTTY)) return;
  #endif
  #if defined ENXIO
  if (callback (data, ENXIO)) return;
  #endif
  #if defined EOPNOTSUPP
  if (callback (data, EOPNOTSUPP)) return;
  #endif
  #if defined EOVERFLOW
  if (callback (data, EOVERFLOW)) return;
  #endif
  #if defined EOWNERDEAD
  if (callback (data, EOWNERDEAD)) return;
  #endif
  #if defined EPERM
  if (callback (data, EPERM)) return;
  #endif
  #if defined EPIPE
  if (callback (data, EPIPE)) return;
  #endif
  #if defined EPROTO
  if (callback (data, EPROTO)) return;
  #endif
  #if defined EPROTONOSUPPORT
  if (callback (data, EPROTONOSUPPORT)) return;
  #endif
  #if defined EPROTOTYPE
  if (callback (data, EPROTOTYPE)) return;
  #endif
  #if defined EROFS
  if (callback (data, EROFS)) return;
  #endif
  #if defined ESOCKTNOSUPPORT
  if (callback (data, ESOCKTNOSUPPORT)) return;
  #endif
  #if defined ESPIPE
  if (callback (data, ESPIPE)) return;
  #endif
  #if defined ESRCH
  if (callback (data, ESRCH)) return;
  #endif
  #if defined ESTALE
  if (callback (data, ESTALE)) return;
  #endif
  #if defined ETIME
  if (callback (data, ETIME)) return;
  #endif
  #if defined ETIMEDOUT
  if (callback (data, ETIMEDOUT)) return;
  #endif
  #if defined ETXTBSY
  if (callback (data, ETXTBSY)) return;
  #endif
  #if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
  if (callback (data, EWOULDBLOCK)) return;
  #endif
  #if defined EXDEV
  if (callback (data, EXDEV)) return;
  #endif

  /* Other error codes on other systems.  */
  /* Solaris */
  #if defined EADI
  if (callback (data, EADI)) return;
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined EADV
  if (callback (data, EADV)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EAUTH
  if (callback (data, EAUTH)) return;
  #endif
  /* GNU/Hurd */
  #if defined EBACKGROUND
  if (callback (data, EBACKGROUND)) return;
  #endif
  /* Mac OS X */
  #if defined EBADARCH
  if (callback (data, EBADARCH)) return;
  #endif
  /* Minix */
  #if defined EBADCALL
  if (callback (data, EBADCALL)) return;
  #endif
  /* Minix */
  #if defined EBADCPU
  if (callback (data, EBADCPU)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADE && EBADE != ECKSUM
  if (callback (data, EBADE)) return;
  #endif
  /* Minix */
  #if defined EBADEPT
  if (callback (data, EBADEPT)) return;
  #endif
  /* Mac OS X */
  #if defined EBADEXEC
  if (callback (data, EBADEXEC)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADFD
  if (callback (data, EBADFD)) return;
  #endif
  /* Minix */
  #if defined EBADIOCTL
  if (callback (data, EBADIOCTL)) return;
  #endif
  /* Mac OS X */
  #if defined EBADMACHO
  if (callback (data, EBADMACHO)) return;
  #endif
  /* Minix */
  #if defined EBADMODE
  if (callback (data, EBADMODE)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADR && EBADR != EFRAGS
  if (callback (data, EBADR)) return;
  #endif
  /* Minix */
  #if defined EBADREQUEST
  if (callback (data, EBADREQUEST)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EBADRPC
  if (callback (data, EBADRPC)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADRQC
  if (callback (data, EBADRQC)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADSLT
  if (callback (data, EBADSLT)) return;
  #endif
  /* HP-UX */
  #if defined EBADVER
  if (callback (data, EBADVER)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBFONT
  if (callback (data, EBFONT)) return;
  #endif
  /* Minix */
  #if defined ECALLDENIED
  if (callback (data, ECALLDENIED)) return;
  #endif
  /* FreeBSD */
  #if defined ECAPMODE
  if (callback (data, ECAPMODE)) return;
  #endif
  /* Cygwin */
  #if defined ECASECLASH
  if (callback (data, ECASECLASH)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ECHRNG
  if (callback (data, ECHRNG)) return;
  #endif
  /* Solaris */
  #if defined ECKSUM
  if (callback (data, ECKSUM)) return;
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ECOMM
  if (callback (data, ECOMM)) return;
  #endif
  /* HP-UX */
  #if defined ECONFIG
  if (callback (data, ECONFIG)) return;
  #endif
  /* AIX */
  #if defined ECORRUPT
  if (callback (data, ECORRUPT)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED
  if (callback (data, ED)) return;
  #endif
  /* Minix */
  #if defined EDEADEPT
  if (callback (data, EDEADEPT)) return;
  #endif
  /* Solaris, Cygwin */
  #if defined EDEADLOCK && EDEADLOCK != EDEADLK
  if (callback (data, EDEADLOCK)) return;
  #endif
  /* Minix */
  #if defined EDEADSRCDST
  if (callback (data, EDEADSRCDST)) return;
  #endif
  /* Mac OS X */
  #if defined EDEVERR
  if (callback (data, EDEVERR)) return;
  #endif
  /* GNU/Hurd */
  #if defined EDIED
  if (callback (data, EDIED)) return;
  #endif
  /* FreeBSD */
  #if defined EDIRIOCTL
  if (callback (data, EDIRIOCTL)) return;
  #endif
  /* AIX */
  #if defined EDIST
  if (callback (data, EDIST)) return;
  #endif
  /* Minix */
  #if defined EDONTREPLY
  if (callback (data, EDONTREPLY)) return;
  #endif
  /* FreeBSD */
  #if defined EDOOFUS && EDOOFUS != EINVAL
  if (callback (data, EDOOFUS)) return;
  #endif
  /* Linux, HP-UX, Cygwin */
  #if defined EDOTDOT
  if (callback (data, EDOTDOT)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_ALREADY_OPEN
  if (callback (data, ED_ALREADY_OPEN)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_DEVICE_DOWN
  if (callback (data, ED_DEVICE_DOWN)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_OPERATION
  if (callback (data, ED_INVALID_OPERATION)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_RECNUM
  if (callback (data, ED_INVALID_RECNUM)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_SIZE
  if (callback (data, ED_INVALID_SIZE)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_IO_ERROR
  if (callback (data, ED_IO_ERROR)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_NO_MEMORY
  if (callback (data, ED_NO_MEMORY)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_NO_SUCH_DEVICE
  if (callback (data, ED_NO_SUCH_DEVICE)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_READ_ONLY
  if (callback (data, ED_READ_ONLY)) return;
  #endif
  /* GNU/Hurd */
  #if defined ED_WOULD_BLOCK
  if (callback (data, ED_WOULD_BLOCK)) return;
  #endif
  /* AIX */
  #if defined EFORMAT
  if (callback (data, EFORMAT)) return;
  #endif
  /* Haiku */
  #if defined EFPOS
  if (callback (data, EFPOS)) return;
  #endif
  /* Solaris */
  #if defined EFRAGS
  if (callback (data, EFRAGS)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix, Cygwin */
  #if defined EFTYPE
  if (callback (data, EFTYPE)) return;
  #endif
  /* Minix */
  #if defined EGENERIC
  if (callback (data, EGENERIC)) return;
  #endif
  /* GNU/Hurd */
  #if defined EGRATUITOUS
  if (callback (data, EGRATUITOUS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EGREGIOUS
  if (callback (data, EGREGIOUS)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EHOSTDOWN
  if (callback (data, EHOSTDOWN)) return;
  #endif
  /* Linux */
  #if defined EHWPOISON
  if (callback (data, EHWPOISON)) return;
  #endif
  /* GNU/Hurd */
  #if defined EIEIO
  if (callback (data, EIEIO)) return;
  #endif
  /* OpenBSD */
  #if defined EIPSEC
  if (callback (data, EIPSEC)) return;
  #endif
  /* Linux */
  #if defined EISNAM
  if (callback (data, EISNAM)) return;
  #endif
  /* FreeBSD */
  #if defined EJUSTRETURN
  if (callback (data, EJUSTRETURN)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_ABORTED
  if (callback (data, EKERN_ABORTED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_FAILURE
  if (callback (data, EKERN_FAILURE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INTERRUPTED
  if (callback (data, EKERN_INTERRUPTED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_ADDRESS
  if (callback (data, EKERN_INVALID_ADDRESS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_ARGUMENT
  if (callback (data, EKERN_INVALID_ARGUMENT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_CAPABILITY
  if (callback (data, EKERN_INVALID_CAPABILITY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_HOST
  if (callback (data, EKERN_INVALID_HOST)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_NAME
  if (callback (data, EKERN_INVALID_NAME)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_RIGHT
  if (callback (data, EKERN_INVALID_RIGHT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_TASK
  if (callback (data, EKERN_INVALID_TASK)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_VALUE
  if (callback (data, EKERN_INVALID_VALUE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_ERROR
  if (callback (data, EKERN_MEMORY_ERROR)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_FAILURE
  if (callback (data, EKERN_MEMORY_FAILURE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_PRESENT
  if (callback (data, EKERN_MEMORY_PRESENT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NAME_EXISTS
  if (callback (data, EKERN_NAME_EXISTS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NOT_IN_SET
  if (callback (data, EKERN_NOT_IN_SET)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NOT_RECEIVER
  if (callback (data, EKERN_NOT_RECEIVER)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NO_ACCESS
  if (callback (data, EKERN_NO_ACCESS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NO_SPACE
  if (callback (data, EKERN_NO_SPACE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_PROTECTION_FAILURE
  if (callback (data, EKERN_PROTECTION_FAILURE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_RESOURCE_SHORTAGE
  if (callback (data, EKERN_RESOURCE_SHORTAGE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_RIGHT_EXISTS
  if (callback (data, EKERN_RIGHT_EXISTS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_TERMINATED
  if (callback (data, EKERN_TERMINATED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_TIMEDOUT
  if (callback (data, EKERN_TIMEDOUT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_UREFS_OVERFLOW
  if (callback (data, EKERN_UREFS_OVERFLOW)) return;
  #endif
  /* GNU/Hurd */
  #if defined EKERN_WRITE_PROTECTION_FAILURE
  if (callback (data, EKERN_WRITE_PROTECTION_FAILURE)) return;
  #endif
  /* Linux */
  #if defined EKEYEXPIRED
  if (callback (data, EKEYEXPIRED)) return;
  #endif
  /* Linux */
  #if defined EKEYREJECTED
  if (callback (data, EKEYREJECTED)) return;
  #endif
  /* Linux */
  #if defined EKEYREVOKED
  if (callback (data, EKEYREVOKED)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL2HLT
  if (callback (data, EL2HLT)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL2NSYNC
  if (callback (data, EL2NSYNC)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL3HLT
  if (callback (data, EL3HLT)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL3RST
  if (callback (data, EL3RST)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ELAST && 0
  if (callback (data, ELAST)) return;
  #endif
  /* Cygwin */
  #if defined ELBIN
  if (callback (data, ELBIN)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBACC
  if (callback (data, ELIBACC)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBBAD
  if (callback (data, ELIBBAD)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBEXEC
  if (callback (data, ELIBEXEC)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBMAX
  if (callback (data, ELIBMAX)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBSCN
  if (callback (data, ELIBSCN)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ELNRNG
  if (callback (data, ELNRNG)) return;
  #endif
  /* Minix */
  #if defined ELOCKED
  if (callback (data, ELOCKED)) return;
  #endif
  /* Solaris */
  #if defined ELOCKUNMAPPED
  if (callback (data, ELOCKUNMAPPED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_BODY_ERROR
  if (callback (data, EMACH_RCV_BODY_ERROR)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_HEADER_ERROR
  if (callback (data, EMACH_RCV_HEADER_ERROR)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INTERRUPTED
  if (callback (data, EMACH_RCV_INTERRUPTED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_DATA
  if (callback (data, EMACH_RCV_INVALID_DATA)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_NAME
  if (callback (data, EMACH_RCV_INVALID_NAME)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_NOTIFY
  if (callback (data, EMACH_RCV_INVALID_NOTIFY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_IN_PROGRESS
  if (callback (data, EMACH_RCV_IN_PROGRESS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_IN_SET
  if (callback (data, EMACH_RCV_IN_SET)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_PORT_CHANGED
  if (callback (data, EMACH_RCV_PORT_CHANGED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_PORT_DIED
  if (callback (data, EMACH_RCV_PORT_DIED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_TIMED_OUT
  if (callback (data, EMACH_RCV_TIMED_OUT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_TOO_LARGE
  if (callback (data, EMACH_RCV_TOO_LARGE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INTERRUPTED
  if (callback (data, EMACH_SEND_INTERRUPTED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_DATA
  if (callback (data, EMACH_SEND_INVALID_DATA)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_DEST
  if (callback (data, EMACH_SEND_INVALID_DEST)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_HEADER
  if (callback (data, EMACH_SEND_INVALID_HEADER)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_MEMORY
  if (callback (data, EMACH_SEND_INVALID_MEMORY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_NOTIFY
  if (callback (data, EMACH_SEND_INVALID_NOTIFY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_REPLY
  if (callback (data, EMACH_SEND_INVALID_REPLY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_RIGHT
  if (callback (data, EMACH_SEND_INVALID_RIGHT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_TYPE
  if (callback (data, EMACH_SEND_INVALID_TYPE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_IN_PROGRESS
  if (callback (data, EMACH_SEND_IN_PROGRESS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_MSG_TOO_SMALL
  if (callback (data, EMACH_SEND_MSG_TOO_SMALL)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NOTIFY_IN_PROGRESS
  if (callback (data, EMACH_SEND_NOTIFY_IN_PROGRESS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NO_BUFFER
  if (callback (data, EMACH_SEND_NO_BUFFER)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NO_NOTIFY
  if (callback (data, EMACH_SEND_NO_NOTIFY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_TIMED_OUT
  if (callback (data, EMACH_SEND_TIMED_OUT)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_WILL_NOTIFY
  if (callback (data, EMACH_SEND_WILL_NOTIFY)) return;
  #endif
  /* AIX */
  #if defined EMEDIA
  if (callback (data, EMEDIA)) return;
  #endif
  /* Linux, OpenBSD */
  #if defined EMEDIUMTYPE
  if (callback (data, EMEDIUMTYPE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_ARRAY_TOO_LARGE
  if (callback (data, EMIG_ARRAY_TOO_LARGE)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_BAD_ARGUMENTS
  if (callback (data, EMIG_BAD_ARGUMENTS)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_BAD_ID
  if (callback (data, EMIG_BAD_ID)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_DESTROY_REQUEST
  if (callback (data, EMIG_DESTROY_REQUEST)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_EXCEPTION
  if (callback (data, EMIG_EXCEPTION)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_NO_REPLY
  if (callback (data, EMIG_NO_REPLY)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_REMOTE_ERROR
  if (callback (data, EMIG_REMOTE_ERROR)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_REPLY_MISMATCH
  if (callback (data, EMIG_REPLY_MISMATCH)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_SERVER_DIED
  if (callback (data, EMIG_SERVER_DIED)) return;
  #endif
  /* GNU/Hurd */
  #if defined EMIG_TYPE_ERROR
  if (callback (data, EMIG_TYPE_ERROR)) return;
  #endif
  /* Linux */
  #if defined ENAVAIL
  if (callback (data, ENAVAIL)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ENEEDAUTH
  if (callback (data, ENEEDAUTH)) return;
  #endif
  /* Cygwin */
  #if defined ENMFILE
  if (callback (data, ENMFILE)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ENOANO && ENOANO != ENOKEY
  if (callback (data, ENOANO)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, Minix */
  #if defined ENOATTR
  if (callback (data, ENOATTR)) return;
  #endif
  /* MirBSD */
  #if defined ENOCOFFEE
  if (callback (data, ENOCOFFEE)) return;
  #endif
  /* Minix */
  #if defined ENOCONN
  if (callback (data, ENOCONN)) return;
  #endif
  /* AIX */
  #if defined ENOCONNECT
  if (callback (data, ENOCONNECT)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ENOCSI
  if (callback (data, ENOCSI)) return;
  #endif
  /* FreeBSD */
  #if defined ENOIOCTL
  if (callback (data, ENOIOCTL)) return;
  #endif
  /* Linux */
  #if defined ENOKEY
  if (callback (data, ENOKEY)) return;
  #endif
  /* HP-UX */
  #if defined ENOLOAD
  if (callback (data, ENOLOAD)) return;
  #endif
  /* HP-UX */
  #if defined ENOMATCH
  if (callback (data, ENOMATCH)) return;
  #endif
  /* Linux, OpenBSD, Cygwin */
  #if defined ENOMEDIUM
  if (callback (data, ENOMEDIUM)) return;
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ENONET
  if (callback (data, ENONET)) return;
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ENOPKG
  if (callback (data, ENOPKG)) return;
  #endif
  /* Mac OS X */
  #if defined ENOPOLICY
  if (callback (data, ENOPOLICY)) return;
  #endif
  /* HP-UX */
  #if defined ENOREG
  if (callback (data, ENOREG)) return;
  #endif
  /* Cygwin */
  #if defined ENOSHARE
  if (callback (data, ENOSHARE)) return;
  #endif
  /* Linux, HP-UX */
  #if defined ENOSYM
  if (callback (data, ENOSYM)) return;
  #endif
  /* Solaris */
  #if defined ENOTACTIVE
  if (callback (data, ENOTACTIVE)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined ENOTBLK
  if (callback (data, ENOTBLK)) return;
  #endif
  /* FreeBSD */
  #if defined ENOTCAPABLE
  if (callback (data, ENOTCAPABLE)) return;
  #endif
  /* Linux */
  #if defined ENOTNAM
  if (callback (data, ENOTNAM)) return;
  #endif
  /* AIX, Minix */
  #if defined ENOTREADY
  if (callback (data, ENOTREADY)) return;
  #endif
  /* AIX */
  #if defined ENOTRUST
  if (callback (data, ENOTRUST)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ENOTUNIQ
  if (callback (data, ENOTUNIQ)) return;
  #endif
  /* HP-UX */
  #if defined ENOUNLD
  if (callback (data, ENOUNLD)) return;
  #endif
  /* HP-UX */
  #if defined ENOUNREG
  if (callback (data, ENOUNREG)) return;
  #endif
  /* Minix */
  #if defined ENOURG
  if (callback (data, ENOURG)) return;
  #endif
  /* native Windows */
  #if defined EOTHER
  if (callback (data, EOTHER)) return;
  #endif
  /* Minix */
  #if defined EPACKSIZE
  if (callback (data, EPACKSIZE)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EPFNOSUPPORT
  if (callback (data, EPFNOSUPPORT)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, Minix, Cygwin */
  #if defined EPROCLIM
  if (callback (data, EPROCLIM)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROCUNAVAIL
  if (callback (data, EPROCUNAVAIL)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROGMISMATCH
  if (callback (data, EPROGMISMATCH)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROGUNAVAIL
  if (callback (data, EPROGUNAVAIL)) return;
  #endif
  /* Mac OS X */
  #if defined EPWROFF
  if (callback (data, EPWROFF)) return;
  #endif
  /* Mac OS X */
  #if defined EQFULL
  if (callback (data, EQFULL)) return;
  #endif
  /* HP-UX */
  #if defined ERELOC
  if (callback (data, ERELOC)) return;
  #endif
  /* FreeBSD */
  #if defined ERELOOKUP
  if (callback (data, ERELOOKUP)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EREMCHG
  if (callback (data, EREMCHG)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EREMOTE
  if (callback (data, EREMOTE)) return;
  #endif
  /* Linux */
  #if defined EREMOTEIO
  if (callback (data, EREMOTEIO)) return;
  #endif
  /* Linux, HP-UX */
  #if defined EREMOTERELEASE
  if (callback (data, EREMOTERELEASE)) return;
  #endif
  /* Linux, FreeBSD, AIX, Solaris, Minix */
  #if defined ERESTART
  if (callback (data, ERESTART)) return;
  #endif
  /* Linux */
  #if defined ERFKILL
  if (callback (data, ERFKILL)) return;
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ERPCMISMATCH
  if (callback (data, ERPCMISMATCH)) return;
  #endif
  /* AIX */
  #if defined ESAD
  if (callback (data, ESAD)) return;
  #endif
  /* Mac OS X */
  #if defined ESHLIBVERS
  if (callback (data, ESHLIBVERS)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined ESHUTDOWN
  if (callback (data, ESHUTDOWN)) return;
  #endif
  /* Haiku */
  #if defined ESIGPARM
  if (callback (data, ESIGPARM)) return;
  #endif
  /* AIX */
  #if defined ESOFT
  if (callback (data, ESOFT)) return;
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ESRMNT
  if (callback (data, ESRMNT)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ESTRPIPE
  if (callback (data, ESTRPIPE)) return;
  #endif
  /* AIX */
  #if defined ESYSERROR
  if (callback (data, ESYSERROR)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined ETOOMANYREFS
  if (callback (data, ETOOMANYREFS)) return;
  #endif
  /* Minix */
  #if defined ETRAPDENIED
  if (callback (data, ETRAPDENIED)) return;
  #endif
  /* Linux */
  #if defined EUCLEAN
  if (callback (data, EUCLEAN)) return;
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EUNATCH
  if (callback (data, EUNATCH)) return;
  #endif
  /* Minix */
  #if defined EURG
  if (callback (data, EURG)) return;
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined EUSERS
  if (callback (data, EUSERS)) return;
  #endif
  /* AIX */
  #if defined EWRPROTECT
  if (callback (data, EWRPROTECT)) return;
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EXFULL
  if (callback (data, EXFULL)) return;
  #endif
}
