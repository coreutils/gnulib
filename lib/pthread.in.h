/* Implement a trivial subset of POSIX 1003.1-2008 pthread.h.

   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Paul Eggert and Glen Lenker.  */

#ifndef _GL_PTHREAD_H_
#define _GL_PTHREAD_H_

#include <errno.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/types.h>
#include <time.h>

#ifndef HAVE_PTHREAD_T
 typedef int pthread_t;
 typedef int pthread_attr_t;
 typedef int pthread_barrier_t;
 typedef int pthread_barrierattr_t;
 typedef int pthread_cond_t;
 typedef int pthread_condattr_t;
 typedef int pthread_key_t;
 typedef int pthread_mutex_t;
 typedef int pthread_mutexattr_t;
 typedef int pthread_once_t;
 typedef int pthread_rwlock_t;
 typedef int pthread_rwlockattr_t;
 typedef int pthread_spinlock_t;
#endif

#define PTHREAD_COND_INITIALIZER { 0 }
#define PTHREAD_MUTEX_INITIALIZER { 0 }
#define PTHREAD_ONCE_INIT { 0 }
#define PTHREAD_RWLOCK_INITIALIZER { 0 }

#define PTHREAD_BARRIER_SERIAL_THREAD (-1)

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_CANCELED ((void *) -1)

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_MUTEX_DEFAULT PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE 2

#define PTHREAD_MUTEX_STALLED 0
#define PTHREAD_MUTEX_ROBUST 1

#define PTHREAD_PRIO_NONE 0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_SCOPE_PROCESS 1

/* Provide substitutes for the thread functions that should work
   adequately on a single-threaded implementation, where
   pthread_create always fails.  The goal is to let programs compile
   on non-pthread hosts with minimal runtime overhead.

   Omit interfaces that have not been analyzed and for which we do not
   know what to do, so that they elicit a compile-time error for
   now.  */

static inline int
pthread_cond_destroy (pthread_cond_t *cond)
{
  /* COND is never seriously used.  */
  return 0;
}

static inline int
pthread_cond_init (pthread_cond_t *restrict cond,
                   pthread_condattr_t const *restrict attr)
{
  /* COND is never seriously used.  */
  return 0;
}

static inline int
pthread_cond_signal (pthread_cond_t *cond)
{
  /* No threads can currently be blocked on COND.  */
  return 0;
}

static inline int
pthread_cond_wait (pthread_cond_t *restrict cond,
                   pthread_mutex_t *restrict mutex)
{
  /* Properly-written applications never come here.  */
  abort ();
  return 0;
}

static inline int
pthread_create (pthread_t *restrict thread,
                pthread_attr_t const *restrict attr,
                void * (*start_routine) (void*), void *restrict arg)
{
  /* Do not create a thread.  */
  return EAGAIN;
}

static inline void
pthread_exit (void *value)
{
  /* There is just one thread, so the process exits.  */
  exit (0);
}

static inline int
pthread_join (pthread_t thread, void **pvalue)
{
  /* Properly-written applications never come here.  */
  abort ();
  return 0;
}

static inline int
pthread_mutex_destroy (pthread_mutex_t *mutex)
{
  /* MUTEX is never seriously used.  */
  return 0;
}

static inline int
pthread_mutex_init (pthread_mutex_t *restrict mutex,
                    pthread_mutexattr_t const *restrict attr)
{
  /* MUTEX is never seriously used.  */
  return 0;
}

static inline int
pthread_mutex_lock (pthread_mutex_t *mutex)
{
  /* There is only one thread, so it always gets the lock.  This
     implementation does not support PTHREAD_MUTEX_ERRORCHECK.  */
  return 0;
}

static inline int
pthread_mutex_unlock (pthread_mutex_t *mutex)
{
  /* There is only one thread, so it always unlocks successfully.
     This implementation does not support robust mutexes or
     PTHREAD_MUTEX_ERRORCHECK.  */
  return 0;
}

static inline int
pthread_spin_init (pthread_spinlock_t *lock, int pshared)
{
  /* LOCK is never seriously used.  */
  return 0;
}

static inline int
pthread_spin_destroy (pthread_spinlock_t *lock)
{
  /* LOCK is never seriously used.  */
  return 0;
}

static inline int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  /* Only one thread, so it always gets the lock.  */
  return 0;
}

static inline int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  /* Only one thread, so it always gets the lock.  Assume that a
     thread never tries a lock that it already holds.  */
  return 0;
}

static inline int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  /* Only one thread, so spin locks are no-ops.  */
  return 0;
}

#endif /* _GL_PTHREAD_H_ */
