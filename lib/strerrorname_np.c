/* Name of system error code.
   Copyright (C) 2020-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2020.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include <errno.h>

const char *
strerrorname_np (int errnum)
{
  switch (errnum)
    {
    /* Error codes specified by ISO C.  */
    case EDOM:            return "EDOM";
    case EILSEQ:          return "EILSEQ";
    case ERANGE:          return "ERANGE";

    /* Error codes specified by POSIX.
       <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html>  */
    #if defined E2BIG
    case E2BIG:           return "E2BIG";
    #endif
    #if defined EACCES
    case EACCES:          return "EACCES";
    #endif
    #if defined EADDRINUSE
    case EADDRINUSE:      return "EADDRINUSE";
    #endif
    #if defined EADDRNOTAVAIL
    case EADDRNOTAVAIL:   return "EADDRNOTAVAIL";
    #endif
    #if defined EAFNOSUPPORT
    case EAFNOSUPPORT:    return "EAFNOSUPPORT";
    #endif
    #if defined EAGAIN
    case EAGAIN:          return "EAGAIN";
    #endif
    #if defined EALREADY
    case EALREADY:        return "EALREADY";
    #endif
    #if defined EBADF
    case EBADF:           return "EBADF";
    #endif
    #if defined EBADMSG
    case EBADMSG:         return "EBADMSG";
    #endif
    #if defined EBUSY
    case EBUSY:           return "EBUSY";
    #endif
    #if defined ECANCELED
    case ECANCELED:       return "ECANCELED";
    #endif
    #if defined ECHILD
    case ECHILD:          return "ECHILD";
    #endif
    #if defined ECONNABORTED
    case ECONNABORTED:    return "ECONNABORTED";
    #endif
    #if defined ECONNREFUSED
    case ECONNREFUSED:    return "ECONNREFUSED";
    #endif
    #if defined ECONNRESET
    case ECONNRESET:      return "ECONNRESET";
    #endif
    #if defined EDEADLK
    case EDEADLK:         return "EDEADLK";
    #endif
    #if defined EDESTADDRREQ
    case EDESTADDRREQ:    return "EDESTADDRREQ";
    #endif
    #if defined EDQUOT
    case EDQUOT:          return "EDQUOT";
    #endif
    #if defined EEXIST
    case EEXIST:          return "EEXIST";
    #endif
    #if defined EFAULT
    case EFAULT:          return "EFAULT";
    #endif
    #if defined EFBIG
    case EFBIG:           return "EFBIG";
    #endif
    #if defined EHOSTUNREACH
    case EHOSTUNREACH:    return "EHOSTUNREACH";
    #endif
    #if defined EIDRM
    case EIDRM:           return "EIDRM";
    #endif
    #if defined EINPROGRESS
    case EINPROGRESS:     return "EINPROGRESS";
    #endif
    #if defined EINTR
    case EINTR:           return "EINTR";
    #endif
    #if defined EINVAL
    case EINVAL:          return "EINVAL";
    #endif
    #if defined EIO
    case EIO:             return "EIO";
    #endif
    #if defined EISCONN
    case EISCONN:         return "EISCONN";
    #endif
    #if defined EISDIR
    case EISDIR:          return "EISDIR";
    #endif
    #if defined ELOOP
    case ELOOP:           return "ELOOP";
    #endif
    #if defined EMFILE
    case EMFILE:          return "EMFILE";
    #endif
    #if defined EMLINK
    case EMLINK:          return "EMLINK";
    #endif
    #if defined EMSGSIZE
    case EMSGSIZE:        return "EMSGSIZE";
    #endif
    #if defined EMULTIHOP
    case EMULTIHOP:       return "EMULTIHOP";
    #endif
    #if defined ENAMETOOLONG
    case ENAMETOOLONG:    return "ENAMETOOLONG";
    #endif
    #if defined ENETDOWN
    case ENETDOWN:        return "ENETDOWN";
    #endif
    #if defined ENETRESET
    case ENETRESET:       return "ENETRESET";
    #endif
    #if defined ENETUNREACH
    case ENETUNREACH:     return "ENETUNREACH";
    #endif
    #if defined ENFILE
    case ENFILE:          return "ENFILE";
    #endif
    #if defined ENOBUFS
    case ENOBUFS:         return "ENOBUFS";
    #endif
    #if defined ENODATA
    case ENODATA:         return "ENODATA";
    #endif
    #if defined ENODEV
    case ENODEV:          return "ENODEV";
    #endif
    #if defined ENOENT
    case ENOENT:          return "ENOENT";
    #endif
    #if defined ENOEXEC
    case ENOEXEC:         return "ENOEXEC";
    #endif
    #if defined ENOLCK
    case ENOLCK:          return "ENOLCK";
    #endif
    #if defined ENOLINK
    case ENOLINK:         return "ENOLINK";
    #endif
    #if defined ENOMEM
    case ENOMEM:          return "ENOMEM";
    #endif
    #if defined ENOMSG
    case ENOMSG:          return "ENOMSG";
    #endif
    #if defined ENOPROTOOPT
    case ENOPROTOOPT:     return "ENOPROTOOPT";
    #endif
    #if defined ENOSPC
    case ENOSPC:          return "ENOSPC";
    #endif
    #if defined ENOSR
    case ENOSR:           return "ENOSR";
    #endif
    #if defined ENOSTR
    case ENOSTR:          return "ENOSTR";
    #endif
    #if defined ENOSYS
    case ENOSYS:          return "ENOSYS";
    #endif
    #if defined ENOTCONN
    case ENOTCONN:        return "ENOTCONN";
    #endif
    #if defined ENOTDIR
    case ENOTDIR:         return "ENOTDIR";
    #endif
    #if defined ENOTEMPTY && ENOTEMPTY != EEXIST
    case ENOTEMPTY:       return "ENOTEMPTY";
    #endif
    #if defined ENOTRECOVERABLE
    case ENOTRECOVERABLE: return "ENOTRECOVERABLE";
    #endif
    #if defined ENOTSOCK
    case ENOTSOCK:        return "ENOTSOCK";
    #endif
    #if defined ENOTSUP && ENOTSUP != EOPNOTSUPP
    case ENOTSUP:         return "ENOTSUP";
    #endif
    #if defined ENOTTY
    case ENOTTY:          return "ENOTTY";
    #endif
    #if defined ENXIO
    case ENXIO:           return "ENXIO";
    #endif
    #if defined EOPNOTSUPP
    case EOPNOTSUPP:      return "EOPNOTSUPP";
    #endif
    #if defined EOVERFLOW
    case EOVERFLOW:       return "EOVERFLOW";
    #endif
    #if defined EOWNERDEAD
    case EOWNERDEAD:      return "EOWNERDEAD";
    #endif
    #if defined EPERM
    case EPERM:           return "EPERM";
    #endif
    #if defined EPIPE
    case EPIPE:           return "EPIPE";
    #endif
    #if defined EPROTO
    case EPROTO:          return "EPROTO";
    #endif
    #if defined EPROTONOSUPPORT
    case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
    #endif
    #if defined EPROTOTYPE
    case EPROTOTYPE:      return "EPROTOTYPE";
    #endif
    #if defined EROFS
    case EROFS:           return "EROFS";
    #endif
    #if defined ESPIPE
    case ESPIPE:          return "ESPIPE";
    #endif
    #if defined ESRCH
    case ESRCH:           return "ESRCH";
    #endif
    #if defined ESTALE
    case ESTALE:          return "ESTALE";
    #endif
    #if defined ETIME
    case ETIME:           return "ETIME";
    #endif
    #if defined ETIMEDOUT
    case ETIMEDOUT:       return "ETIMEDOUT";
    #endif
    #if defined ETXTBSY
    case ETXTBSY:         return "ETXTBSY";
    #endif
    #if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:     return "EWOULDBLOCK";
    #endif
    #if defined EXDEV
    case EXDEV:           return "EXDEV";
    #endif

    /* Other error codes on other systems.  */
    /* Solaris */
    #if defined EADI
    case EADI:            return "EADI";
    #endif
    /* Linux, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EADV
    case EADV:            return "EADV";
    #endif
    /* OSF/1 */
    #if defined EAIO
    case EAIO:            return "EAIO";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
    #if defined EAUTH
    case EAUTH:           return "EAUTH";
    #endif
    /* GNU/Hurd */
    #if defined EBACKGROUND
    case EBACKGROUND:     return "EBACKGROUND";
    #endif
    /* Mac OS X */
    #if defined EBADARCH
    case EBADARCH:        return "EBADARCH";
    #endif
    /* Minix */
    #if defined EBADCALL
    case EBADCALL:        return "EBADCALL";
    #endif
    /* Minix */
    #if defined EBADCPU
    case EBADCPU:         return "EBADCPU";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBADE
    case EBADE:           return "EBADE";
    #endif
    /* Minix */
    #if defined EBADEPT
    case EBADEPT:         return "EBADEPT";
    #endif
    /* Mac OS X */
    #if defined EBADEXEC
    case EBADEXEC:        return "EBADEXEC";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBADFD
    case EBADFD:          return "EBADFD";
    #endif
    /* IRIX */
    #if defined EBADFILT
    case EBADFILT:        return "EBADFILT";
    #endif
    /* Minix */
    #if defined EBADIOCTL
    case EBADIOCTL:       return "EBADIOCTL";
    #endif
    /* Mac OS X */
    #if defined EBADMACHO
    case EBADMACHO:       return "EBADMACHO";
    #endif
    /* Minix */
    #if defined EBADMODE
    case EBADMODE:        return "EBADMODE";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBADR
    case EBADR:           return "EBADR";
    #endif
    /* Minix */
    #if defined EBADREQUEST
    case EBADREQUEST:     return "EBADREQUEST";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix */
    #if defined EBADRPC
    case EBADRPC:         return "EBADRPC";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBADRQC
    case EBADRQC:         return "EBADRQC";
    #endif
    /* IRIX */
    #if defined EBADRSPEC
    case EBADRSPEC:       return "EBADRSPEC";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBADSLT
    case EBADSLT:         return "EBADSLT";
    #endif
    /* IRIX */
    #if defined EBADTSPEC
    case EBADTSPEC:       return "EBADTSPEC";
    #endif
    /* HP-UX */
    #if defined EBADVER
    case EBADVER:         return "EBADVER";
    #endif
    /* IRIX */
    #if defined EBDHDL
    case EBDHDL:          return "EBDHDL";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EBFONT
    case EBFONT:          return "EBFONT";
    #endif
    /* IRIX */
    #if defined EBUFSIZE
    case EBUFSIZE:        return "EBUFSIZE";
    #endif
    /* Minix */
    #if defined ECALLDENIED
    case ECALLDENIED:     return "ECALLDENIED";
    #endif
    /* IRIX */
    #if defined ECANTEXTENT
    case ECANTEXTENT:     return "ECANTEXTENT";
    #endif
    /* FreeBSD */
    #if defined ECAPMODE
    case ECAPMODE:        return "ECAPMODE";
    #endif
    /* Cygwin */
    #if defined ECASECLASH
    case ECASECLASH:      return "ECASECLASH";
    #endif
    /* IRIX */
    #if defined ECELLDOWN
    case ECELLDOWN:       return "ECELLDOWN";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ECHRNG
    case ECHRNG:          return "ECHRNG";
    #endif
    /* IRIX */
    #if defined ECKPT
    case ECKPT:           return "ECKPT";
    #endif
    /* IRIX */
    #if defined ECLOCKCPU
    case ECLOCKCPU:       return "ECLOCKCPU";
    #endif
    /* OSF/1 */
    #if defined ECLONEME && ECLONEME != ERESTART
    case ECLONEME:        return "ECLONEME";
    #endif
    /* Linux, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ECOMM
    case ECOMM:           return "ECOMM";
    #endif
    /* HP-UX */
    #if defined ECONFIG
    case ECONFIG:         return "ECONFIG";
    #endif
    /* IRIX */
    #if defined ECONTROLLER
    case ECONTROLLER:     return "ECONTROLLER";
    #endif
    /* AIX */
    #if defined ECORRUPT
    case ECORRUPT:        return "ECORRUPT";
    #endif
    /* GNU/Hurd */
    #if defined ED
    case ED:              return "ED";
    #endif
    /* Minix */
    #if defined EDEADEPT
    case EDEADEPT:        return "EDEADEPT";
    #endif
    /* IRIX, Solaris, Cygwin */
    #if defined EDEADLOCK && EDEADLOCK != EDEADLK
    case EDEADLOCK:       return "EDEADLOCK";
    #endif
    /* Minix */
    #if defined EDEADSRCDST
    case EDEADSRCDST:     return "EDEADSRCDST";
    #endif
    /* IRIX */
    #if defined EDELAY
    case EDELAY:          return "EDELAY";
    #endif
    /* IRIX */
    #if defined EDESTROYED
    case EDESTROYED:      return "EDESTROYED";
    #endif
    /* Mac OS X */
    #if defined EDEVERR
    case EDEVERR:         return "EDEVERR";
    #endif
    /* GNU/Hurd */
    #if defined EDIED
    case EDIED:           return "EDIED";
    #endif
    /* IRIX */
    #if defined EDIRCORRUPTED && EDIRCORRUPTED != EFSCORRUPTED
    case EDIRCORRUPTED:   return "EDIRCORRUPTED";
    #endif
    /* FreeBSD */
    #if defined EDIRIOCTL
    case EDIRIOCTL:       return "EDIRIOCTL";
    #endif
    /* OSF/1 */
    #if defined EDIRTY
    case EDIRTY:          return "EDIRTY";
    #endif
    /* IRIX */
    #if defined EDISJOINT
    case EDISJOINT:       return "EDISJOINT";
    #endif
    /* AIX */
    #if defined EDIST
    case EDIST:           return "EDIST";
    #endif
    /* Minix */
    #if defined EDONTREPLY
    case EDONTREPLY:      return "EDONTREPLY";
    #endif
    /* FreeBSD */
    #if defined EDOOFUS
    case EDOOFUS:         return "EDOOFUS";
    #endif
    /* Linux, HP-UX, Cygwin */
    #if defined EDOTDOT
    case EDOTDOT:         return "EDOTDOT";
    #endif
    /* OSF/1 */
    #if defined EDUPPKG
    case EDUPPKG:         return "EDUPPKG";
    #endif
    /* GNU/Hurd */
    #if defined ED_ALREADY_OPEN
    case ED_ALREADY_OPEN: return "ED_ALREADY_OPEN";
    #endif
    /* GNU/Hurd */
    #if defined ED_DEVICE_DOWN
    case ED_DEVICE_DOWN:  return "ED_DEVICE_DOWN";
    #endif
    /* GNU/Hurd */
    #if defined ED_INVALID_OPERATION
    case ED_INVALID_OPERATION:return "ED_INVALID_OPERATION";
    #endif
    /* GNU/Hurd */
    #if defined ED_INVALID_RECNUM
    case ED_INVALID_RECNUM:return "ED_INVALID_RECNUM";
    #endif
    /* GNU/Hurd */
    #if defined ED_INVALID_SIZE
    case ED_INVALID_SIZE: return "ED_INVALID_SIZE";
    #endif
    /* GNU/Hurd */
    #if defined ED_IO_ERROR
    case ED_IO_ERROR:     return "ED_IO_ERROR";
    #endif
    /* GNU/Hurd */
    #if defined ED_NO_MEMORY
    case ED_NO_MEMORY:    return "ED_NO_MEMORY";
    #endif
    /* GNU/Hurd */
    #if defined ED_NO_SUCH_DEVICE
    case ED_NO_SUCH_DEVICE:return "ED_NO_SUCH_DEVICE";
    #endif
    /* GNU/Hurd */
    #if defined ED_READ_ONLY
    case ED_READ_ONLY:    return "ED_READ_ONLY";
    #endif
    /* GNU/Hurd */
    #if defined ED_WOULD_BLOCK
    case ED_WOULD_BLOCK:  return "ED_WOULD_BLOCK";
    #endif
    /* IRIX */
    #if defined EEMPTY
    case EEMPTY:          return "EEMPTY";
    #endif
    /* OSF/1 */
    #if defined EEMULATE
    case EEMULATE:        return "EEMULATE";
    #endif
    /* IRIX */
    #if defined EENDOFMINOR
    case EENDOFMINOR:     return "EENDOFMINOR";
    #endif
    /* IRIX */
    #if defined EENQUEUED
    case EENQUEUED:       return "EENQUEUED";
    #endif
    /* OSF/1 */
    #if defined EFAIL
    case EFAIL:           return "EFAIL";
    #endif
    /* AIX */
    #if defined EFORMAT
    case EFORMAT:         return "EFORMAT";
    #endif
    /* Haiku */
    #if defined EFPOS
    case EFPOS:           return "EFPOS";
    #endif
    /* IRIX */
    #if defined EFSCORRUPTED
    case EFSCORRUPTED:    return "EFSCORRUPTED";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix, Cygwin */
    #if defined EFTYPE
    case EFTYPE:          return "EFTYPE";
    #endif
    /* Minix */
    #if defined EGENERIC
    case EGENERIC:        return "EGENERIC";
    #endif
    /* GNU/Hurd */
    #if defined EGRATUITOUS
    case EGRATUITOUS:     return "EGRATUITOUS";
    #endif
    /* GNU/Hurd */
    #if defined EGREGIOUS
    case EGREGIOUS:       return "EGREGIOUS";
    #endif
    /* IRIX */
    #if defined EGROUPLOOP
    case EGROUPLOOP:      return "EGROUPLOOP";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Haiku, Cygwin */
    #if defined EHOSTDOWN
    case EHOSTDOWN:       return "EHOSTDOWN";
    #endif
    /* Linux */
    #if defined EHWPOISON
    case EHWPOISON:       return "EHWPOISON";
    #endif
    /* GNU/Hurd */
    #if defined EIEIO
    case EIEIO:           return "EIEIO";
    #endif
    /* IRIX */
    #if defined EINIT
    case EINIT:           return "EINIT";
    #endif
    /* OSF/1 */
    #if defined EINPROG
    case EINPROG:         return "EINPROG";
    #endif
    /* IRIX */
    #if defined EINVALMODE
    case EINVALMODE:      return "EINVALMODE";
    #endif
    /* IRIX */
    #if defined EINVALSTATE
    case EINVALSTATE:     return "EINVALSTATE";
    #endif
    /* IRIX */
    #if defined EINVALTIME
    case EINVALTIME:      return "EINVALTIME";
    #endif
    /* IRIX */
    #if defined EIORESID
    case EIORESID:        return "EIORESID";
    #endif
    /* OpenBSD */
    #if defined EIPSEC
    case EIPSEC:          return "EIPSEC";
    #endif
    /* Linux, IRIX */
    #if defined EISNAM
    case EISNAM:          return "EISNAM";
    #endif
    /* IRIX */
    #if defined EJOINED
    case EJOINED:         return "EJOINED";
    #endif
    /* FreeBSD, OSF/1 */
    #if defined EJUSTRETURN
    case EJUSTRETURN:     return "EJUSTRETURN";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_ABORTED
    case EKERN_ABORTED:   return "EKERN_ABORTED";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_FAILURE
    case EKERN_FAILURE:   return "EKERN_FAILURE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INTERRUPTED
    case EKERN_INTERRUPTED:return "EKERN_INTERRUPTED";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_ADDRESS
    case EKERN_INVALID_ADDRESS:return "EKERN_INVALID_ADDRESS";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_ARGUMENT
    case EKERN_INVALID_ARGUMENT:return "EKERN_INVALID_ARGUMENT";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_CAPABILITY
    case EKERN_INVALID_CAPABILITY:return "EKERN_INVALID_CAPABILITY";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_HOST
    case EKERN_INVALID_HOST:return "EKERN_INVALID_HOST";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_NAME
    case EKERN_INVALID_NAME:return "EKERN_INVALID_NAME";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_RIGHT
    case EKERN_INVALID_RIGHT:return "EKERN_INVALID_RIGHT";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_TASK
    case EKERN_INVALID_TASK:return "EKERN_INVALID_TASK";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_INVALID_VALUE
    case EKERN_INVALID_VALUE:return "EKERN_INVALID_VALUE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_MEMORY_ERROR
    case EKERN_MEMORY_ERROR:return "EKERN_MEMORY_ERROR";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_MEMORY_FAILURE
    case EKERN_MEMORY_FAILURE:return "EKERN_MEMORY_FAILURE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_MEMORY_PRESENT
    case EKERN_MEMORY_PRESENT:return "EKERN_MEMORY_PRESENT";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_NAME_EXISTS
    case EKERN_NAME_EXISTS:return "EKERN_NAME_EXISTS";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_NOT_IN_SET
    case EKERN_NOT_IN_SET:return "EKERN_NOT_IN_SET";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_NOT_RECEIVER
    case EKERN_NOT_RECEIVER:return "EKERN_NOT_RECEIVER";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_NO_ACCESS
    case EKERN_NO_ACCESS: return "EKERN_NO_ACCESS";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_NO_SPACE
    case EKERN_NO_SPACE:  return "EKERN_NO_SPACE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_PROTECTION_FAILURE
    case EKERN_PROTECTION_FAILURE:return "EKERN_PROTECTION_FAILURE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_RESOURCE_SHORTAGE
    case EKERN_RESOURCE_SHORTAGE:return "EKERN_RESOURCE_SHORTAGE";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_RIGHT_EXISTS
    case EKERN_RIGHT_EXISTS:return "EKERN_RIGHT_EXISTS";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_TERMINATED
    case EKERN_TERMINATED:return "EKERN_TERMINATED";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_TIMEDOUT
    case EKERN_TIMEDOUT:  return "EKERN_TIMEDOUT";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_UREFS_OVERFLOW
    case EKERN_UREFS_OVERFLOW:return "EKERN_UREFS_OVERFLOW";
    #endif
    /* GNU/Hurd */
    #if defined EKERN_WRITE_PROTECTION_FAILURE
    case EKERN_WRITE_PROTECTION_FAILURE:return "EKERN_WRITE_PROTECTION_FAILURE";
    #endif
    /* Linux */
    #if defined EKEYEXPIRED
    case EKEYEXPIRED:     return "EKEYEXPIRED";
    #endif
    /* Linux */
    #if defined EKEYREJECTED
    case EKEYREJECTED:    return "EKEYREJECTED";
    #endif
    /* Linux */
    #if defined EKEYREVOKED
    case EKEYREVOKED:     return "EKEYREVOKED";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EL2HLT
    case EL2HLT:          return "EL2HLT";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EL2NSYNC
    case EL2NSYNC:        return "EL2NSYNC";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EL3HLT
    case EL3HLT:          return "EL3HLT";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EL3RST
    case EL3RST:          return "EL3RST";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
    #if defined ELAST && 0
    case ELAST:           return "ELAST";
    #endif
    /* Cygwin */
    #if defined ELBIN
    case ELBIN:           return "ELBIN";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ELIBACC
    case ELIBACC:         return "ELIBACC";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ELIBBAD
    case ELIBBAD:         return "ELIBBAD";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ELIBEXEC
    case ELIBEXEC:        return "ELIBEXEC";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ELIBMAX
    case ELIBMAX:         return "ELIBMAX";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ELIBSCN
    case ELIBSCN:         return "ELIBSCN";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ELNRNG
    case ELNRNG:          return "ELNRNG";
    #endif
    /* Minix */
    #if defined ELOCKED
    case ELOCKED:         return "ELOCKED";
    #endif
    /* Solaris */
    #if defined ELOCKUNMAPPED
    case ELOCKUNMAPPED:   return "ELOCKUNMAPPED";
    #endif
    /* IRIX */
    #if defined ELOGINLIM
    case ELOGINLIM:       return "ELOGINLIM";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_BODY_ERROR
    case EMACH_RCV_BODY_ERROR:return "EMACH_RCV_BODY_ERROR";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_HEADER_ERROR
    case EMACH_RCV_HEADER_ERROR:return "EMACH_RCV_HEADER_ERROR";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_INTERRUPTED
    case EMACH_RCV_INTERRUPTED:return "EMACH_RCV_INTERRUPTED";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_INVALID_DATA
    case EMACH_RCV_INVALID_DATA:return "EMACH_RCV_INVALID_DATA";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_INVALID_NAME
    case EMACH_RCV_INVALID_NAME:return "EMACH_RCV_INVALID_NAME";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_INVALID_NOTIFY
    case EMACH_RCV_INVALID_NOTIFY:return "EMACH_RCV_INVALID_NOTIFY";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_IN_PROGRESS
    case EMACH_RCV_IN_PROGRESS:return "EMACH_RCV_IN_PROGRESS";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_IN_SET
    case EMACH_RCV_IN_SET:return "EMACH_RCV_IN_SET";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_PORT_CHANGED
    case EMACH_RCV_PORT_CHANGED:return "EMACH_RCV_PORT_CHANGED";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_PORT_DIED
    case EMACH_RCV_PORT_DIED:return "EMACH_RCV_PORT_DIED";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_TIMED_OUT
    case EMACH_RCV_TIMED_OUT:return "EMACH_RCV_TIMED_OUT";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_RCV_TOO_LARGE
    case EMACH_RCV_TOO_LARGE:return "EMACH_RCV_TOO_LARGE";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INTERRUPTED
    case EMACH_SEND_INTERRUPTED:return "EMACH_SEND_INTERRUPTED";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_DATA
    case EMACH_SEND_INVALID_DATA:return "EMACH_SEND_INVALID_DATA";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_DEST
    case EMACH_SEND_INVALID_DEST:return "EMACH_SEND_INVALID_DEST";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_HEADER
    case EMACH_SEND_INVALID_HEADER:return "EMACH_SEND_INVALID_HEADER";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_MEMORY
    case EMACH_SEND_INVALID_MEMORY:return "EMACH_SEND_INVALID_MEMORY";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_NOTIFY
    case EMACH_SEND_INVALID_NOTIFY:return "EMACH_SEND_INVALID_NOTIFY";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_REPLY
    case EMACH_SEND_INVALID_REPLY:return "EMACH_SEND_INVALID_REPLY";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_RIGHT
    case EMACH_SEND_INVALID_RIGHT:return "EMACH_SEND_INVALID_RIGHT";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_INVALID_TYPE
    case EMACH_SEND_INVALID_TYPE:return "EMACH_SEND_INVALID_TYPE";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_IN_PROGRESS
    case EMACH_SEND_IN_PROGRESS:return "EMACH_SEND_IN_PROGRESS";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_MSG_TOO_SMALL
    case EMACH_SEND_MSG_TOO_SMALL:return "EMACH_SEND_MSG_TOO_SMALL";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_NOTIFY_IN_PROGRESS
    case EMACH_SEND_NOTIFY_IN_PROGRESS:return "EMACH_SEND_NOTIFY_IN_PROGRESS";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_NO_BUFFER
    case EMACH_SEND_NO_BUFFER:return "EMACH_SEND_NO_BUFFER";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_NO_NOTIFY
    case EMACH_SEND_NO_NOTIFY:return "EMACH_SEND_NO_NOTIFY";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_TIMED_OUT
    case EMACH_SEND_TIMED_OUT:return "EMACH_SEND_TIMED_OUT";
    #endif
    /* GNU/Hurd */
    #if defined EMACH_SEND_WILL_NOTIFY
    case EMACH_SEND_WILL_NOTIFY:return "EMACH_SEND_WILL_NOTIFY";
    #endif
    /* AIX, OSF/1 */
    #if defined EMEDIA
    case EMEDIA:          return "EMEDIA";
    #endif
    /* Linux, OpenBSD */
    #if defined EMEDIUMTYPE
    case EMEDIUMTYPE:     return "EMEDIUMTYPE";
    #endif
    /* IRIX */
    #if defined EMEMRETRY
    case EMEMRETRY:       return "EMEMRETRY";
    #endif
    /* IRIX */
    #if defined EMIGRATED
    case EMIGRATED:       return "EMIGRATED";
    #endif
    /* IRIX */
    #if defined EMIGRATING
    case EMIGRATING:      return "EMIGRATING";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_ARRAY_TOO_LARGE
    case EMIG_ARRAY_TOO_LARGE:return "EMIG_ARRAY_TOO_LARGE";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_BAD_ARGUMENTS
    case EMIG_BAD_ARGUMENTS:return "EMIG_BAD_ARGUMENTS";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_BAD_ID
    case EMIG_BAD_ID:     return "EMIG_BAD_ID";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_DESTROY_REQUEST
    case EMIG_DESTROY_REQUEST:return "EMIG_DESTROY_REQUEST";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_EXCEPTION
    case EMIG_EXCEPTION:  return "EMIG_EXCEPTION";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_NO_REPLY
    case EMIG_NO_REPLY:   return "EMIG_NO_REPLY";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_REMOTE_ERROR
    case EMIG_REMOTE_ERROR:return "EMIG_REMOTE_ERROR";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_REPLY_MISMATCH
    case EMIG_REPLY_MISMATCH:return "EMIG_REPLY_MISMATCH";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_SERVER_DIED
    case EMIG_SERVER_DIED:return "EMIG_SERVER_DIED";
    #endif
    /* GNU/Hurd */
    #if defined EMIG_TYPE_ERROR
    case EMIG_TYPE_ERROR: return "EMIG_TYPE_ERROR";
    #endif
    /* OSF/1 */
    #if defined EMTIMERS
    case EMTIMERS:        return "EMTIMERS";
    #endif
    /* IRIX */
    #if defined EMUSTRUN
    case EMUSTRUN:        return "EMUSTRUN";
    #endif
    /* Linux, IRIX */
    #if defined ENAVAIL
    case ENAVAIL:         return "ENAVAIL";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
    #if defined ENEEDAUTH
    case ENEEDAUTH:       return "ENEEDAUTH";
    #endif
    /* IRIX */
    #if defined ENFSREMOTE
    case ENFSREMOTE:      return "ENFSREMOTE";
    #endif
    /* Cygwin */
    #if defined ENMFILE
    case ENMFILE:         return "ENMFILE";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ENOANO
    case ENOANO:          return "ENOANO";
    #endif
    /* IRIX */
    #if defined ENOATTACH
    case ENOATTACH:       return "ENOATTACH";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, IRIX, Minix */
    #if defined ENOATTR
    case ENOATTR:         return "ENOATTR";
    #endif
    /* IRIX */
    #if defined ENOBWD
    case ENOBWD:          return "ENOBWD";
    #endif
    /* MirBSD */
    #if defined ENOCOFFEE
    case ENOCOFFEE:       return "ENOCOFFEE";
    #endif
    /* Minix */
    #if defined ENOCONN
    case ENOCONN:         return "ENOCONN";
    #endif
    /* AIX */
    #if defined ENOCONNECT
    case ENOCONNECT:      return "ENOCONNECT";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ENOCSI
    case ENOCSI:          return "ENOCSI";
    #endif
    /* IRIX */
    #if defined ENOEXIST
    case ENOEXIST:        return "ENOEXIST";
    #endif
    /* IRIX */
    #if defined ENOINTRGROUP
    case ENOINTRGROUP:    return "ENOINTRGROUP";
    #endif
    /* FreeBSD */
    #if defined ENOIOCTL
    case ENOIOCTL:        return "ENOIOCTL";
    #endif
    /* Linux */
    #if defined ENOKEY
    case ENOKEY:          return "ENOKEY";
    #endif
    /* IRIX */
    #if defined ENOLIMFILE
    case ENOLIMFILE:      return "ENOLIMFILE";
    #endif
    /* HP-UX */
    #if defined ENOLOAD
    case ENOLOAD:         return "ENOLOAD";
    #endif
    /* IRIX */
    #if defined ENOLOGIN
    case ENOLOGIN:        return "ENOLOGIN";
    #endif
    /* HP-UX */
    #if defined ENOMATCH
    case ENOMATCH:        return "ENOMATCH";
    #endif
    /* Linux, OpenBSD, Cygwin */
    #if defined ENOMEDIUM
    case ENOMEDIUM:       return "ENOMEDIUM";
    #endif
    /* IRIX */
    #if defined ENOMESSAGE
    case ENOMESSAGE:      return "ENOMESSAGE";
    #endif
    /* Linux, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ENONET
    case ENONET:          return "ENONET";
    #endif
    /* Linux, HP-UX, IRIX, OSF/1, Solaris, Cygwin */
    #if defined ENOPKG
    case ENOPKG:          return "ENOPKG";
    #endif
    /* Mac OS X */
    #if defined ENOPOLICY
    case ENOPOLICY:       return "ENOPOLICY";
    #endif
    /* IRIX */
    #if defined ENOPROC
    case ENOPROC:         return "ENOPROC";
    #endif
    /* HP-UX */
    #if defined ENOREG
    case ENOREG:          return "ENOREG";
    #endif
    /* IRIX */
    #if defined ENOSERVICE
    case ENOSERVICE:      return "ENOSERVICE";
    #endif
    /* Cygwin */
    #if defined ENOSHARE
    case ENOSHARE:        return "ENOSHARE";
    #endif
    /* HP-UX, OSF/1 */
    #if defined ENOSYM
    case ENOSYM:          return "ENOSYM";
    #endif
    /* Solaris */
    #if defined ENOTACTIVE
    case ENOTACTIVE:      return "ENOTACTIVE";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Cygwin */
    #if defined ENOTBLK
    case ENOTBLK:         return "ENOTBLK";
    #endif
    /* FreeBSD */
    #if defined ENOTCAPABLE
    case ENOTCAPABLE:     return "ENOTCAPABLE";
    #endif
    /* IRIX */
    #if defined ENOTCONTROLLER
    case ENOTCONTROLLER:  return "ENOTCONTROLLER";
    #endif
    /* IRIX */
    #if defined ENOTENQUEUED
    case ENOTENQUEUED:    return "ENOTENQUEUED";
    #endif
    /* IRIX */
    #if defined ENOTJOINED
    case ENOTJOINED:      return "ENOTJOINED";
    #endif
    /* Linux, IRIX */
    #if defined ENOTNAM
    case ENOTNAM:         return "ENOTNAM";
    #endif
    /* AIX, Minix */
    #if defined ENOTREADY
    case ENOTREADY:       return "ENOTREADY";
    #endif
    /* AIX */
    #if defined ENOTRUST
    case ENOTRUST:        return "ENOTRUST";
    #endif
    /* IRIX */
    #if defined ENOTSTOPPED
    case ENOTSTOPPED:     return "ENOTSTOPPED";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ENOTUNIQ
    case ENOTUNIQ:        return "ENOTUNIQ";
    #endif
    /* HP-UX */
    #if defined ENOUNLD
    case ENOUNLD:         return "ENOUNLD";
    #endif
    /* HP-UX */
    #if defined ENOUNREG
    case ENOUNREG:        return "ENOUNREG";
    #endif
    /* Minix */
    #if defined ENOURG
    case ENOURG:          return "ENOURG";
    #endif
    /* native Windows */
    #if defined EOTHER
    case EOTHER:          return "EOTHER";
    #endif
    /* Minix */
    #if defined EPACKSIZE
    case EPACKSIZE:       return "EPACKSIZE";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Haiku, Cygwin */
    #if defined EPFNOSUPPORT
    case EPFNOSUPPORT:    return "EPFNOSUPPORT";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, IRIX, OSF/1, Minix, Cygwin */
    #if defined EPROCLIM
    case EPROCLIM:        return "EPROCLIM";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix */
    #if defined EPROCUNAVAIL
    case EPROCUNAVAIL:    return "EPROCUNAVAIL";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix */
    #if defined EPROGMISMATCH
    case EPROGMISMATCH:   return "EPROGMISMATCH";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix */
    #if defined EPROGUNAVAIL
    case EPROGUNAVAIL:    return "EPROGUNAVAIL";
    #endif
    /* Mac OS X */
    #if defined EPWROFF
    case EPWROFF:         return "EPWROFF";
    #endif
    /* Mac OS X */
    #if defined EQFULL
    case EQFULL:          return "EQFULL";
    #endif
    /* HP-UX */
    #if defined ERELOC
    case ERELOC:          return "ERELOC";
    #endif
    /* OSF/1 */
    #if defined ERELOCATED
    case ERELOCATED:      return "ERELOCATED";
    #endif
    /* FreeBSD */
    #if defined ERELOOKUP
    case ERELOOKUP:       return "ERELOOKUP";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EREMCHG
    case EREMCHG:         return "EREMCHG";
    #endif
    /* IRIX */
    #if defined EREMDEV
    case EREMDEV:         return "EREMDEV";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Haiku, Cygwin */
    #if defined EREMOTE
    case EREMOTE:         return "EREMOTE";
    #endif
    /* Linux, IRIX */
    #if defined EREMOTEIO
    case EREMOTEIO:       return "EREMOTEIO";
    #endif
    /* HP-UX */
    #if defined EREMOTERELEASE
    case EREMOTERELEASE:  return "EREMOTERELEASE";
    #endif
    /* Linux, FreeBSD, AIX, IRIX, OSF/1, Solaris, Minix */
    #if defined ERESTART
    case ERESTART:        return "ERESTART";
    #endif
    /* Linux */
    #if defined ERFKILL
    case ERFKILL:         return "ERFKILL";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, OSF/1, Minix */
    #if defined ERPCMISMATCH
    case ERPCMISMATCH:    return "ERPCMISMATCH";
    #endif
    /* AIX */
    #if defined ESAD
    case ESAD:            return "ESAD";
    #endif
    /* Mac OS X */
    #if defined ESHLIBVERS
    case ESHLIBVERS:      return "ESHLIBVERS";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Haiku, Cygwin */
    #if defined ESHUTDOWN
    case ESHUTDOWN:       return "ESHUTDOWN";
    #endif
    /* Haiku */
    #if defined ESIGPARM
    case ESIGPARM:        return "ESIGPARM";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Cygwin */
    #if defined ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT: return "ESOCKTNOSUPPORT";
    #endif
    /* AIX, OSF/1 */
    #if defined ESOFT
    case ESOFT:           return "ESOFT";
    #endif
    /* Linux, HP-UX, IRIX, Solaris, Cygwin */
    #if defined ESRMNT
    case ESRMNT:          return "ESRMNT";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined ESTRPIPE
    case ESTRPIPE:        return "ESTRPIPE";
    #endif
    /* OSF/1 */
    #if defined ESUCCESS
    case ESUCCESS:        return "ESUCCESS";
    #endif
    /* AIX */
    #if defined ESYSERROR
    case ESYSERROR:       return "ESYSERROR";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Cygwin */
    #if defined ETOOMANYREFS
    case ETOOMANYREFS:    return "ETOOMANYREFS";
    #endif
    /* Minix */
    #if defined ETRAPDENIED
    case ETRAPDENIED:     return "ETRAPDENIED";
    #endif
    /* Linux, IRIX */
    #if defined EUCLEAN
    case EUCLEAN:         return "EUCLEAN";
    #endif
    /* Linux, AIX, HP-UX, IRIX, Solaris, Cygwin */
    #if defined EUNATCH
    case EUNATCH:         return "EUNATCH";
    #endif
    /* Minix */
    #if defined EURG
    case EURG:            return "EURG";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, IRIX, OSF/1, Solaris, Minix, Cygwin */
    #if defined EUSERS
    case EUSERS:          return "EUSERS";
    #endif
    /* OSF/1 */
    #if defined EVERSION
    case EVERSION:        return "EVERSION";
    #endif
    /* IRIX */
    #if defined EWRONGFS
    case EWRONGFS:        return "EWRONGFS";
    #endif
    /* AIX */
    #if defined EWRPROTECT
    case EWRPROTECT:      return "EWRPROTECT";
    #endif
    /* Linux, IRIX, Solaris, Cygwin */
    #if defined EXFULL
    case EXFULL:          return "EXFULL";
    #endif

    default:
      return NULL;
    }
}
