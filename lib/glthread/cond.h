/* Condition variables for multithreading.
   Copyright (C) 2005-2016 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

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
#ifndef _GL_INLINE_HEADER_BEGIN
 #error "Please include config.h first."
#endif

_GL_INLINE_HEADER_BEGIN
#ifndef _GLTHREAD_COND_INLINE
# define _GLTHREAD_COND_INLINE _GL_INLINE
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
#   pragma weak pthread_cancel
#   define pthread_in_use() (pthread_cancel != NULL)
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

#if USE_PTH_THREADS

/* Use the GNU Pth threads library.  */

# include <pth.h>

# ifdef __cplusplus
extern "C" {
# endif

# if USE_PTH_THREADS_WEAK

/* Use weak references to the GNU Pth threads library.  */

#  pragma weak pth_cond_init
#  pragma weak pth_cond_await
#  pragma weak pth_cond_notify
#  pragma weak pth_event
#  pragma weak pth_timeout

#  pragma weak pth_cancel
#  define pth_in_use() (pth_cancel != NULL)

# else

#  define pth_in_use() 1

# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

typedef pth_cond_t gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME = gl_cond_initializer;
# define gl_cond_initializer \
    PTH_COND_INIT
# define glthread_cond_init(COND) \
    (pth_in_use () && !pth_cond_init (COND) ? errno : 0)
# define glthread_cond_wait(COND, LOCK) \
    (pth_in_use () && !pth_cond_await (COND, LOCK, NULL) ? errno : 0)
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) \
    (pth_in_use () ? glthread_cond_timedwait_multithreaded (COND, LOCK, ABSTIME) : 0)
# define glthread_cond_signal(COND) \
    (pth_in_use () && !pth_cond_notify (COND, FALSE) ? errno : 0)
# define glthread_cond_broadcast(COND) \
    (pth_in_use () && !pth_cond_notify (COND, TRUE) ? errno : 0)
# define glthread_cond_destroy(COND) 0
extern int glthread_cond_timedwait_multithreaded (gl_cond_t *cond, gl_lock_t *lock, struct timespec *abstime);

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if USE_SOLARIS_THREADS

/* Use the old Solaris threads library.  */

# include <thread.h>
# include <synch.h>

# ifdef __cplusplus
extern "C" {
# endif

# if USE_SOLARIS_THREADS_WEAK

/* Use weak references to the old Solaris threads library.  */

#  pragma weak cond_init
#  pragma weak cond_wait
#  pragma weak cond_timedwait
#  pragma weak cond_signal
#  pragma weak cond_broadcast
#  pragma weak cond_destroy
#  pragma weak thr_suspend
#  define thread_in_use() (thr_suspend != NULL)

# else

#  define thread_in_use() 1

# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

typedef cond_t gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME = gl_cond_initializer;
# define gl_cond_initializer \
    DEFAULTCV
# define glthread_cond_init(COND) \
    (pthread_in_use () ? cond_init (COND, USYNC_THREAD, NULL) : 0)
# define glthread_cond_wait(COND, LOCK) \
    (pthread_in_use () ? cond_wait (COND, LOCK) : 0)
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) \
    (pthread_in_use () ? glthread_cond_timedwait_multithreaded (COND, LOCK, ABSTIME) : 0)
# define glthread_cond_signal(COND) \
    (pthread_in_use () ? cond_signal (COND) : 0)
# define glthread_cond_broadcast(COND) \
    (pthread_in_use () ? cond_broadcast (COND) : 0)
# define glthread_cond_destroy(COND) \
    (pthread_in_use () ? cond_destroy (COND) : 0)
extern int glthread_cond_timedwait_multithreaded (gl_cond_t *cond, gl_lock_t *lock, struct timespec *abstime);

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if USE_WINDOWS_THREADS

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# ifdef __cplusplus
extern "C" {
# endif

/* -------------------------- gl_cond_t datatype -------------------------- */

struct gl_waitqueue_link
{
  struct gl_waitqueue_link *wql_next;
  struct gl_waitqueue_link *wql_prev;
};
typedef struct
        {
          struct gl_waitqueue_link wq_list; /* circular list of waiting threads */
        }
        gl_linked_waitqueue_t;
typedef struct
        {
          gl_spinlock_t guard; /* protects the initialization */
          CRITICAL_SECTION lock; /* protects the remaining fields */
          gl_linked_waitqueue_t waiters; /* waiting threads */
        }
        gl_cond_t;
# define gl_cond_define(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME;
# define gl_cond_define_initialized(STORAGECLASS, NAME) \
    STORAGECLASS gl_cond_t NAME = gl_cond_initializer;
# define gl_cond_initializer \
    { { 0, -1 } }
# define glthread_cond_init(COND) \
    glthread_cond_init_func (COND)
# define glthread_cond_wait(COND, LOCK) \
    glthread_cond_wait_func (COND, LOCK)
# define glthread_cond_timedwait(COND, LOCK, ABSTIME) \
    glthread_cond_timedwait_func (COND, LOCK, ABSTIME)
# define glthread_cond_signal(COND) \
    glthread_cond_signal_func (COND)
# define glthread_cond_broadcast(COND) \
    glthread_cond_broadcast_func (COND)
# define glthread_cond_destroy(COND) \
    glthread_cond_destroy_func (COND)
extern int glthread_cond_init_func (gl_cond_t *cond);
extern int glthread_cond_wait_func (gl_cond_t *cond, gl_lock_t *lock);
extern int glthread_cond_timedwait_func (gl_cond_t *cond, gl_lock_t *lock, struct timespec *abstime);
extern int glthread_cond_signal_func (gl_cond_t *cond);
extern int glthread_cond_broadcast_func (gl_cond_t *cond);
extern int glthread_cond_destroy_func (gl_cond_t *cond);

# ifdef __cplusplus
}
# endif

#endif

/* ========================================================================= */

#if !(USE_POSIX_THREADS || USE_PTH_THREADS || USE_SOLARIS_THREADS || USE_WINDOWS_THREADS)

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
