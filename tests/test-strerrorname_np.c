/* Test of strerrorname_np() function.

   Copyright (C) 2020-2026 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include "signature.h"
SIGNATURE_CHECK (strerrorname_np, const char *, (int));

#include <errno.h>

#include "macros.h"

int
main (void)
{
  /* Error codes specified by ISO C.  */
  ASSERT (streq (strerrorname_np (EDOM), "EDOM"));
  ASSERT (streq (strerrorname_np (EILSEQ), "EILSEQ"));
  ASSERT (streq (strerrorname_np (ERANGE), "ERANGE"));

  /* Error codes specified by POSIX.
     <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html>  */
  #if defined E2BIG
  ASSERT (streq (strerrorname_np (E2BIG), "E2BIG"));
  #endif
  #if defined EACCES
  ASSERT (streq (strerrorname_np (EACCES), "EACCES"));
  #endif
  #if defined EADDRINUSE
  ASSERT (streq (strerrorname_np (EADDRINUSE), "EADDRINUSE"));
  #endif
  #if defined EADDRNOTAVAIL
  ASSERT (streq (strerrorname_np (EADDRNOTAVAIL), "EADDRNOTAVAIL"));
  #endif
  #if defined EAFNOSUPPORT
  ASSERT (streq (strerrorname_np (EAFNOSUPPORT), "EAFNOSUPPORT"));
  #endif
  #if defined EAGAIN
  ASSERT (streq (strerrorname_np (EAGAIN), "EAGAIN"));
  #endif
  #if defined EALREADY
  ASSERT (streq (strerrorname_np (EALREADY), "EALREADY"));
  #endif
  #if defined EBADF
  ASSERT (streq (strerrorname_np (EBADF), "EBADF"));
  #endif
  #if defined EBADMSG
  ASSERT (streq (strerrorname_np (EBADMSG), "EBADMSG"));
  #endif
  #if defined EBUSY
  ASSERT (streq (strerrorname_np (EBUSY), "EBUSY"));
  #endif
  #if defined ECANCELED
  ASSERT (streq (strerrorname_np (ECANCELED), "ECANCELED"));
  #endif
  #if defined ECHILD
  ASSERT (streq (strerrorname_np (ECHILD), "ECHILD"));
  #endif
  #if defined ECONNABORTED
  ASSERT (streq (strerrorname_np (ECONNABORTED), "ECONNABORTED"));
  #endif
  #if defined ECONNREFUSED
  ASSERT (streq (strerrorname_np (ECONNREFUSED), "ECONNREFUSED"));
  #endif
  #if defined ECONNRESET
  ASSERT (streq (strerrorname_np (ECONNRESET), "ECONNRESET"));
  #endif
  #if defined EDEADLK
  ASSERT (streq (strerrorname_np (EDEADLK), "EDEADLK"));
  #endif
  #if defined EDESTADDRREQ
  ASSERT (streq (strerrorname_np (EDESTADDRREQ), "EDESTADDRREQ"));
  #endif
  #if defined EDQUOT
  ASSERT (streq (strerrorname_np (EDQUOT), "EDQUOT"));
  #endif
  #if defined EEXIST
  ASSERT (streq (strerrorname_np (EEXIST), "EEXIST"));
  #endif
  #if defined EFAULT
  ASSERT (streq (strerrorname_np (EFAULT), "EFAULT"));
  #endif
  #if defined EFBIG
  ASSERT (streq (strerrorname_np (EFBIG), "EFBIG"));
  #endif
  #if defined EHOSTUNREACH
  ASSERT (streq (strerrorname_np (EHOSTUNREACH), "EHOSTUNREACH"));
  #endif
  #if defined EIDRM
  ASSERT (streq (strerrorname_np (EIDRM), "EIDRM"));
  #endif
  #if defined EINPROGRESS
  ASSERT (streq (strerrorname_np (EINPROGRESS), "EINPROGRESS"));
  #endif
  #if defined EINTR
  ASSERT (streq (strerrorname_np (EINTR), "EINTR"));
  #endif
  #if defined EINVAL
  ASSERT (streq (strerrorname_np (EINVAL), "EINVAL"));
  #endif
  #if defined EIO
  ASSERT (streq (strerrorname_np (EIO), "EIO"));
  #endif
  #if defined EISCONN
  ASSERT (streq (strerrorname_np (EISCONN), "EISCONN"));
  #endif
  #if defined EISDIR
  ASSERT (streq (strerrorname_np (EISDIR), "EISDIR"));
  #endif
  #if defined ELOOP
  ASSERT (streq (strerrorname_np (ELOOP), "ELOOP"));
  #endif
  #if defined EMFILE
  ASSERT (streq (strerrorname_np (EMFILE), "EMFILE"));
  #endif
  #if defined EMLINK
  ASSERT (streq (strerrorname_np (EMLINK), "EMLINK"));
  #endif
  #if defined EMSGSIZE
  ASSERT (streq (strerrorname_np (EMSGSIZE), "EMSGSIZE"));
  #endif
  #if defined EMULTIHOP
  ASSERT (streq (strerrorname_np (EMULTIHOP), "EMULTIHOP"));
  #endif
  #if defined ENAMETOOLONG
  ASSERT (streq (strerrorname_np (ENAMETOOLONG), "ENAMETOOLONG"));
  #endif
  #if defined ENETDOWN
  ASSERT (streq (strerrorname_np (ENETDOWN), "ENETDOWN"));
  #endif
  #if defined ENETRESET
  ASSERT (streq (strerrorname_np (ENETRESET), "ENETRESET"));
  #endif
  #if defined ENETUNREACH
  ASSERT (streq (strerrorname_np (ENETUNREACH), "ENETUNREACH"));
  #endif
  #if defined ENFILE
  ASSERT (streq (strerrorname_np (ENFILE), "ENFILE"));
  #endif
  #if defined ENOBUFS
  ASSERT (streq (strerrorname_np (ENOBUFS), "ENOBUFS"));
  #endif
  #if defined ENODATA
  ASSERT (streq (strerrorname_np (ENODATA), "ENODATA"));
  #endif
  #if defined ENODEV
  ASSERT (streq (strerrorname_np (ENODEV), "ENODEV"));
  #endif
  #if defined ENOENT
  ASSERT (streq (strerrorname_np (ENOENT), "ENOENT"));
  #endif
  #if defined ENOEXEC
  ASSERT (streq (strerrorname_np (ENOEXEC), "ENOEXEC"));
  #endif
  #if defined ENOLCK
  ASSERT (streq (strerrorname_np (ENOLCK), "ENOLCK"));
  #endif
  #if defined ENOLINK
  ASSERT (streq (strerrorname_np (ENOLINK), "ENOLINK"));
  #endif
  #if defined ENOMEM
  ASSERT (streq (strerrorname_np (ENOMEM), "ENOMEM"));
  #endif
  #if defined ENOMSG
  ASSERT (streq (strerrorname_np (ENOMSG), "ENOMSG"));
  #endif
  #if defined ENOPROTOOPT
  ASSERT (streq (strerrorname_np (ENOPROTOOPT), "ENOPROTOOPT"));
  #endif
  #if defined ENOSPC
  ASSERT (streq (strerrorname_np (ENOSPC), "ENOSPC"));
  #endif
  #if defined ENOSR
  ASSERT (streq (strerrorname_np (ENOSR), "ENOSR"));
  #endif
  #if defined ENOSTR
  ASSERT (streq (strerrorname_np (ENOSTR), "ENOSTR"));
  #endif
  #if defined ENOSYS
  ASSERT (streq (strerrorname_np (ENOSYS), "ENOSYS"));
  #endif
  #if defined ENOTCONN
  ASSERT (streq (strerrorname_np (ENOTCONN), "ENOTCONN"));
  #endif
  #if defined ENOTDIR
  ASSERT (streq (strerrorname_np (ENOTDIR), "ENOTDIR"));
  #endif
  #if defined ENOTEMPTY && ENOTEMPTY != EEXIST
  ASSERT (streq (strerrorname_np (ENOTEMPTY), "ENOTEMPTY"));
  #endif
  #if defined ENOTRECOVERABLE
  ASSERT (streq (strerrorname_np (ENOTRECOVERABLE), "ENOTRECOVERABLE"));
  #endif
  #if defined ENOTSOCK
  ASSERT (streq (strerrorname_np (ENOTSOCK), "ENOTSOCK"));
  #endif
  #if defined ENOTSUP && ENOTSUP != EOPNOTSUPP
  ASSERT (streq (strerrorname_np (ENOTSUP), "ENOTSUP"));
  #endif
  #if defined ENOTTY
  ASSERT (streq (strerrorname_np (ENOTTY), "ENOTTY"));
  #endif
  #if defined ENXIO
  ASSERT (streq (strerrorname_np (ENXIO), "ENXIO"));
  #endif
  #if defined EOPNOTSUPP
  ASSERT (streq (strerrorname_np (EOPNOTSUPP), "EOPNOTSUPP"));
  #endif
  #if defined EOVERFLOW
  ASSERT (streq (strerrorname_np (EOVERFLOW), "EOVERFLOW"));
  #endif
  #if defined EOWNERDEAD
  ASSERT (streq (strerrorname_np (EOWNERDEAD), "EOWNERDEAD"));
  #endif
  #if defined EPERM
  ASSERT (streq (strerrorname_np (EPERM), "EPERM"));
  #endif
  #if defined EPIPE
  ASSERT (streq (strerrorname_np (EPIPE), "EPIPE"));
  #endif
  #if defined EPROTO
  ASSERT (streq (strerrorname_np (EPROTO), "EPROTO"));
  #endif
  #if defined EPROTONOSUPPORT
  ASSERT (streq (strerrorname_np (EPROTONOSUPPORT), "EPROTONOSUPPORT"));
  #endif
  #if defined EPROTOTYPE
  ASSERT (streq (strerrorname_np (EPROTOTYPE), "EPROTOTYPE"));
  #endif
  #if defined EROFS
  ASSERT (streq (strerrorname_np (EROFS), "EROFS"));
  #endif
  #if defined ESPIPE
  ASSERT (streq (strerrorname_np (ESPIPE), "ESPIPE"));
  #endif
  #if defined ESRCH
  ASSERT (streq (strerrorname_np (ESRCH), "ESRCH"));
  #endif
  #if defined ESTALE
  ASSERT (streq (strerrorname_np (ESTALE), "ESTALE"));
  #endif
  #if defined ETIME
  ASSERT (streq (strerrorname_np (ETIME), "ETIME"));
  #endif
  #if defined ETIMEDOUT
  ASSERT (streq (strerrorname_np (ETIMEDOUT), "ETIMEDOUT"));
  #endif
  #if defined ETXTBSY
  ASSERT (streq (strerrorname_np (ETXTBSY), "ETXTBSY"));
  #endif
  #if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
  ASSERT (streq (strerrorname_np (EWOULDBLOCK), "EWOULDBLOCK"));
  #endif
  #if defined EXDEV
  ASSERT (streq (strerrorname_np (EXDEV), "EXDEV"));
  #endif

  /* Other error codes on other systems.  */
  /* Solaris */
  #if defined EADI
  ASSERT (streq (strerrorname_np (EADI), "EADI"));
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined EADV
  ASSERT (streq (strerrorname_np (EADV), "EADV"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EAUTH
  ASSERT (streq (strerrorname_np (EAUTH), "EAUTH"));
  #endif
  /* GNU/Hurd */
  #if defined EBACKGROUND
  ASSERT (streq (strerrorname_np (EBACKGROUND), "EBACKGROUND"));
  #endif
  /* Mac OS X */
  #if defined EBADARCH
  ASSERT (streq (strerrorname_np (EBADARCH), "EBADARCH"));
  #endif
  /* Minix */
  #if defined EBADCALL
  ASSERT (streq (strerrorname_np (EBADCALL), "EBADCALL"));
  #endif
  /* Minix */
  #if defined EBADCPU
  ASSERT (streq (strerrorname_np (EBADCPU), "EBADCPU"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADE && EBADE != ECKSUM
  ASSERT (streq (strerrorname_np (EBADE), "EBADE"));
  #endif
  /* Minix */
  #if defined EBADEPT
  ASSERT (streq (strerrorname_np (EBADEPT), "EBADEPT"));
  #endif
  /* Mac OS X */
  #if defined EBADEXEC
  ASSERT (streq (strerrorname_np (EBADEXEC), "EBADEXEC"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADFD
  ASSERT (streq (strerrorname_np (EBADFD), "EBADFD"));
  #endif
  /* Minix */
  #if defined EBADIOCTL
  ASSERT (streq (strerrorname_np (EBADIOCTL), "EBADIOCTL"));
  #endif
  /* Mac OS X */
  #if defined EBADMACHO
  ASSERT (streq (strerrorname_np (EBADMACHO), "EBADMACHO"));
  #endif
  /* Minix */
  #if defined EBADMODE
  ASSERT (streq (strerrorname_np (EBADMODE), "EBADMODE"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADR && EBADR != EFRAGS
  ASSERT (streq (strerrorname_np (EBADR), "EBADR"));
  #endif
  /* Minix */
  #if defined EBADREQUEST
  ASSERT (streq (strerrorname_np (EBADREQUEST), "EBADREQUEST"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EBADRPC
  ASSERT (streq (strerrorname_np (EBADRPC), "EBADRPC"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADRQC
  ASSERT (streq (strerrorname_np (EBADRQC), "EBADRQC"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBADSLT
  ASSERT (streq (strerrorname_np (EBADSLT), "EBADSLT"));
  #endif
  /* HP-UX */
  #if defined EBADVER
  ASSERT (streq (strerrorname_np (EBADVER), "EBADVER"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EBFONT
  ASSERT (streq (strerrorname_np (EBFONT), "EBFONT"));
  #endif
  /* Minix */
  #if defined ECALLDENIED
  ASSERT (streq (strerrorname_np (ECALLDENIED), "ECALLDENIED"));
  #endif
  /* FreeBSD */
  #if defined ECAPMODE
  ASSERT (streq (strerrorname_np (ECAPMODE), "ECAPMODE"));
  #endif
  /* Cygwin */
  #if defined ECASECLASH
  ASSERT (streq (strerrorname_np (ECASECLASH), "ECASECLASH"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ECHRNG
  ASSERT (streq (strerrorname_np (ECHRNG), "ECHRNG"));
  #endif
  /* Solaris */
  #if defined ECKSUM
  ASSERT (streq (strerrorname_np (ECKSUM), "ECKSUM"));
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ECOMM
  ASSERT (streq (strerrorname_np (ECOMM), "ECOMM"));
  #endif
  /* HP-UX */
  #if defined ECONFIG
  ASSERT (streq (strerrorname_np (ECONFIG), "ECONFIG"));
  #endif
  /* AIX */
  #if defined ECORRUPT
  ASSERT (streq (strerrorname_np (ECORRUPT), "ECORRUPT"));
  #endif
  /* GNU/Hurd */
  #if defined ED
  ASSERT (streq (strerrorname_np (ED), "ED"));
  #endif
  /* Minix */
  #if defined EDEADEPT
  ASSERT (streq (strerrorname_np (EDEADEPT), "EDEADEPT"));
  #endif
  /* Solaris, Cygwin */
  #if defined EDEADLOCK && EDEADLOCK != EDEADLK
  ASSERT (streq (strerrorname_np (EDEADLOCK), "EDEADLOCK"));
  #endif
  /* Minix */
  #if defined EDEADSRCDST
  ASSERT (streq (strerrorname_np (EDEADSRCDST), "EDEADSRCDST"));
  #endif
  /* Mac OS X */
  #if defined EDEVERR
  ASSERT (streq (strerrorname_np (EDEVERR), "EDEVERR"));
  #endif
  /* GNU/Hurd */
  #if defined EDIED
  ASSERT (streq (strerrorname_np (EDIED), "EDIED"));
  #endif
  /* FreeBSD */
  #if defined EDIRIOCTL
  ASSERT (streq (strerrorname_np (EDIRIOCTL), "EDIRIOCTL"));
  #endif
  /* AIX */
  #if defined EDIST
  ASSERT (streq (strerrorname_np (EDIST), "EDIST"));
  #endif
  /* Minix */
  #if defined EDONTREPLY
  ASSERT (streq (strerrorname_np (EDONTREPLY), "EDONTREPLY"));
  #endif
  /* FreeBSD */
  #if defined EDOOFUS && EDOOFUS != EINVAL
  ASSERT (streq (strerrorname_np (EDOOFUS), "EDOOFUS"));
  #endif
  /* Linux, HP-UX, Cygwin */
  #if defined EDOTDOT
  ASSERT (streq (strerrorname_np (EDOTDOT), "EDOTDOT"));
  #endif
  /* GNU/Hurd */
  #if defined ED_ALREADY_OPEN
  ASSERT (streq (strerrorname_np (ED_ALREADY_OPEN), "ED_ALREADY_OPEN"));
  #endif
  /* GNU/Hurd */
  #if defined ED_DEVICE_DOWN
  ASSERT (streq (strerrorname_np (ED_DEVICE_DOWN), "ED_DEVICE_DOWN"));
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_OPERATION
  ASSERT (streq (strerrorname_np (ED_INVALID_OPERATION), "ED_INVALID_OPERATION"));
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_RECNUM
  ASSERT (streq (strerrorname_np (ED_INVALID_RECNUM), "ED_INVALID_RECNUM"));
  #endif
  /* GNU/Hurd */
  #if defined ED_INVALID_SIZE
  ASSERT (streq (strerrorname_np (ED_INVALID_SIZE), "ED_INVALID_SIZE"));
  #endif
  /* GNU/Hurd */
  #if defined ED_IO_ERROR
  ASSERT (streq (strerrorname_np (ED_IO_ERROR), "ED_IO_ERROR"));
  #endif
  /* GNU/Hurd */
  #if defined ED_NO_MEMORY
  ASSERT (streq (strerrorname_np (ED_NO_MEMORY), "ED_NO_MEMORY"));
  #endif
  /* GNU/Hurd */
  #if defined ED_NO_SUCH_DEVICE
  ASSERT (streq (strerrorname_np (ED_NO_SUCH_DEVICE), "ED_NO_SUCH_DEVICE"));
  #endif
  /* GNU/Hurd */
  #if defined ED_READ_ONLY
  ASSERT (streq (strerrorname_np (ED_READ_ONLY), "ED_READ_ONLY"));
  #endif
  /* GNU/Hurd */
  #if defined ED_WOULD_BLOCK
  ASSERT (streq (strerrorname_np (ED_WOULD_BLOCK), "ED_WOULD_BLOCK"));
  #endif
  /* AIX */
  #if defined EFORMAT
  ASSERT (streq (strerrorname_np (EFORMAT), "EFORMAT"));
  #endif
  /* Haiku */
  #if defined EFPOS
  ASSERT (streq (strerrorname_np (EFPOS), "EFPOS"));
  #endif
  /* Solaris */
  #if defined EFRAGS
  ASSERT (streq (strerrorname_np (EFRAGS), "EFRAGS"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix, Cygwin */
  #if defined EFTYPE
  ASSERT (streq (strerrorname_np (EFTYPE), "EFTYPE"));
  #endif
  /* Minix */
  #if defined EGENERIC
  ASSERT (streq (strerrorname_np (EGENERIC), "EGENERIC"));
  #endif
  /* GNU/Hurd */
  #if defined EGRATUITOUS
  ASSERT (streq (strerrorname_np (EGRATUITOUS), "EGRATUITOUS"));
  #endif
  /* GNU/Hurd */
  #if defined EGREGIOUS
  ASSERT (streq (strerrorname_np (EGREGIOUS), "EGREGIOUS"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EHOSTDOWN
  ASSERT (streq (strerrorname_np (EHOSTDOWN), "EHOSTDOWN"));
  #endif
  /* Linux */
  #if defined EHWPOISON
  ASSERT (streq (strerrorname_np (EHWPOISON), "EHWPOISON"));
  #endif
  /* GNU/Hurd */
  #if defined EIEIO
  ASSERT (streq (strerrorname_np (EIEIO), "EIEIO"));
  #endif
  /* OpenBSD */
  #if defined EIPSEC
  ASSERT (streq (strerrorname_np (EIPSEC), "EIPSEC"));
  #endif
  /* Linux */
  #if defined EISNAM
  ASSERT (streq (strerrorname_np (EISNAM), "EISNAM"));
  #endif
  /* FreeBSD */
  #if defined EJUSTRETURN
  ASSERT (streq (strerrorname_np (EJUSTRETURN), "EJUSTRETURN"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_ABORTED
  ASSERT (streq (strerrorname_np (EKERN_ABORTED), "EKERN_ABORTED"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_FAILURE
  ASSERT (streq (strerrorname_np (EKERN_FAILURE), "EKERN_FAILURE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INTERRUPTED
  ASSERT (streq (strerrorname_np (EKERN_INTERRUPTED), "EKERN_INTERRUPTED"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_ADDRESS
  ASSERT (streq (strerrorname_np (EKERN_INVALID_ADDRESS), "EKERN_INVALID_ADDRESS"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_ARGUMENT
  ASSERT (streq (strerrorname_np (EKERN_INVALID_ARGUMENT), "EKERN_INVALID_ARGUMENT"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_CAPABILITY
  ASSERT (streq (strerrorname_np (EKERN_INVALID_CAPABILITY), "EKERN_INVALID_CAPABILITY"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_HOST
  ASSERT (streq (strerrorname_np (EKERN_INVALID_HOST), "EKERN_INVALID_HOST"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_NAME
  ASSERT (streq (strerrorname_np (EKERN_INVALID_NAME), "EKERN_INVALID_NAME"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_RIGHT
  ASSERT (streq (strerrorname_np (EKERN_INVALID_RIGHT), "EKERN_INVALID_RIGHT"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_TASK
  ASSERT (streq (strerrorname_np (EKERN_INVALID_TASK), "EKERN_INVALID_TASK"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_INVALID_VALUE
  ASSERT (streq (strerrorname_np (EKERN_INVALID_VALUE), "EKERN_INVALID_VALUE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_ERROR
  ASSERT (streq (strerrorname_np (EKERN_MEMORY_ERROR), "EKERN_MEMORY_ERROR"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_FAILURE
  ASSERT (streq (strerrorname_np (EKERN_MEMORY_FAILURE), "EKERN_MEMORY_FAILURE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_MEMORY_PRESENT
  ASSERT (streq (strerrorname_np (EKERN_MEMORY_PRESENT), "EKERN_MEMORY_PRESENT"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NAME_EXISTS
  ASSERT (streq (strerrorname_np (EKERN_NAME_EXISTS), "EKERN_NAME_EXISTS"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NOT_IN_SET
  ASSERT (streq (strerrorname_np (EKERN_NOT_IN_SET), "EKERN_NOT_IN_SET"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NOT_RECEIVER
  ASSERT (streq (strerrorname_np (EKERN_NOT_RECEIVER), "EKERN_NOT_RECEIVER"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NO_ACCESS
  ASSERT (streq (strerrorname_np (EKERN_NO_ACCESS), "EKERN_NO_ACCESS"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_NO_SPACE
  ASSERT (streq (strerrorname_np (EKERN_NO_SPACE), "EKERN_NO_SPACE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_PROTECTION_FAILURE
  ASSERT (streq (strerrorname_np (EKERN_PROTECTION_FAILURE), "EKERN_PROTECTION_FAILURE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_RESOURCE_SHORTAGE
  ASSERT (streq (strerrorname_np (EKERN_RESOURCE_SHORTAGE), "EKERN_RESOURCE_SHORTAGE"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_RIGHT_EXISTS
  ASSERT (streq (strerrorname_np (EKERN_RIGHT_EXISTS), "EKERN_RIGHT_EXISTS"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_TERMINATED
  ASSERT (streq (strerrorname_np (EKERN_TERMINATED), "EKERN_TERMINATED"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_TIMEDOUT
  ASSERT (streq (strerrorname_np (EKERN_TIMEDOUT), "EKERN_TIMEDOUT"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_UREFS_OVERFLOW
  ASSERT (streq (strerrorname_np (EKERN_UREFS_OVERFLOW), "EKERN_UREFS_OVERFLOW"));
  #endif
  /* GNU/Hurd */
  #if defined EKERN_WRITE_PROTECTION_FAILURE
  ASSERT (streq (strerrorname_np (EKERN_WRITE_PROTECTION_FAILURE), "EKERN_WRITE_PROTECTION_FAILURE"));
  #endif
  /* Linux */
  #if defined EKEYEXPIRED
  ASSERT (streq (strerrorname_np (EKEYEXPIRED), "EKEYEXPIRED"));
  #endif
  /* Linux */
  #if defined EKEYREJECTED
  ASSERT (streq (strerrorname_np (EKEYREJECTED), "EKEYREJECTED"));
  #endif
  /* Linux */
  #if defined EKEYREVOKED
  ASSERT (streq (strerrorname_np (EKEYREVOKED), "EKEYREVOKED"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL2HLT
  ASSERT (streq (strerrorname_np (EL2HLT), "EL2HLT"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL2NSYNC
  ASSERT (streq (strerrorname_np (EL2NSYNC), "EL2NSYNC"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL3HLT
  ASSERT (streq (strerrorname_np (EL3HLT), "EL3HLT"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EL3RST
  ASSERT (streq (strerrorname_np (EL3RST), "EL3RST"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ELAST && 0
  ASSERT (streq (strerrorname_np (ELAST), "ELAST"));
  #endif
  /* Cygwin */
  #if defined ELBIN
  ASSERT (streq (strerrorname_np (ELBIN), "ELBIN"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBACC
  ASSERT (streq (strerrorname_np (ELIBACC), "ELIBACC"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBBAD
  ASSERT (streq (strerrorname_np (ELIBBAD), "ELIBBAD"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBEXEC
  ASSERT (streq (strerrorname_np (ELIBEXEC), "ELIBEXEC"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBMAX
  ASSERT (streq (strerrorname_np (ELIBMAX), "ELIBMAX"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ELIBSCN
  ASSERT (streq (strerrorname_np (ELIBSCN), "ELIBSCN"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ELNRNG
  ASSERT (streq (strerrorname_np (ELNRNG), "ELNRNG"));
  #endif
  /* Minix */
  #if defined ELOCKED
  ASSERT (streq (strerrorname_np (ELOCKED), "ELOCKED"));
  #endif
  /* Solaris */
  #if defined ELOCKUNMAPPED
  ASSERT (streq (strerrorname_np (ELOCKUNMAPPED), "ELOCKUNMAPPED"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_BODY_ERROR
  ASSERT (streq (strerrorname_np (EMACH_RCV_BODY_ERROR), "EMACH_RCV_BODY_ERROR"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_HEADER_ERROR
  ASSERT (streq (strerrorname_np (EMACH_RCV_HEADER_ERROR), "EMACH_RCV_HEADER_ERROR"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INTERRUPTED
  ASSERT (streq (strerrorname_np (EMACH_RCV_INTERRUPTED), "EMACH_RCV_INTERRUPTED"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_DATA
  ASSERT (streq (strerrorname_np (EMACH_RCV_INVALID_DATA), "EMACH_RCV_INVALID_DATA"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_NAME
  ASSERT (streq (strerrorname_np (EMACH_RCV_INVALID_NAME), "EMACH_RCV_INVALID_NAME"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_INVALID_NOTIFY
  ASSERT (streq (strerrorname_np (EMACH_RCV_INVALID_NOTIFY), "EMACH_RCV_INVALID_NOTIFY"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_IN_PROGRESS
  ASSERT (streq (strerrorname_np (EMACH_RCV_IN_PROGRESS), "EMACH_RCV_IN_PROGRESS"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_IN_SET
  ASSERT (streq (strerrorname_np (EMACH_RCV_IN_SET), "EMACH_RCV_IN_SET"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_PORT_CHANGED
  ASSERT (streq (strerrorname_np (EMACH_RCV_PORT_CHANGED), "EMACH_RCV_PORT_CHANGED"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_PORT_DIED
  ASSERT (streq (strerrorname_np (EMACH_RCV_PORT_DIED), "EMACH_RCV_PORT_DIED"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_TIMED_OUT
  ASSERT (streq (strerrorname_np (EMACH_RCV_TIMED_OUT), "EMACH_RCV_TIMED_OUT"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_RCV_TOO_LARGE
  ASSERT (streq (strerrorname_np (EMACH_RCV_TOO_LARGE), "EMACH_RCV_TOO_LARGE"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INTERRUPTED
  ASSERT (streq (strerrorname_np (EMACH_SEND_INTERRUPTED), "EMACH_SEND_INTERRUPTED"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_DATA
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_DATA), "EMACH_SEND_INVALID_DATA"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_DEST
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_DEST), "EMACH_SEND_INVALID_DEST"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_HEADER
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_HEADER), "EMACH_SEND_INVALID_HEADER"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_MEMORY
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_MEMORY), "EMACH_SEND_INVALID_MEMORY"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_NOTIFY
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_NOTIFY), "EMACH_SEND_INVALID_NOTIFY"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_REPLY
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_REPLY), "EMACH_SEND_INVALID_REPLY"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_RIGHT
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_RIGHT), "EMACH_SEND_INVALID_RIGHT"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_INVALID_TYPE
  ASSERT (streq (strerrorname_np (EMACH_SEND_INVALID_TYPE), "EMACH_SEND_INVALID_TYPE"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_IN_PROGRESS
  ASSERT (streq (strerrorname_np (EMACH_SEND_IN_PROGRESS), "EMACH_SEND_IN_PROGRESS"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_MSG_TOO_SMALL
  ASSERT (streq (strerrorname_np (EMACH_SEND_MSG_TOO_SMALL), "EMACH_SEND_MSG_TOO_SMALL"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NOTIFY_IN_PROGRESS
  ASSERT (streq (strerrorname_np (EMACH_SEND_NOTIFY_IN_PROGRESS), "EMACH_SEND_NOTIFY_IN_PROGRESS"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NO_BUFFER
  ASSERT (streq (strerrorname_np (EMACH_SEND_NO_BUFFER), "EMACH_SEND_NO_BUFFER"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_NO_NOTIFY
  ASSERT (streq (strerrorname_np (EMACH_SEND_NO_NOTIFY), "EMACH_SEND_NO_NOTIFY"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_TIMED_OUT
  ASSERT (streq (strerrorname_np (EMACH_SEND_TIMED_OUT), "EMACH_SEND_TIMED_OUT"));
  #endif
  /* GNU/Hurd */
  #if defined EMACH_SEND_WILL_NOTIFY
  ASSERT (streq (strerrorname_np (EMACH_SEND_WILL_NOTIFY), "EMACH_SEND_WILL_NOTIFY"));
  #endif
  /* AIX */
  #if defined EMEDIA
  ASSERT (streq (strerrorname_np (EMEDIA), "EMEDIA"));
  #endif
  /* Linux, OpenBSD */
  #if defined EMEDIUMTYPE
  ASSERT (streq (strerrorname_np (EMEDIUMTYPE), "EMEDIUMTYPE"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_ARRAY_TOO_LARGE
  ASSERT (streq (strerrorname_np (EMIG_ARRAY_TOO_LARGE), "EMIG_ARRAY_TOO_LARGE"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_BAD_ARGUMENTS
  ASSERT (streq (strerrorname_np (EMIG_BAD_ARGUMENTS), "EMIG_BAD_ARGUMENTS"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_BAD_ID
  ASSERT (streq (strerrorname_np (EMIG_BAD_ID), "EMIG_BAD_ID"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_DESTROY_REQUEST
  ASSERT (streq (strerrorname_np (EMIG_DESTROY_REQUEST), "EMIG_DESTROY_REQUEST"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_EXCEPTION
  ASSERT (streq (strerrorname_np (EMIG_EXCEPTION), "EMIG_EXCEPTION"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_NO_REPLY
  ASSERT (streq (strerrorname_np (EMIG_NO_REPLY), "EMIG_NO_REPLY"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_REMOTE_ERROR
  ASSERT (streq (strerrorname_np (EMIG_REMOTE_ERROR), "EMIG_REMOTE_ERROR"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_REPLY_MISMATCH
  ASSERT (streq (strerrorname_np (EMIG_REPLY_MISMATCH), "EMIG_REPLY_MISMATCH"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_SERVER_DIED
  ASSERT (streq (strerrorname_np (EMIG_SERVER_DIED), "EMIG_SERVER_DIED"));
  #endif
  /* GNU/Hurd */
  #if defined EMIG_TYPE_ERROR
  ASSERT (streq (strerrorname_np (EMIG_TYPE_ERROR), "EMIG_TYPE_ERROR"));
  #endif
  /* Linux */
  #if defined ENAVAIL
  ASSERT (streq (strerrorname_np (ENAVAIL), "ENAVAIL"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ENEEDAUTH
  ASSERT (streq (strerrorname_np (ENEEDAUTH), "ENEEDAUTH"));
  #endif
  /* Cygwin */
  #if defined ENMFILE
  ASSERT (streq (strerrorname_np (ENMFILE), "ENMFILE"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ENOANO && ENOANO != ENOKEY
  ASSERT (streq (strerrorname_np (ENOANO), "ENOANO"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, Minix */
  #if defined ENOATTR
  ASSERT (streq (strerrorname_np (ENOATTR), "ENOATTR"));
  #endif
  /* MirBSD */
  #if defined ENOCOFFEE
  ASSERT (streq (strerrorname_np (ENOCOFFEE), "ENOCOFFEE"));
  #endif
  /* Minix */
  #if defined ENOCONN
  ASSERT (streq (strerrorname_np (ENOCONN), "ENOCONN"));
  #endif
  /* AIX */
  #if defined ENOCONNECT
  ASSERT (streq (strerrorname_np (ENOCONNECT), "ENOCONNECT"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined ENOCSI
  ASSERT (streq (strerrorname_np (ENOCSI), "ENOCSI"));
  #endif
  /* FreeBSD */
  #if defined ENOIOCTL
  ASSERT (streq (strerrorname_np (ENOIOCTL), "ENOIOCTL"));
  #endif
  /* Linux */
  #if defined ENOKEY
  ASSERT (streq (strerrorname_np (ENOKEY), "ENOKEY"));
  #endif
  /* HP-UX */
  #if defined ENOLOAD
  ASSERT (streq (strerrorname_np (ENOLOAD), "ENOLOAD"));
  #endif
  /* HP-UX */
  #if defined ENOMATCH
  ASSERT (streq (strerrorname_np (ENOMATCH), "ENOMATCH"));
  #endif
  /* Linux, OpenBSD, Cygwin */
  #if defined ENOMEDIUM
  ASSERT (streq (strerrorname_np (ENOMEDIUM), "ENOMEDIUM"));
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ENONET
  ASSERT (streq (strerrorname_np (ENONET), "ENONET"));
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ENOPKG
  ASSERT (streq (strerrorname_np (ENOPKG), "ENOPKG"));
  #endif
  /* Mac OS X */
  #if defined ENOPOLICY
  ASSERT (streq (strerrorname_np (ENOPOLICY), "ENOPOLICY"));
  #endif
  /* HP-UX */
  #if defined ENOREG
  ASSERT (streq (strerrorname_np (ENOREG), "ENOREG"));
  #endif
  /* Cygwin */
  #if defined ENOSHARE
  ASSERT (streq (strerrorname_np (ENOSHARE), "ENOSHARE"));
  #endif
  /* Linux, HP-UX */
  #if defined ENOSYM
  ASSERT (streq (strerrorname_np (ENOSYM), "ENOSYM"));
  #endif
  /* Solaris */
  #if defined ENOTACTIVE
  ASSERT (streq (strerrorname_np (ENOTACTIVE), "ENOTACTIVE"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined ENOTBLK
  ASSERT (streq (strerrorname_np (ENOTBLK), "ENOTBLK"));
  #endif
  /* FreeBSD */
  #if defined ENOTCAPABLE
  ASSERT (streq (strerrorname_np (ENOTCAPABLE), "ENOTCAPABLE"));
  #endif
  /* Linux */
  #if defined ENOTNAM
  ASSERT (streq (strerrorname_np (ENOTNAM), "ENOTNAM"));
  #endif
  /* AIX, Minix */
  #if defined ENOTREADY
  ASSERT (streq (strerrorname_np (ENOTREADY), "ENOTREADY"));
  #endif
  /* AIX */
  #if defined ENOTRUST
  ASSERT (streq (strerrorname_np (ENOTRUST), "ENOTRUST"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ENOTUNIQ
  ASSERT (streq (strerrorname_np (ENOTUNIQ), "ENOTUNIQ"));
  #endif
  /* HP-UX */
  #if defined ENOUNLD
  ASSERT (streq (strerrorname_np (ENOUNLD), "ENOUNLD"));
  #endif
  /* HP-UX */
  #if defined ENOUNREG
  ASSERT (streq (strerrorname_np (ENOUNREG), "ENOUNREG"));
  #endif
  /* Minix */
  #if defined ENOURG
  ASSERT (streq (strerrorname_np (ENOURG), "ENOURG"));
  #endif
  /* native Windows */
  #if defined EOTHER
  ASSERT (streq (strerrorname_np (EOTHER), "EOTHER"));
  #endif
  /* Minix */
  #if defined EPACKSIZE
  ASSERT (streq (strerrorname_np (EPACKSIZE), "EPACKSIZE"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EPFNOSUPPORT
  ASSERT (streq (strerrorname_np (EPFNOSUPPORT), "EPFNOSUPPORT"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, Minix, Cygwin */
  #if defined EPROCLIM
  ASSERT (streq (strerrorname_np (EPROCLIM), "EPROCLIM"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROCUNAVAIL
  ASSERT (streq (strerrorname_np (EPROCUNAVAIL), "EPROCUNAVAIL"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROGMISMATCH
  ASSERT (streq (strerrorname_np (EPROGMISMATCH), "EPROGMISMATCH"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined EPROGUNAVAIL
  ASSERT (streq (strerrorname_np (EPROGUNAVAIL), "EPROGUNAVAIL"));
  #endif
  /* Mac OS X */
  #if defined EPWROFF
  ASSERT (streq (strerrorname_np (EPWROFF), "EPWROFF"));
  #endif
  /* Mac OS X */
  #if defined EQFULL
  ASSERT (streq (strerrorname_np (EQFULL), "EQFULL"));
  #endif
  /* HP-UX */
  #if defined ERELOC
  ASSERT (streq (strerrorname_np (ERELOC), "ERELOC"));
  #endif
  /* FreeBSD */
  #if defined ERELOOKUP
  ASSERT (streq (strerrorname_np (ERELOOKUP), "ERELOOKUP"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EREMCHG
  ASSERT (streq (strerrorname_np (EREMCHG), "EREMCHG"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined EREMOTE
  ASSERT (streq (strerrorname_np (EREMOTE), "EREMOTE"));
  #endif
  /* Linux */
  #if defined EREMOTEIO
  ASSERT (streq (strerrorname_np (EREMOTEIO), "EREMOTEIO"));
  #endif
  /* Linux, HP-UX */
  #if defined EREMOTERELEASE
  ASSERT (streq (strerrorname_np (EREMOTERELEASE), "EREMOTERELEASE"));
  #endif
  /* Linux, FreeBSD, AIX, Solaris, Minix */
  #if defined ERESTART
  ASSERT (streq (strerrorname_np (ERESTART), "ERESTART"));
  #endif
  /* Linux */
  #if defined ERFKILL
  ASSERT (streq (strerrorname_np (ERFKILL), "ERFKILL"));
  #endif
  /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
  #if defined ERPCMISMATCH
  ASSERT (streq (strerrorname_np (ERPCMISMATCH), "ERPCMISMATCH"));
  #endif
  /* AIX */
  #if defined ESAD
  ASSERT (streq (strerrorname_np (ESAD), "ESAD"));
  #endif
  /* Mac OS X */
  #if defined ESHLIBVERS
  ASSERT (streq (strerrorname_np (ESHLIBVERS), "ESHLIBVERS"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Haiku, Cygwin */
  #if defined ESHUTDOWN
  ASSERT (streq (strerrorname_np (ESHUTDOWN), "ESHUTDOWN"));
  #endif
  /* Haiku */
  #if defined ESIGPARM
  ASSERT (streq (strerrorname_np (ESIGPARM), "ESIGPARM"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined ESOCKTNOSUPPORT
  ASSERT (streq (strerrorname_np (ESOCKTNOSUPPORT), "ESOCKTNOSUPPORT"));
  #endif
  /* AIX */
  #if defined ESOFT
  ASSERT (streq (strerrorname_np (ESOFT), "ESOFT"));
  #endif
  /* Linux, HP-UX, Solaris, Cygwin */
  #if defined ESRMNT
  ASSERT (streq (strerrorname_np (ESRMNT), "ESRMNT"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined ESTRPIPE
  ASSERT (streq (strerrorname_np (ESTRPIPE), "ESTRPIPE"));
  #endif
  /* AIX */
  #if defined ESYSERROR
  ASSERT (streq (strerrorname_np (ESYSERROR), "ESYSERROR"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined ETOOMANYREFS
  ASSERT (streq (strerrorname_np (ETOOMANYREFS), "ETOOMANYREFS"));
  #endif
  /* Minix */
  #if defined ETRAPDENIED
  ASSERT (streq (strerrorname_np (ETRAPDENIED), "ETRAPDENIED"));
  #endif
  /* Linux */
  #if defined EUCLEAN
  ASSERT (streq (strerrorname_np (EUCLEAN), "EUCLEAN"));
  #endif
  /* Linux, AIX, HP-UX, Solaris, Cygwin */
  #if defined EUNATCH
  ASSERT (streq (strerrorname_np (EUNATCH), "EUNATCH"));
  #endif
  /* Minix */
  #if defined EURG
  ASSERT (streq (strerrorname_np (EURG), "EURG"));
  #endif
  /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Minix, Cygwin */
  #if defined EUSERS
  ASSERT (streq (strerrorname_np (EUSERS), "EUSERS"));
  #endif
  /* AIX */
  #if defined EWRPROTECT
  ASSERT (streq (strerrorname_np (EWRPROTECT), "EWRPROTECT"));
  #endif
  /* Linux, Solaris, Cygwin */
  #if defined EXFULL
  ASSERT (streq (strerrorname_np (EXFULL), "EXFULL"));
  #endif

  ASSERT (strerrorname_np (20200830) == NULL);
  ASSERT (strerrorname_np (-714) == NULL);

  return test_exit_status;
}
