/* An ISO C 11 compatible <threads.h>.

   Copyright (C) 2019-2022 Free Software Foundation, Inc.

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

#if __GNUC__ >= 3
@PRAGMA_SYSTEM_HEADER@
#endif
@PRAGMA_COLUMNS@

#ifndef _@GUARD_PREFIX@_THREADS_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_THREADS_H@
# @INCLUDE_NEXT@ @NEXT_THREADS_H@
#endif

#ifndef _@GUARD_PREFIX@_THREADS_H
#define _@GUARD_PREFIX@_THREADS_H

#if !@HAVE_THREADS_H@

# include <time.h>

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#  include <windows.h>

# else
/* Use POSIX threads.  */

#  include <pthread.h>

# endif

#endif

/* The __attribute__ feature is available in gcc versions 2.5 and later.
   The attribute __pure__ was added in gcc 2.96.  */
#ifndef _GL_ATTRIBUTE_PURE
# if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96) || defined __clang__
#  define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define _GL_ATTRIBUTE_PURE /* empty */
# endif
#endif

/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */

/* The definition of _Noreturn is copied here.  */

/* The definition of _GL_ARG_NONNULL is copied here.  */

/* The definition of _GL_WARN_ON_USE is copied here.  */


/* Storage class specifier for thread-local storage.  */
#ifdef _AIX
/* The macro definition from AIX 7.1..7.2 <threads.h> is unusable, because
   its expansion ends in a semicolon.  */
# undef thread_local
#endif
#if !@HAVE_THREADS_H@ || !defined thread_local
# define thread_local _Thread_local
#endif
/* Define the macro thread_local if and only if it actually works.  */
#if !@HAVE_THREAD_LOCAL@
# undef thread_local
#endif


/* =========== ISO C 11 7.26.5 Thread functions =========== */

#if !@HAVE_THREADS_H@

/* Return codes.  */
enum
{
  thrd_success  = 0,
  thrd_timedout = 1,
  thrd_busy     = 2,
  thrd_nomem    = 3,
  thrd_error    = 4
};

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  include "windows-thread.h"

typedef glwthread_thread_t thrd_t;

# else
/* Use POSIX threads.  */

typedef pthread_t thrd_t;

# endif

#endif

#if @BROKEN_THRD_START_T@
/* Need to override thrd_start_t, to make thrd_create work.  */
# define thrd_start_t rpl_thrd_start_t
/* Need to override thrd_t, to make thrd_join work.  */
struct thrd_with_exitcode
{
  thrd_t volatile tid;
  int volatile detached;
  int volatile exitcode;
};
typedef struct thrd_with_exitcode *rpl_thrd_t;
# define thrd_t rpl_thrd_t
#endif
/* Type of the main function of a thread.  */
#if !@HAVE_THREADS_H@ || @BROKEN_THRD_START_T@
typedef int (* thrd_start_t) (void *);
#endif

#if @GNULIB_THRD@
# if @REPLACE_THRD_CREATE@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define thrd_create rpl_thrd_create
#  endif
_GL_FUNCDECL_RPL (thrd_create, int, (thrd_t *, thrd_start_t, void *)
                                    _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (thrd_create, int, (thrd_t *, thrd_start_t, void *));
# else
#  if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_create, int, (thrd_t *, thrd_start_t, void *)
                                    _GL_ARG_NONNULL ((1, 2)));
#  endif
_GL_CXXALIAS_SYS (thrd_create, int, (thrd_t *, thrd_start_t, void *));
# endif
_GL_CXXALIASWARN (thrd_create);
#elif defined GNULIB_POSIXCHECK
# undef thrd_create
# if HAVE_RAW_DECL_THRD_CREATE
_GL_WARN_ON_USE (thrd_create, "thrd_create is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if @REPLACE_THRD_CURRENT@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define thrd_current rpl_thrd_current
#  endif
_GL_FUNCDECL_RPL (thrd_current, thrd_t, (void) _GL_ATTRIBUTE_PURE);
_GL_CXXALIAS_RPL (thrd_current, thrd_t, (void));
# else
#  if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_current, thrd_t, (void) _GL_ATTRIBUTE_PURE);
#  endif
_GL_CXXALIAS_SYS (thrd_current, thrd_t, (void));
# endif
_GL_CXXALIASWARN (thrd_current);
#elif defined GNULIB_POSIXCHECK
# undef thrd_current
# if HAVE_RAW_DECL_THRD_CURRENT
_GL_WARN_ON_USE (thrd_current, "thrd_current is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if @REPLACE_THRD_EQUAL@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define thrd_equal rpl_thrd_equal
#  endif
_GL_FUNCDECL_RPL (thrd_equal, int, (thrd_t, thrd_t) _GL_ATTRIBUTE_PURE);
_GL_CXXALIAS_RPL (thrd_equal, int, (thrd_t, thrd_t));
# else
#  if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_equal, int, (thrd_t, thrd_t) _GL_ATTRIBUTE_PURE);
#  endif
_GL_CXXALIAS_SYS (thrd_equal, int, (thrd_t, thrd_t));
# endif
_GL_CXXALIASWARN (thrd_equal);
#elif defined GNULIB_POSIXCHECK
# undef thrd_equal
# if HAVE_RAW_DECL_THRD_EQUAL
_GL_WARN_ON_USE (thrd_equal, "thrd_equal is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_sleep, int,
                              (const struct timespec *, struct timespec *)
                              _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (thrd_sleep, int,
                              (const struct timespec *, struct timespec *));
_GL_CXXALIASWARN (thrd_sleep);
#elif defined GNULIB_POSIXCHECK
# undef thrd_sleep
# if HAVE_RAW_DECL_THRD_SLEEP
_GL_WARN_ON_USE (thrd_sleep, "thrd_sleep is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_yield, void, (void));
# endif
_GL_CXXALIAS_SYS (thrd_yield, void, (void));
_GL_CXXALIASWARN (thrd_yield);
#elif defined GNULIB_POSIXCHECK
# undef thrd_yield
# if HAVE_RAW_DECL_THRD_YIELD
_GL_WARN_ON_USE (thrd_yield, "thrd_yield is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if @REPLACE_THRD_DETACH@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define thrd_detach rpl_thrd_detach
#  endif
_GL_FUNCDECL_RPL (thrd_detach, int, (thrd_t));
_GL_CXXALIAS_RPL (thrd_detach, int, (thrd_t));
# else
#  if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_detach, int, (thrd_t));
#  endif
_GL_CXXALIAS_SYS (thrd_detach, int, (thrd_t));
# endif
_GL_CXXALIASWARN (thrd_detach);
#elif defined GNULIB_POSIXCHECK
# undef thrd_detach
# if HAVE_RAW_DECL_THRD_DETACH
_GL_WARN_ON_USE (thrd_detach, "thrd_detach is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if @REPLACE_THRD_JOIN@
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define thrd_join rpl_thrd_join
#  endif
_GL_FUNCDECL_RPL (thrd_join, int, (thrd_t, int *));
_GL_CXXALIAS_RPL (thrd_join, int, (thrd_t, int *));
# else
#  if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_join, int, (thrd_t, int *));
#  endif
_GL_CXXALIAS_SYS (thrd_join, int, (thrd_t, int *));
# endif
_GL_CXXALIASWARN (thrd_join);
#elif defined GNULIB_POSIXCHECK
# undef thrd_join
# if HAVE_RAW_DECL_THRD_JOIN
_GL_WARN_ON_USE (thrd_join, "thrd_join is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif

#if @GNULIB_THRD@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (thrd_exit, _Noreturn void, (int));
# endif
/* Need to cast because of AIX with xlclang++.  */
_GL_CXXALIAS_SYS_CAST (thrd_exit, void, (int));
_GL_CXXALIASWARN (thrd_exit);
#elif defined GNULIB_POSIXCHECK
# undef thrd_exit
# if HAVE_RAW_DECL_THRD_EXIT
_GL_WARN_ON_USE (thrd_exit, "thrd_exit is unportable - "
                 "use gnulib module thrd for portability");
# endif
#endif


/* =========== ISO C 11 7.26.4 Mutex functions =========== */

#if !@HAVE_THREADS_H@

/* Types of mutexes.  */
enum
{
  mtx_plain     = 0,
  mtx_timed     = 1,
  mtx_recursive = 2
};

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  include "windows-mutex.h"
#  include "windows-recmutex.h"
#  include "windows-timedmutex.h"
#  include "windows-timedrecmutex.h"

typedef struct
        {
          int type;
          union
            {
              glwthread_mutex_t         u_mutex;
              glwthread_recmutex_t      u_recmutex;
              glwthread_timedmutex_t    u_timedmutex;
              glwthread_timedrecmutex_t u_timedrecmutex;
            }
          u;
        }
        mtx_t;

# else
/* Use POSIX threads.  */

typedef pthread_mutex_t mtx_t;

# endif

#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_init, int, (mtx_t *, int) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (mtx_init, int, (mtx_t *, int));
_GL_CXXALIASWARN (mtx_init);
#elif defined GNULIB_POSIXCHECK
# undef mtx_init
# if HAVE_RAW_DECL_MTX_INIT
_GL_WARN_ON_USE (mtx_init, "mtx_init is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_lock, int, (mtx_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (mtx_lock, int, (mtx_t *));
_GL_CXXALIASWARN (mtx_lock);
#elif defined GNULIB_POSIXCHECK
# undef mtx_lock
# if HAVE_RAW_DECL_MTX_LOCK
_GL_WARN_ON_USE (mtx_lock, "mtx_lock is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_trylock, int, (mtx_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (mtx_trylock, int, (mtx_t *));
_GL_CXXALIASWARN (mtx_trylock);
#elif defined GNULIB_POSIXCHECK
# undef mtx_trylock
# if HAVE_RAW_DECL_MTX_TRYLOCK
_GL_WARN_ON_USE (mtx_trylock, "mtx_trylock is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_timedlock, int, (mtx_t *, const struct timespec *)
                                      _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (mtx_timedlock, int, (mtx_t *, const struct timespec *));
_GL_CXXALIASWARN (mtx_timedlock);
#elif defined GNULIB_POSIXCHECK
# undef mtx_timedlock
# if HAVE_RAW_DECL_MTX_TIMEDLOCK
_GL_WARN_ON_USE (mtx_timedlock, "mtx_timedlock is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_unlock, int, (mtx_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (mtx_unlock, int, (mtx_t *));
_GL_CXXALIASWARN (mtx_unlock);
#elif defined GNULIB_POSIXCHECK
# undef mtx_unlock
# if HAVE_RAW_DECL_MTX_UNLOCK
_GL_WARN_ON_USE (mtx_unlock, "mtx_unlock is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (mtx_destroy, void, (mtx_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (mtx_destroy, void, (mtx_t *));
_GL_CXXALIASWARN (mtx_destroy);
#elif defined GNULIB_POSIXCHECK
# undef mtx_destroy
# if HAVE_RAW_DECL_MTX_DESTROY
_GL_WARN_ON_USE (mtx_destroy, "mtx_destroy is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif


/* =========== ISO C 11 7.26.2 Initialization functions =========== */

#if !@HAVE_THREADS_H@

/* Type that contains a flag for use by call_once.  */
# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  include "windows-once.h"

typedef glwthread_once_t once_flag;
#  define ONCE_FLAG_INIT GLWTHREAD_ONCE_INIT

# else
/* Use POSIX threads.  */

typedef pthread_once_t once_flag;
#  define ONCE_FLAG_INIT PTHREAD_ONCE_INIT

# endif

#endif

#if @GNULIB_MTX@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (call_once, void, (once_flag *, void (*) (void))
                                   _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS_CAST (call_once, void, (once_flag *, void (*) (void)));
_GL_CXXALIASWARN (call_once);
#elif defined GNULIB_POSIXCHECK
# undef call_once
# if HAVE_RAW_DECL_CALL_ONCE
_GL_WARN_ON_USE (call_once, "call_once is unportable - "
                 "use gnulib module mtx for portability");
# endif
#endif


/* =========== ISO C 11 7.26.3 Condition variable functions =========== */

#if !@HAVE_THREADS_H@

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  include "windows-cond.h"

typedef glwthread_cond_t cnd_t;

# else
/* Use POSIX threads.  */

typedef pthread_cond_t cnd_t;

# endif

#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_init, int, (cnd_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (cnd_init, int, (cnd_t *));
_GL_CXXALIASWARN (cnd_init);
#elif defined GNULIB_POSIXCHECK
# undef cnd_init
# if HAVE_RAW_DECL_CND_INIT
_GL_WARN_ON_USE (cnd_init, "cnd_init is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_wait, int, (cnd_t *, mtx_t *) _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (cnd_wait, int, (cnd_t *, mtx_t *));
_GL_CXXALIASWARN (cnd_wait);
#elif defined GNULIB_POSIXCHECK
# undef cnd_wait
# if HAVE_RAW_DECL_CND_WAIT
_GL_WARN_ON_USE (cnd_wait, "cnd_wait is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_timedwait, int,
                                 (cnd_t *, mtx_t *, const struct timespec *)
                                 _GL_ARG_NONNULL ((1, 2, 3)));
# endif
_GL_CXXALIAS_SYS (cnd_timedwait, int,
                                 (cnd_t *, mtx_t *, const struct timespec *));
_GL_CXXALIASWARN (cnd_timedwait);
#elif defined GNULIB_POSIXCHECK
# undef cnd_timedwait
# if HAVE_RAW_DECL_CND_TIMEDWAIT
_GL_WARN_ON_USE (cnd_timedwait, "cnd_timedwait is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_signal, int, (cnd_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (cnd_signal, int, (cnd_t *));
_GL_CXXALIASWARN (cnd_signal);
#elif defined GNULIB_POSIXCHECK
# undef cnd_signal
# if HAVE_RAW_DECL_CND_SIGNAL
_GL_WARN_ON_USE (cnd_signal, "cnd_signal is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_broadcast, int, (cnd_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (cnd_broadcast, int, (cnd_t *));
_GL_CXXALIASWARN (cnd_broadcast);
#elif defined GNULIB_POSIXCHECK
# undef cnd_broadcast
# if HAVE_RAW_DECL_CND_BROADCAST
_GL_WARN_ON_USE (cnd_broadcast, "cnd_broadcast is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif

#if @GNULIB_CND@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (cnd_destroy, void, (cnd_t *) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (cnd_destroy, void, (cnd_t *));
_GL_CXXALIASWARN (cnd_destroy);
#elif defined GNULIB_POSIXCHECK
# undef cnd_destroy
# if HAVE_RAW_DECL_CND_DESTROY
_GL_WARN_ON_USE (cnd_destroy, "cnd_destroy is unportable - "
                 "use gnulib module cnd for portability");
# endif
#endif


/* =========== ISO C 11 7.26.6 Thread-specific storage functions =========== */

#if !@HAVE_THREADS_H@

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  include "windows-tls.h"

typedef glwthread_tls_key_t tss_t;
#  define TSS_DTOR_ITERATIONS GLWTHREAD_DESTRUCTOR_ITERATIONS

# else
/* Use POSIX threads.  */

#  include <limits.h>

typedef pthread_key_t tss_t;

# endif

/* Type for the destructor of a thread-specific storage pointer.  */
typedef void (*tss_dtor_t) (void *);

#endif

/* AIX 7.1 <threads.h> does not define TSS_DTOR_ITERATIONS.  */
#ifndef TSS_DTOR_ITERATIONS
# ifdef PTHREAD_DESTRUCTOR_ITERATIONS
#  define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS
# else
   /* IRIX 6.5 does not define PTHREAD_DESTRUCTOR_ITERATIONS.
      This value is a wild guess.  */
#  define TSS_DTOR_ITERATIONS 1
# endif
#endif

#if @GNULIB_TSS@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (tss_create, int, (tss_t *, tss_dtor_t) _GL_ARG_NONNULL ((1)));
# endif
_GL_CXXALIAS_SYS (tss_create, int, (tss_t *, tss_dtor_t));
_GL_CXXALIASWARN (tss_create);
#elif defined GNULIB_POSIXCHECK
# undef tss_create
# if HAVE_RAW_DECL_TSS_CREATE
_GL_WARN_ON_USE (tss_create, "tss_create is unportable - "
                 "use gnulib module tss for portability");
# endif
#endif

#if @GNULIB_TSS@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (tss_set, int, (tss_t, void *));
# endif
_GL_CXXALIAS_SYS (tss_set, int, (tss_t, void *));
_GL_CXXALIASWARN (tss_set);
#elif defined GNULIB_POSIXCHECK
# undef tss_set
# if HAVE_RAW_DECL_TSS_SET
_GL_WARN_ON_USE (tss_set, "tss_set is unportable - "
                 "use gnulib module tss for portability");
# endif
#endif

#if @GNULIB_TSS@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (tss_get, void *, (tss_t));
# endif
_GL_CXXALIAS_SYS (tss_get, void *, (tss_t));
_GL_CXXALIASWARN (tss_get);
#elif defined GNULIB_POSIXCHECK
# undef tss_get
# if HAVE_RAW_DECL_TSS_GET
_GL_WARN_ON_USE (tss_get, "tss_get is unportable - "
                 "use gnulib module tss for portability");
# endif
#endif

#if @GNULIB_TSS@
# if !@HAVE_THREADS_H@
_GL_FUNCDECL_SYS (tss_delete, void, (tss_t));
# endif
_GL_CXXALIAS_SYS (tss_delete, void, (tss_t));
_GL_CXXALIASWARN (tss_delete);
#elif defined GNULIB_POSIXCHECK
# undef tss_delete
# if HAVE_RAW_DECL_TSS_DELETE
_GL_WARN_ON_USE (tss_delete, "tss_delete is unportable - "
                 "use gnulib module tss for portability");
# endif
#endif


#endif /* _@GUARD_PREFIX@_THREADS_H */
#endif /* _@GUARD_PREFIX@_THREADS_H */
