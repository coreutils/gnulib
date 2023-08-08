/* Declarations for GNU's read utmp module.

   Copyright (C) 1992-2007, 2009-2023 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by jla; revised by djm */

#ifndef __READUTMP_H__
#define __READUTMP_H__

/* This file uses _GL_ATTRIBUTE_MALLOC, _GL_ATTRIBUTE_RETURNS_NONNULL,
   HAVE_UTMP_H, HAVE_UTMPX_H, HAVE_STRUCT_UTMP_*, HAVE_STRUCT_UTMPX_*,
   HAVE_UTMPNAME, HAVE_UTMPXNAME, HAVE_DECL_GETUTENT.  */
#if !_GL_CONFIG_H_INCLUDED
# error "Please include config.h first."
#endif

#include "idx.h"

#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

/* AIX 4.3.3 has both utmp.h and utmpx.h, but only struct utmp
   has the ut_exit member.  */
#if (HAVE_UTMPX_H && HAVE_UTMP_H && HAVE_STRUCT_UTMP_UT_EXIT \
     && ! HAVE_STRUCT_UTMPX_UT_EXIT)
# undef HAVE_UTMPX_H
#endif

/* HPUX 10.20 needs utmp.h, for the definition of e.g., UTMP_FILE.  */
#if HAVE_UTMP_H
# include <utmp.h>
#endif

/* Needed for BOOT_TIME and USER_PROCESS.  */
#if HAVE_UTMPX_H
# if defined _THREAD_SAFE && defined UTMP_DATA_INIT
    /* When including both utmp.h and utmpx.h on AIX 4.3, with _THREAD_SAFE
       defined, work around the duplicate struct utmp_data declaration.  */
#  define utmp_data gl_aix_4_3_workaround_utmp_data
# endif
# include <utmpx.h>
#endif

#if READUTMP_USE_SYSTEMD || ! (HAVE_UTMPX_H || HAVE_UTMP_H)

struct gl_utmp
{
  /* All 'char *' here are of arbitrary length and point to storage
     with lifetime equal to that of this struct.  */
  char *ut_user;                /* User name */
  char *ut_id;                  /* Session ID */
  char *ut_line;                /* seat / device */
  char *ut_host;                /* for remote sessions: user@host or host */
  struct timespec ut_ts;        /* time */
  pid_t ut_pid;                 /* process ID of ? */
  pid_t ut_session;             /* process ID of session leader */
  short ut_type;                /* BOOT_TIME or USER_PROCESS */
};

# define HAVE_GL_UTMP 1
# define UTMP_STRUCT_NAME gl_utmp
# define UT_TIME_MEMBER(UT) ((UT)->ut_ts.tv_sec)
# define UT_EXIT_E_TERMINATION(UT) 0
# define UT_EXIT_E_EXIT(UT) 0

#elif HAVE_UTMPX_H

/* <utmpx.h> defines 'struct utmpx' with the following fields:

     Field        Type                       Platforms
     ----------   ------                     ---------
   ⎡ ut_user      char[]                     glibc, musl, macOS, FreeBSD, AIX, HP-UX, IRIX, Solaris, Cygwin
   ⎣ ut_name      char[]                     NetBSD, Minix
     ut_id        char[]                     glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
     ut_line      char[]                     glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
     ut_pid       pid_t                      glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
     ut_type      short                      glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
   ⎡ ut_tv        struct                     glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
   ⎢              { tv_sec; tv_usec; }
   ⎣ ut_time      time_t                     Cygwin
     ut_host      char[]                     glibc, musl, macOS, FreeBSD, NetBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
     ut_exit      struct                     glibc, musl, NetBSD, Minix, HP-UX, IRIX, Solaris
                  { e_termination; e_exit; }
     ut_session   [long] int                 glibc, musl, NetBSD, Minix, IRIX, Solaris
   ⎡ ut_addr      [long] int                 HP-UX, Cygwin
   ⎢ ut_addr_v6   [u]int[4]                  glibc, musl
   ⎣ ut_ss        struct sockaddr_storage    NetBSD, Minix
 */

# define UTMP_STRUCT_NAME utmpx
# define UT_TIME_MEMBER(UT) ((UT)->ut_tv.tv_sec)
# define SET_UTMP_ENT setutxent
# define GET_UTMP_ENT getutxent
# define END_UTMP_ENT endutxent
# ifdef HAVE_UTMPXNAME
#  define UTMP_NAME_FUNCTION utmpxname
# elif defined UTXDB_ACTIVE
#  define UTMP_NAME_FUNCTION(x) setutxdb (UTXDB_ACTIVE, x)
# endif

# if HAVE_STRUCT_UTMPX_UT_EXIT_E_TERMINATION
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.e_termination)
# elif HAVE_STRUCT_UTMPX_UT_EXIT_UT_TERMINATION /* OSF/1 */
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.ut_termination)
# else
#  define UT_EXIT_E_TERMINATION(UT) 0
# endif

# if HAVE_STRUCT_UTMPX_UT_EXIT_E_EXIT
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.e_exit)
# elif HAVE_STRUCT_UTMPX_UT_EXIT_UT_EXIT /* OSF/1 */
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.ut_exit)
# else
#  define UT_EXIT_E_EXIT(UT) 0
# endif

#elif HAVE_UTMP_H

/* <utmp.h> defines 'struct utmp' with the following fields:

     Field        Type                       Platforms
     ----------   ------                     ---------
   ⎡ ut_user      char[]                     glibc, musl, AIX, HP-UX, IRIX, Solaris, Cygwin, Android
   ⎣ ut_name      char[]                     macOS, old FreeBSD, NetBSD, OpenBSD, Minix
     ut_id        char[]                     glibc, musl, AIX, HP-UX, IRIX, Solaris, Cygwin, Android
     ut_line      char[]                     glibc, musl, macOS, old FreeBSD, NetBSD, OpenBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin, Android
     ut_pid       pid_t                      glibc, musl, AIX, HP-UX, IRIX, Solaris, Cygwin, Android
     ut_type      short                      glibc, musl, AIX, HP-UX, IRIX, Solaris, Cygwin, Android
   ⎡ ut_tv        struct                     glibc, musl, Android
   ⎢              { tv_sec; tv_usec; }
   ⎣ ut_time      time_t                     macOS, old FreeBSD, NetBSD, OpenBSD, Minix, AIX, HP-UX, IRIX, Solaris, Cygwin
     ut_host      char[]                     glibc, musl, macOS, old FreeBSD, NetBSD, OpenBSD, Minix, AIX, HP-UX, Cygwin, Android
     ut_exit      struct                     glibc, musl, AIX, HP-UX, IRIX, Solaris, Android
                  { e_termination; e_exit; }
     ut_session   [long] int                 glibc, musl, Android
   ⎡ ut_addr      [long] int                 HP-UX, Cygwin
   ⎣ ut_addr_v6   [u]int[4]                  glibc, musl, Android
 */

# if !HAVE_DECL_GETUTENT
    struct utmp *getutent (void);
# endif
# define UTMP_STRUCT_NAME utmp
# define UT_TIME_MEMBER(UT) ((UT)->ut_ts.tv_sec)
# define SET_UTMP_ENT setutent
# define GET_UTMP_ENT getutent
# define END_UTMP_ENT endutent
# ifdef HAVE_UTMPNAME
#  define UTMP_NAME_FUNCTION utmpname
# endif

# if HAVE_STRUCT_UTMP_UT_EXIT_E_TERMINATION
#  define UT_EXIT_E_TERMINATION(UT) ((UT)->ut_exit.e_termination)
# else
#  define UT_EXIT_E_TERMINATION(UT) 0
# endif

# if HAVE_STRUCT_UTMP_UT_EXIT_E_EXIT
#  define UT_EXIT_E_EXIT(UT) ((UT)->ut_exit.e_exit)
# else
#  define UT_EXIT_E_EXIT(UT) 0
# endif

#endif

/* Accessor macro for the member named ut_user or ut_name.  */
#if (!HAVE_GL_UTMP \
     && (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_NAME \
         : HAVE_UTMP_H && HAVE_STRUCT_UTMP_UT_NAME))
# define UT_USER(UT) ((UT)->ut_name)
#else
# define UT_USER(UT) ((UT)->ut_user)
#endif

#define HAVE_STRUCT_XTMP_UT_EXIT \
  (!HAVE_GL_UTMP && (HAVE_STRUCT_UTMP_UT_EXIT || HAVE_STRUCT_UTMPX_UT_EXIT)

#define HAVE_STRUCT_XTMP_UT_ID \
  (HAVE_GL_UTMP || HAVE_STRUCT_UTMP_UT_ID || HAVE_STRUCT_UTMPX_UT_ID)

#define HAVE_STRUCT_XTMP_UT_PID \
  (HAVE_GL_UTMP || HAVE_STRUCT_UTMP_UT_PID || HAVE_STRUCT_UTMPX_UT_PID)

#define HAVE_STRUCT_XTMP_UT_HOST \
  (HAVE_GL_UTMP || HAVE_STRUCT_UTMP_UT_HOST || HAVE_STRUCT_UTMPX_UT_HOST)

/* Type of entry returned by read_utmp().  */
typedef struct UTMP_STRUCT_NAME STRUCT_UTMP;

/* Size of the UT_USER (ut) member, or -1 if unbounded.  */
#if HAVE_GL_UTMP
enum { UT_USER_SIZE = -1 };
#else
enum { UT_USER_SIZE = sizeof UT_USER ((STRUCT_UTMP *) 0) };
# define UT_USER_SIZE UT_USER_SIZE
#endif

/* Size of the ut->ut_id member, or -1 if unbounded.  */
#if HAVE_GL_UTMP
enum { UT_ID_SIZE = -1 };
#else
# if (HAVE_UTMPX_H ? HAVE_STRUCT_UTMPX_UT_ID : HAVE_STRUCT_UTMP_UT_ID)
enum { UT_ID_SIZE = sizeof (((STRUCT_UTMP *) 0)->ut_id) };
# else
enum { UT_ID_SIZE = 1 };
# endif
# define UT_ID_SIZE UT_ID_SIZE
#endif

/* Size of the ut->ut_line member, or -1 if unbounded.  */
#if HAVE_GL_UTMP
enum { UT_LINE_SIZE = -1 };
#else
enum { UT_LINE_SIZE = sizeof (((STRUCT_UTMP *) 0)->ut_line) };
# define UT_LINE_SIZE UT_LINE_SIZE
#endif

/* Size of the ut->ut_host member, or -1 if unbounded.  */
#if HAVE_GL_UTMP
enum { UT_HOST_SIZE = -1 };
#else
enum { UT_HOST_SIZE = sizeof (((STRUCT_UTMP *) 0)->ut_host) };
# define UT_HOST_SIZE UT_HOST_SIZE
#endif

/* Definition of UTMP_FILE and WTMP_FILE.  */

#if !defined UTMP_FILE && defined _PATH_UTMP
# define UTMP_FILE _PATH_UTMP
#endif

#if !defined WTMP_FILE && defined _PATH_WTMP
# define WTMP_FILE _PATH_WTMP
#endif

#ifdef UTMPX_FILE /* Solaris, SysVr4 */
# undef UTMP_FILE
# define UTMP_FILE UTMPX_FILE
#endif

#ifdef WTMPX_FILE /* Solaris, SysVr4 */
# undef WTMP_FILE
# define WTMP_FILE WTMPX_FILE
#endif

#ifndef UTMP_FILE
# define UTMP_FILE "/etc/utmp"
#endif

#ifndef WTMP_FILE
# define WTMP_FILE "/etc/wtmp"
#endif

/* Accessor macro for the member named ut_pid.  */
#if HAVE_STRUCT_XTMP_UT_PID
# define UT_PID(UT) ((UT)->ut_pid)
#else
# define UT_PID(UT) 0
#endif

/* Accessor macros for the member named ut_type.  */

#if HAVE_GL_UTMP || HAVE_STRUCT_UTMP_UT_TYPE || HAVE_STRUCT_UTMPX_UT_TYPE
# define UT_TYPE_EQ(UT, V) ((UT)->ut_type == (V))
# define UT_TYPE_NOT_DEFINED 0
#else
# define UT_TYPE_EQ(UT, V) 0
# define UT_TYPE_NOT_DEFINED 1
#endif

#ifdef BOOT_TIME
# define UT_TYPE_BOOT_TIME(UT) UT_TYPE_EQ (UT, BOOT_TIME)
#else
# define UT_TYPE_BOOT_TIME(UT) 0
#endif

#ifdef USER_PROCESS
# define UT_TYPE_USER_PROCESS(UT) UT_TYPE_EQ (UT, USER_PROCESS)
#else
# define UT_TYPE_USER_PROCESS(UT) 0
#endif

/* Determines whether an entry *UT corresponds to a user process.  */
#define IS_USER_PROCESS(UT)                                    \
   (UT_USER (UT)[0]                                             \
    && (UT_TYPE_USER_PROCESS (UT)                               \
        || (UT_TYPE_NOT_DEFINED && UT_TIME_MEMBER (UT) != 0)))

/* Define if read_utmp is not just a dummy.  */
#if READUTMP_USE_SYSTEMD || HAVE_UTMPX_H || HAVE_UTMP_H
# define READ_UTMP_SUPPORTED 1
#endif

/* Options for read_utmp.  */
enum
  {
    READ_UTMP_CHECK_PIDS = 1,
    READ_UTMP_USER_PROCESS = 2
  };

/* Return a copy of UT_USER (UT), without trailing spaces,
   as a freshly allocated string.  */
char *extract_trimmed_name (const STRUCT_UTMP *ut)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE
  _GL_ATTRIBUTE_RETURNS_NONNULL;

/* Read the utmp entries corresponding to file FILE into freshly-
   malloc'd storage, set *UTMP_BUF to that pointer, set *N_ENTRIES to
   the number of entries, and return zero.  If there is any error,
   return -1, setting errno, and don't modify the parameters.
   A good candidate for FILE is UTMP_FILE.
   If OPTIONS & READ_UTMP_CHECK_PIDS is nonzero, omit entries whose
   process-IDs do not currently exist.
   If OPTIONS & READ_UTMP_USER_PROCESS is nonzero, omit entries which
   do not correspond to a user process.  */
int read_utmp (char const *file, idx_t *n_entries, STRUCT_UTMP **utmp_buf,
               int options);

#endif /* __READUTMP_H__ */
