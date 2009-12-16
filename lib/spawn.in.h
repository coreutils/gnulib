/* Definitions for POSIX spawn interface.
   Copyright (C) 2000, 2003-2004, 2008-2010 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#ifndef _GL_SPAWN_H

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_SPAWN_H@
# @INCLUDE_NEXT@ @NEXT_SPAWN_H@
#endif

#ifndef _GL_SPAWN_H
#define _GL_SPAWN_H

/* Get definitions of 'struct sched_param' and 'sigset_t'.
   But avoid namespace pollution on glibc systems.  */
#ifndef __GLIBC__
# include <sched.h>
# include <signal.h>
#endif

#include <sys/types.h>

#ifndef __THROW
# define __THROW
#endif

/* GCC 2.95 and later have "__restrict"; C99 compilers have
   "restrict", and "configure" may have defined "restrict".
   Other compilers use __restrict, __restrict__, and _Restrict, and
   'configure' might #define 'restrict' to those words, so pick a
   different name.  */
#ifndef _Restrict_
# if 199901L <= __STDC_VERSION__
#  define _Restrict_ restrict
# elif 2 < __GNUC__ || (2 == __GNUC__ && 95 <= __GNUC_MINOR__)
#  define _Restrict_ __restrict
# else
#  define _Restrict_
# endif
#endif
/* gcc 3.1 and up support the [restrict] syntax.  Don't trust
   sys/cdefs.h's definition of __restrict_arr, though, as it
   mishandles gcc -ansi -pedantic.  */
#ifndef _Restrict_arr_
# if ((199901L <= __STDC_VERSION__                                      \
       || ((3 < __GNUC__ || (3 == __GNUC__ && 1 <= __GNUC_MINOR__))     \
           && !__STRICT_ANSI__))                                        \
      && !defined __GNUG__)
#  define _Restrict_arr_ _Restrict_
# else
#  define _Restrict_arr_
# endif
#endif

/* The definition of _GL_ARG_NONNULL is copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Data structure to contain attributes for thread creation.  */
#if @REPLACE_POSIX_SPAWN@
# define posix_spawnattr_t rpl_posix_spawnattr_t
#endif
#if @REPLACE_POSIX_SPAWN@ || !@HAVE_POSIX_SPAWNATTR_T@
typedef struct
{
  short int _flags;
  pid_t _pgrp;
  sigset_t _sd;
  sigset_t _ss;
  struct sched_param _sp;
  int _policy;
  int __pad[16];
} posix_spawnattr_t;
#endif


/* Data structure to contain information about the actions to be
   performed in the new process with respect to file descriptors.  */
#if @REPLACE_POSIX_SPAWN@
# define posix_spawn_file_actions_t rpl_posix_spawn_file_actions_t
#endif
#if @REPLACE_POSIX_SPAWN@ || !@HAVE_POSIX_SPAWN_FILE_ACTIONS_T@
typedef struct
{
  int _allocated;
  int _used;
  struct __spawn_action *_actions;
  int __pad[16];
} posix_spawn_file_actions_t;
#endif


/* Flags to be set in the `posix_spawnattr_t'.  */
#if @REPLACE_POSIX_SPAWN@
/* Use the values from the system, for better compatibility.  */
/* But this implementation does not support AIX extensions.  */
# undef POSIX_SPAWN_FORK_HANDLERS
#else
# define POSIX_SPAWN_RESETIDS           0x01
# define POSIX_SPAWN_SETPGROUP          0x02
# define POSIX_SPAWN_SETSIGDEF          0x04
# define POSIX_SPAWN_SETSIGMASK         0x08
# define POSIX_SPAWN_SETSCHEDPARAM      0x10
# define POSIX_SPAWN_SETSCHEDULER       0x20
#endif
/* A GNU extension.  Use the next free bit position.  */
#define POSIX_SPAWN_USEVFORK \
  ((POSIX_SPAWN_RESETIDS | (POSIX_SPAWN_RESETIDS - 1)                   \
    | POSIX_SPAWN_SETPGROUP | (POSIX_SPAWN_SETPGROUP - 1)               \
    | POSIX_SPAWN_SETSIGDEF | (POSIX_SPAWN_SETSIGDEF - 1)               \
    | POSIX_SPAWN_SETSIGMASK | (POSIX_SPAWN_SETSIGMASK - 1)             \
    | POSIX_SPAWN_SETSCHEDPARAM | (POSIX_SPAWN_SETSCHEDPARAM - 1)       \
    | POSIX_SPAWN_SETSCHEDULER | (POSIX_SPAWN_SETSCHEDULER - 1))        \
   + 1)
typedef int verify_POSIX_SPAWN_USEVFORK_no_overlap
            [2 * (((POSIX_SPAWN_RESETIDS | POSIX_SPAWN_SETPGROUP
                    | POSIX_SPAWN_SETSIGDEF | POSIX_SPAWN_SETSIGMASK
                    | POSIX_SPAWN_SETSCHEDPARAM | POSIX_SPAWN_SETSCHEDULER)
                   & POSIX_SPAWN_USEVFORK) == 0) - 1];


#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_POSIX_SPAWN@
/* Spawn a new process executing PATH with the attributes describes in *ATTRP.
   Before running the process perform the actions described in FILE-ACTIONS.

   This function is a possible cancellation points and therefore not
   marked with __THROW. */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn rpl_posix_spawn
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn (pid_t *_Restrict_ __pid,
                        const char *_Restrict_ __path,
                        const posix_spawn_file_actions_t *_Restrict_ __file_actions,
                        const posix_spawnattr_t *_Restrict_ __attrp,
                        char *const argv[_Restrict_arr_],
                        char *const envp[_Restrict_arr_])
     _GL_ARG_NONNULL ((2, 5, 6));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn
# if HAVE_RAW_DECL_POSIX_SPAWN
_GL_WARN_ON_USE (posix_spawn, "posix_spawn is unportable - "
                 "use gnulib module posix_spawn for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNP@
/* Similar to `posix_spawn' but search for FILE in the PATH.

   This function is a possible cancellation points and therefore not
   marked with __THROW.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnp rpl_posix_spawnp
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnp (pid_t *__pid, const char *__file,
                         const posix_spawn_file_actions_t *__file_actions,
                         const posix_spawnattr_t *__attrp,
                         char *const argv[], char *const envp[])
     _GL_ARG_NONNULL ((2, 5, 6));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnp
# if HAVE_RAW_DECL_POSIX_SPAWNP
_GL_WARN_ON_USE (posix_spawnp, "posix_spawnp is unportable - "
                 "use gnulib module posix_spawnp for portability");
# endif
#endif


#if @GNULIB_POSIX_SPAWNATTR_INIT@
/* Initialize data structure with attributes for `spawn' to default values.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_init rpl_posix_spawnattr_init
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_init (posix_spawnattr_t *__attr)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_init
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_INIT
_GL_WARN_ON_USE (posix_spawnattr_init, "posix_spawnattr_init is unportable - "
                 "use gnulib module posix_spawnattr_init for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_DESTROY@
/* Free resources associated with ATTR.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_destroy rpl_posix_spawnattr_destroy
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_destroy (posix_spawnattr_t *__attr)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_destroy
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_DESTROY
_GL_WARN_ON_USE (posix_spawnattr_destroy, "posix_spawnattr_destroy is unportable - "
                 "use gnulib module posix_spawnattr_destroy for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETSIGDEFAULT@
/* Store signal mask for signals with default handling from ATTR in
   SIGDEFAULT.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getsigdefault rpl_posix_spawnattr_getsigdefault
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getsigdefault (const posix_spawnattr_t *_Restrict_ __attr,
                                          sigset_t *_Restrict_ __sigdefault)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getsigdefault
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETSIGDEFAULT
_GL_WARN_ON_USE (posix_spawnattr_getsigdefault, "posix_spawnattr_getsigdefault is unportable - "
                 "use gnulib module posix_spawnattr_getsigdefault for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETSIGDEFAULT@
/* Set signal mask for signals with default handling in ATTR to SIGDEFAULT.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setsigdefault rpl_posix_spawnattr_setsigdefault
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setsigdefault (posix_spawnattr_t *_Restrict_ __attr,
                                          const sigset_t *_Restrict_ __sigdefault)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setsigdefault
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETSIGDEFAULT
_GL_WARN_ON_USE (posix_spawnattr_setsigdefault, "posix_spawnattr_setsigdefault is unportable - "
                 "use gnulib module posix_spawnattr_setsigdefault for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETSIGMASK@
/* Store signal mask for the new process from ATTR in SIGMASK.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getsigmask rpl_posix_spawnattr_getsigmask
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getsigmask (const posix_spawnattr_t *_Restrict_ __attr,
                                       sigset_t *_Restrict_ __sigmask)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getsigmask
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETSIGMASK
_GL_WARN_ON_USE (posix_spawnattr_getsigmask, "posix_spawnattr_getsigmask is unportable - "
                 "use gnulib module posix_spawnattr_getsigmask for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETSIGMASK@
/* Set signal mask for the new process in ATTR to SIGMASK.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setsigmask rpl_posix_spawnattr_setsigmask
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setsigmask (posix_spawnattr_t *_Restrict_ __attr,
                                       const sigset_t *_Restrict_ __sigmask)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setsigmask
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETSIGMASK
_GL_WARN_ON_USE (posix_spawnattr_setsigmask, "posix_spawnattr_setsigmask is unportable - "
                 "use gnulib module posix_spawnattr_setsigmask for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETFLAGS@
/* Get flag word from the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getflags rpl_posix_spawnattr_getflags
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getflags (const posix_spawnattr_t *_Restrict_ __attr,
                                     short int *_Restrict_ __flags)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getflags
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETFLAGS
_GL_WARN_ON_USE (posix_spawnattr_getflags, "posix_spawnattr_getflags is unportable - "
                 "use gnulib module posix_spawnattr_getflags for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETFLAGS@
/* Store flags in the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setflags rpl_posix_spawnattr_setflags
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setflags (posix_spawnattr_t *__attr,
                                     short int __flags)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setflags
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETFLAGS
_GL_WARN_ON_USE (posix_spawnattr_setflags, "posix_spawnattr_setflags is unportable - "
                 "use gnulib module posix_spawnattr_setflags for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETPGROUP@
/* Get process group ID from the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getpgroup rpl_posix_spawnattr_getpgroup
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getpgroup (const posix_spawnattr_t *_Restrict_ __attr,
                                      pid_t *_Restrict_ __pgroup)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getpgroup
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETPGROUP
_GL_WARN_ON_USE (posix_spawnattr_getpgroup, "posix_spawnattr_getpgroup is unportable - "
                 "use gnulib module posix_spawnattr_getpgroup for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETPGROUP@
/* Store process group ID in the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setpgroup rpl_posix_spawnattr_setpgroup
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setpgroup (posix_spawnattr_t *__attr,
                                      pid_t __pgroup)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setpgroup
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETPGROUP
_GL_WARN_ON_USE (posix_spawnattr_setpgroup, "posix_spawnattr_setpgroup is unportable - "
                 "use gnulib module posix_spawnattr_setpgroup for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETSCHEDPOLICY@
/* Get scheduling policy from the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getschedpolicy rpl_posix_spawnattr_getschedpolicy
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getschedpolicy (const posix_spawnattr_t *_Restrict_ __attr,
                                           int *_Restrict_ __schedpolicy)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getschedpolicy
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETSCHEDPOLICY
_GL_WARN_ON_USE (posix_spawnattr_getschedpolicy, "posix_spawnattr_getschedpolicy is unportable - "
                 "use gnulib module posix_spawnattr_getschedpolicy for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETSCHEDPOLICY@
/* Store scheduling policy in the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setschedpolicy rpl_posix_spawnattr_setschedpolicy
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setschedpolicy (posix_spawnattr_t *__attr,
                                           int __schedpolicy)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setschedpolicy
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETSCHEDPOLICY
_GL_WARN_ON_USE (posix_spawnattr_setschedpolicy, "posix_spawnattr_setschedpolicy is unportable - "
                 "use gnulib module posix_spawnattr_setschedpolicy for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_GETSCHEDPARAM@
/* Get scheduling parameters from the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_getschedparam rpl_posix_spawnattr_getschedparam
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_getschedparam (const posix_spawnattr_t *_Restrict_ __attr,
                                          struct sched_param *_Restrict_ __schedparam)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_getschedparam
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_GETSCHEDPARAM
_GL_WARN_ON_USE (posix_spawnattr_getschedparam, "posix_spawnattr_getschedparam is unportable - "
                 "use gnulib module posix_spawnattr_getschedparam for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWNATTR_SETSCHEDPARAM@
/* Store scheduling parameters in the attribute structure.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawnattr_setschedparam rpl_posix_spawnattr_setschedparam
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawnattr_setschedparam (posix_spawnattr_t *_Restrict_ __attr,
                                          const struct sched_param *_Restrict_ __schedparam)
     __THROW _GL_ARG_NONNULL ((1, 2));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawnattr_setschedparam
# if HAVE_RAW_DECL_POSIX_SPAWNATTR_SETSCHEDPARAM
_GL_WARN_ON_USE (posix_spawnattr_setschedparam, "posix_spawnattr_setschedparam is unportable - "
                 "use gnulib module posix_spawnattr_setschedparam for portability");
# endif
#endif


#if @GNULIB_POSIX_SPAWN_FILE_ACTIONS_INIT@
/* Initialize data structure for file attribute for `spawn' call.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn_file_actions_init rpl_posix_spawn_file_actions_init
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn_file_actions_init (posix_spawn_file_actions_t *__file_actions)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn_file_actions_init
# if HAVE_RAW_DECL_POSIX_SPAWN_FILE_ACTIONS_INIT
_GL_WARN_ON_USE (posix_spawn_file_actions_init, "posix_spawn_file_actions_init is unportable - "
                 "use gnulib module posix_spawn_file_actions_init for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWN_FILE_ACTIONS_DESTROY@
/* Free resources associated with FILE-ACTIONS.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn_file_actions_destroy rpl_posix_spawn_file_actions_destroy
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn_file_actions_destroy (posix_spawn_file_actions_t *__file_actions)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn_file_actions_destroy
# if HAVE_RAW_DECL_POSIX_SPAWN_FILE_ACTIONS_DESTROY
_GL_WARN_ON_USE (posix_spawn_file_actions_destroy, "posix_spawn_file_actions_destroy is unportable - "
                 "use gnulib module posix_spawn_file_actions_destroy for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWN_FILE_ACTIONS_ADDOPEN@
/* Add an action to FILE-ACTIONS which tells the implementation to call
   `open' for the given file during the `spawn' call.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn_file_actions_addopen rpl_posix_spawn_file_actions_addopen
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn_file_actions_addopen (posix_spawn_file_actions_t *_Restrict_ __file_actions,
                                             int __fd,
                                             const char *_Restrict_ __path,
                                             int __oflag, mode_t __mode)
     __THROW _GL_ARG_NONNULL ((1, 3));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn_file_actions_addopen
# if HAVE_RAW_DECL_POSIX_SPAWN_FILE_ACTIONS_ADDOPEN
_GL_WARN_ON_USE (posix_spawn_file_actions_addopen, "posix_spawn_file_actions_addopen is unportable - "
                 "use gnulib module posix_spawn_file_actions_addopen for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWN_FILE_ACTIONS_ADDCLOSE@
/* Add an action to FILE-ACTIONS which tells the implementation to call
   `close' for the given file descriptor during the `spawn' call.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn_file_actions_addclose rpl_posix_spawn_file_actions_addclose
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn_file_actions_addclose (posix_spawn_file_actions_t *__file_actions,
                                              int __fd)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn_file_actions_addclose
# if HAVE_RAW_DECL_POSIX_SPAWN_FILE_ACTIONS_ADDCLOSE
_GL_WARN_ON_USE (posix_spawn_file_actions_addclose, "posix_spawn_file_actions_addclose is unportable - "
                 "use gnulib module posix_spawn_file_actions_addclose for portability");
# endif
#endif

#if @GNULIB_POSIX_SPAWN_FILE_ACTIONS_ADDDUP2@
/* Add an action to FILE-ACTIONS which tells the implementation to call
   `dup2' for the given file descriptors during the `spawn' call.  */
# if @REPLACE_POSIX_SPAWN@
#  define posix_spawn_file_actions_adddup2 rpl_posix_spawn_file_actions_adddup2
# endif
# if !@HAVE_POSIX_SPAWN@ || @REPLACE_POSIX_SPAWN@
extern int posix_spawn_file_actions_adddup2 (posix_spawn_file_actions_t *__file_actions,
                                             int __fd, int __newfd)
     __THROW _GL_ARG_NONNULL ((1));
# endif
#elif defined GNULIB_POSIXCHECK
# undef posix_spawn_file_actions_adddup2
# if HAVE_RAW_DECL_POSIX_SPAWN_FILE_ACTIONS_ADDDUP2
_GL_WARN_ON_USE (posix_spawn_file_actions_adddup2, "posix_spawn_file_actions_adddup2 is unportable - "
                 "use gnulib module posix_spawn_file_actions_adddup2 for portability");
# endif
#endif


#ifdef __cplusplus
}
#endif


#endif /* _GL_SPAWN_H */
#endif /* _GL_SPAWN_H */
