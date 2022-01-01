/* Condition variables for multithreading.
   Copyright (C) 2005-2022 Free Software Foundation, Inc.

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

/* Written by Yoann Vandoorselaere <yoann@prelude-ids.org>, 2008.
   Based on Bruno Haible <bruno@clisp.org> lock.h */

/*
   Condition variables can be used for waiting until a condition
   becomes true. In this respect, they are similar to wait queues. But
   contrary to wait queues, condition variables have different
   semantics that allows events to be lost when there is no thread
   waiting for them.

   Condition variable:
     Type:                gl_cond_t
     Declaration:         gl_cond_define(extern, name)
     Initializer:         gl_cond_define_initialized(, name)
     Initialization:      gl_cond_init (name);
     Waiting:             gl_cond_wait (name, lock);
     Timed wait:          bool timedout = gl_cond_timedwait (name, lock, abstime);
                          where lock is a gl_lock_t variable (cf. <glthread/lock.h>)
     Signaling:           gl_cond_signal (name);
     Broadcasting:        gl_cond_broadcast (name);
     De-initialization:   gl_cond_destroy (name);
   Equivalent functions with control of error handling:
     Initialization:      err = glthread_cond_init (&name);
     Waiting:             err = glthread_cond_wait (&name);
     Timed wait:          err = glthread_cond_timedwait (&name, &lock, abstime);
     Signaling:           err = glthread_cond_signal (&name);
     Broadcasting:        err = glthread_cond_broadcast (&name);
     De-initialization:   err = glthread_cond_destroy (&name);
*/


#ifndef _GLTHREAD_COND_H
#define _GLTHREAD_COND_H

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "glthread/lock.h"

#if !defined c11_threads_in_use
# if HAVE_THREADS_H && USE_POSIX_THREADS_FROM_LIBC
#  define c11_threads_in_use() 1
# elif HAVE_THREADS_H && USE_POSIX_THREADS_WEAK
#  include <threads.h>
#  pragma weak thrd_exit
#  define c11_threads_in_use() (thrd_exit != NULL)
# else
#  define c11_threads_in_use() 0
# endif
#endif

#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
#endif

_GL_INLINE_HEADER_BEGIN
#ifndef _GLTHREAD_COND_INLINE
# define _GLTHREAD_COND_INLINE _GL_INLINE
#endif

/* ========================================================================= */

#if USE_ISOC_THREADS || USE_ISOC_AND_POSIX_THREADS

/* Use the ISO C threads library.  */

# include <threads.h>

# ifdef __cplusplus
extern "C" {
# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

typedef struct
        {
          int volatile init_needed;
          once_flag init_once;
          void (*init_func) (void);
          cnd_t condition;
        }
        gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    static void _atomic_init_##NAME (void);       \
    STORAGECLASS gl_cond_t NAME =                 \
      { 1, ONCE_FLAG_INIT, _atomic_init_##NAME }; \
    static void _atomic_init_##NAME (void)        \
    {                                             \
      if (glthread_cond_init (&(NAME)))           \
        abort ();                                 \
    }
extern int glthread_cond_init (gl_cond_t *condition);
extern int glthread_cond_wait (gl_cond_t *condition, gl_lock_t *lock);
extern int glthread_cond_timedwait (gl_cond_t *condition, gl_lock_t *lock,
                                    const struct timespec *abstime);
extern int glthread_cond_signal (gl_cond_t *condition);
extern int glthread_cond_broadcast (gl_cond_t *condition);
extern int glthread_cond_destroy (gl_cond_t *condition);

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if USE_POSIX_THREADS

/* Use the POSIX threads library.  */

# include <pthread.h>

# ifdef __cplusplus
extern "C" {
# endif

# if PTHREAD_IN_USE_DETECTION_HARD

/* The pthread_in_use() detection needs to be done at runtime.  */
#  define pthread_in_use() \
     glthread_in_use ()
extern int glthread_in_use (void);

# endif

# if USE_POSIX_THREADS_WEAK

/* Use weak references to the POSIX threads library.  */

/* Weak references avoid dragging in external libraries if the other parts
   of the program don't use them.  Here we use them, because we don't want
   every program that uses libintl to depend on libpthread.  This assumes
   that libpthread would not be loaded after libintl; i.e. if libintl is
   loaded first, by an executable that does not depend on libpthread, and
   then a module is dynamically loaded that depends on libpthread, libintl
   will not be multithread-safe.  */

/* The way to test at runtime whether libpthread is present is to test
   whether a function pointer's value, such as &pthread_mutex_init, is
   non-NULL.  However, some versions of GCC have a bug through which, in
   PIC mode, &foo != NULL always evaluates to true if there is a direct
   call to foo(...) in the same function.  To avoid this, we test the
   address of a function in libpthread that we don't use.  */

#  pragma weak pthread_cond_init
#  pragma weak pthread_cond_wait
#  pragma weak pthread_cond_timedwait
#  pragma weak pthread_cond_signal
#  pragma weak pthread_cond_broadcast
#  pragma weak pthread_cond_destroy
#  ifndef pthread_self
#   pragma weak pthread_self
#  endif

#  if !PTHREAD_IN_USE_DETECTION_HARD
#   pragma weak pthread_mutexattr_gettype
#   define pthread_in_use() \
      (pthread_mutexattr_gettype != NULL || c11_threads_in_use ())
#  endif

# else

#  if !PTHREAD_IN_USE_DETECTION_HARD
#   define pthread_in_use() 1
#  endif

# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

typedef pthread_cond_t gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME = gl_cond_initializer;
# define gl_cond_initializer \
    PTHREAD_COND_INITIALIZER
# define glthread_cond_init(COND) \
    (pthread_in_use () ? pthread_cond_init (COND, NULL) : 0)
# define glthread_cond_wait(COND, LOCK) \
    (pthread_in_use () ? pthread_cond_wait (COND, LOCK) : 0)
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) \
    (pthread_in_use () ? pthread_cond_timedwait (COND, LOCK, ABSTIME) : 0)
# define glthread_cond_signal(COND) \
    (pthread_in_use () ? pthread_cond_signal (COND) : 0)
# define glthread_cond_broadcast(COND) \
    (pthread_in_use () ? pthread_cond_broadcast (COND) : 0)
# define glthread_cond_destroy(COND) \
    (pthread_in_use () ? pthread_cond_destroy (COND) : 0)

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if USE_WINDOWS_THREADS

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# include "windows-cond.h"

# ifdef __cplusplus
extern "C" {
# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

typedef glwthread_cond_t gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME = gl_cond_initializer;
# define gl_cond_initializer \
    GLWTHREAD_COND_INIT
# define glthread_cond_init(COND) \
    glwthread_cond_init (COND)
# define glthread_cond_wait(COND, LOCK) \
    glwthread_cond_wait (COND, LOCK, \
                         (int (*) (void *)) glwthread_mutex_lock, \
                         (int (*) (void *)) glwthread_mutex_unlock)
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) \
    glwthread_cond_timedwait (COND, LOCK, \
                              (int (*) (void *)) glwthread_mutex_lock, \
                              (int (*) (void *)) glwthread_mutex_unlock, \
                              ABSTIME)
# define glthread_cond_signal(COND) \
    glwthread_cond_signal (COND)
# define glthread_cond_broadcast(COND) \
    glwthread_cond_broadcast (COND)
# define glthread_cond_destroy(COND) \
    glwthread_cond_destroy (COND)

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if !(USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS || USE_WINDOWS_THREADS)

/* Provide dummy implementation if threads are not supported.  */

typedef int gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME)
# define gl_cond_define_initialized(STORAGECLASS, NAME)
# define glthread_cond_init(COND) 0
# define glthread_cond_wait(COND, LOCK) 0
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) 0
# define glthread_cond_signal(COND) 0
# define glthread_cond_broadcast(COND) 0
# define glthread_cond_destroy(COND) 0

#endif

/* ========================================================================= */

/* Macros with built-in error handling.  */

#ifdef __cplusplus
extern "C" {
#endif

#define gl_cond_init(COND)             \
   do                                  \
     {                                 \
       if (glthread_cond_init (&COND)) \
         abort ();                     \
     }                                 \
   while (0)
#define gl_cond_wait(COND, LOCK)              \
   do                                         \
     {                                        \
       if (glthread_cond_wait (&COND, &LOCK)) \
         abort ();                            \
     }                                        \
   while (0)
#define gl_cond_timedwait(COND, LOCK, ABSTIME) \
  gl_cond_timedwait_func (&COND, &LOCK, ABSTIME)
_GLTHREAD_COND_INLINE bool
gl_cond_timedwait_func (gl_cond_t *cond, gl_lock_t *lock, struct timespec *abstime)
{
  int err = glthread_cond_timedwait (cond, lock, abstime);
  if (err == ETIMEDOUT)
    return true;
  if (err != 0)
    abort ();
  return false;
}
#define gl_cond_signal(COND)             \
   do                                    \
     {                                   \
       if (glthread_cond_signal (&COND)) \
         abort ();                       \
     }                                   \
   while (0)
#define gl_cond_broadcast(COND)             \
   do                                       \
     {                                      \
       if (glthread_cond_broadcast (&COND)) \
         abort ();                          \
     }                                      \
   while (0)
#define gl_cond_destroy(COND)             \
   do                                     \
     {                                    \
       if (glthread_cond_destroy (&COND)) \
         abort ();                        \
     }                                    \
   while (0)

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* _GLTHREAD_COND_H */
