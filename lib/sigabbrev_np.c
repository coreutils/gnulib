/* Abbreviated name of signals.
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

#include <signal.h>

const char *
sigabbrev_np (int sig)
{
  switch (sig)
    {
    /* Signals specified by ISO C.  */
    case SIGABRT:     return "ABRT";
    case SIGFPE:      return "FPE";
    case SIGILL:      return "ILL";
    case SIGINT:      return "INT";
    case SIGSEGV:     return "SEGV";
    case SIGTERM:     return "TERM";

    /* Signals specified by POSIX.
       <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/signal.h.html>  */
    #if defined SIGALRM
    case SIGALRM:     return "ALRM";
    #endif
    #if defined SIGBUS
    case SIGBUS:      return "BUS";
    #endif
    #if defined SIGCHLD
    case SIGCHLD:     return "CHLD";
    #endif
    #if defined SIGCONT
    case SIGCONT:     return "CONT";
    #endif
    #if defined SIGHUP
    case SIGHUP:      return "HUP";
    #endif
    #if defined SIGKILL
    case SIGKILL:     return "KILL";
    #endif
    #if defined SIGPIPE
    case SIGPIPE:     return "PIPE";
    #endif
    #if defined SIGQUIT
    case SIGQUIT:     return "QUIT";
    #endif
    #if defined SIGSTOP
    case SIGSTOP:     return "STOP";
    #endif
    #if defined SIGTSTP
    case SIGTSTP:     return "TSTP";
    #endif
    #if defined SIGTTIN
    case SIGTTIN:     return "TTIN";
    #endif
    #if defined SIGTTOU
    case SIGTTOU:     return "TTOU";
    #endif
    #if defined SIGUSR1
    case SIGUSR1:     return "USR1";
    #endif
    #if defined SIGUSR2
    case SIGUSR2:     return "USR2";
    #endif
    #if defined SIGPOLL
    case SIGPOLL:     return "POLL";
    #endif
    #if defined SIGPROF
    case SIGPROF:     return "PROF";
    #endif
    #if defined SIGSYS
    case SIGSYS:      return "SYS";
    #endif
    #if defined SIGTRAP
    case SIGTRAP:     return "TRAP";
    #endif
    #if defined SIGURG
    case SIGURG:      return "URG";
    #endif
    #if defined SIGVTALRM
    case SIGVTALRM:   return "VTALRM";
    #endif
    #if defined SIGXCPU
    case SIGXCPU:     return "XCPU";
    #endif
    #if defined SIGXFSZ
    case SIGXFSZ:     return "XFSZ";
    #endif

    /* Other signals on other systems.  */
    /* native Windows */
    #if defined SIGBREAK
    case SIGBREAK:    return "BREAK";
    #endif
    /* IRIX */
    #if defined SIGCKPT
    case SIGCKPT:     return "CKPT";
    #endif
    /* Linux, IRIX, Cygwin */
    #if defined SIGCLD && SIGCLD != SIGCHLD
    case SIGCLD:      return "CLD";
    #endif
    /* AIX */
    #if defined SIGCPUFAIL
    case SIGCPUFAIL:  return "CPUFAIL";
    #endif
    /* AIX */
    #if defined SIGDANGER
    case SIGDANGER:   return "DANGER";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix, AIX, IRIX, Cygwin, mingw */
    #if defined SIGEMT
    case SIGEMT:      return "EMT";
    #endif
    /* Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix */
    #if defined SIGINFO && SIGINFO != SIGPWR
    case SIGINFO:     return "INFO";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix, AIX, IRIX, Cygwin */
    #if defined SIGIO && SIGIO != SIGPOLL
    case SIGIO:       return "IO";
    #endif
    /* Linux, IRIX, Cygwin, mingw */
    #if defined SIGIOT && SIGIOT != SIGABRT
    case SIGIOT:      return "IOT";
    #endif
    /* AIX */
    #if defined SIGKAP
    case SIGKAP:      return "KAP";
    #endif
    /* Haiku */
    #if defined SIGKILLTHR
    case SIGKILLTHR:  return "KILLTHR";
    #endif
    /* Minix */
    #if defined SIGKMEM
    case SIGKMEM:     return "KMEM";
    #endif
    /* Minix */
    #if defined SIGKMESS
    case SIGKMESS:    return "KMESS";
    #endif
    /* Minix */
    #if defined SIGKSIG
    case SIGKSIG:     return "KSIG";
    #endif
    /* Minix */
    #if defined SIGKSIGSM
    case SIGKSIGSM:   return "KSIGSM";
    #endif
    /* FreeBSD */
    #if defined SIGLIBRT
    case SIGLIBRT:    return "LIBRT";
    #endif
    /* Cygwin */
    #if defined SIGLOST && SIGLOST != SIGABRT && SIGLOST != SIGPWR
    case SIGLOST:     return "LOST";
    #endif
    /* AIX */
    #if defined SIGMIGRATE
    case SIGMIGRATE:  return "MIGRATE";
    #endif
    /* AIX */
    #if defined SIGMSG
    case SIGMSG:      return "MSG";
    #endif
    /* AIX */
    #if defined SIGPRE
    case SIGPRE:      return "PRE";
    #endif
    /* IRIX */
    #if defined SIGPTINTR
    case SIGPTINTR:   return "PTINTR";
    #endif
    /* IRIX */
    #if defined SIGPTRESCHED
    case SIGPTRESCHED:return "PTRESCHED";
    #endif
    /* Linux, NetBSD, Minix, AIX, IRIX, Cygwin */
    #if defined SIGPWR
    case SIGPWR:      return "PWR";
    #endif
    /* AIX */
    #if defined SIGRECONFIG
    case SIGRECONFIG: return "RECONFIG";
    #endif
    /* AIX */
    #if defined SIGRECOVERY
    case SIGRECOVERY: return "RECOVERY";
    #endif
    /* IRIX */
    #if defined SIGRESTART
    case SIGRESTART:  return "RESTART";
    #endif
    /* AIX */
    #if defined SIGRETRACT
    case SIGRETRACT:  return "RETRACT";
    #endif
    /* AIX */
    #if defined SIGSAK
    case SIGSAK:      return "SAK";
    #endif
    /* Minix */
    #if defined SIGSNDELAY
    case SIGSNDELAY:  return "SNDELAY";
    #endif
    /* AIX */
    #if defined SIGSOUND
    case SIGSOUND:    return "SOUND";
    #endif
    /* Linux */
    #if defined SIGSTKFLT
    case SIGSTKFLT:   return "STKFLT";
    #endif
    /* AIX */
    #if defined SIGSYSERROR
    case SIGSYSERROR: return "SYSERROR";
    #endif
    /* AIX */
    #if defined SIGTALRM
    case SIGTALRM:    return "TALRM";
    #endif
    /* FreeBSD, OpenBSD */
    #if defined SIGTHR
    case SIGTHR:      return "THR";
    #endif
    /* IRIX */
    #if defined SIGUME
    case SIGUME:      return "UME";
    #endif
    /* AIX */
    #if defined SIGVIRT
    case SIGVIRT:     return "VIRT";
    #endif
    /* AIX */
    #if defined SIGWAITING
    case SIGWAITING:  return "WAITING";
    #endif
    /* Linux, Mac OS X, FreeBSD, NetBSD, OpenBSD, Minix, AIX, IRIX, Cygwin, Haiku */
    #if defined SIGWINCH
    case SIGWINCH:    return "WINCH";
    #endif

    default:
      return NULL;
    }
}
