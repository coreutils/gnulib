/* ISO C 11 condition variables for multithreading.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

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

/* Written by Yoann Vandoorselaere <yoann@prelude-ids.org>, 2008,
   and Bruno Haible <bruno@clisp.org>, 2008, 2019.  */

#include <config.h>

#include <threads.h>

#include <errno.h>
#include <stdlib.h>

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# include "windows-mutex.h"
# include "windows-recmutex.h"
# include "windows-timedmutex.h"
# include "windows-timedrecmutex.h"

#else
/* Use POSIX threads.  */

# include <pthread.h>

#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

int
cnd_init (cnd_t *cond)
{
  int err = glwthread_cond_init (cond);
  return (err == 0 ? thrd_success :
          err == ENOMEM ? thrd_nomem :
          thrd_error);
}

static int
mutex_lock (mtx_t *mutex)
{
  switch (mutex->type)
    {
    case mtx_plain:
      return glwthread_mutex_lock (&mutex->u.u_mutex);
    case mtx_plain | mtx_recursive:
      return glwthread_recmutex_lock (&mutex->u.u_recmutex);
    case mtx_timed:
      return glwthread_timedmutex_lock (&mutex->u.u_timedmutex);
    case mtx_timed | mtx_recursive:
      return glwthread_timedrecmutex_lock (&mutex->u.u_timedrecmutex);
    default:
      abort ();
    }
}

static int
mutex_unlock (mtx_t *mutex)
{
  switch (mutex->type)
    {
    case mtx_plain:
      return glwthread_mutex_unlock (&mutex->u.u_mutex);
    case mtx_plain | mtx_recursive:
      return glwthread_recmutex_unlock (&mutex->u.u_recmutex);
    case mtx_timed:
      return glwthread_timedmutex_unlock (&mutex->u.u_timedmutex);
    case mtx_timed | mtx_recursive:
      return glwthread_timedrecmutex_unlock (&mutex->u.u_timedrecmutex);
    default:
      abort ();
    }
}

int
cnd_wait (cnd_t *cond, mtx_t *mutex)
{
  int err = glwthread_cond_wait (cond, mutex,
                                 (int (*) (void *)) mutex_lock,
                                 (int (*) (void *)) mutex_unlock);
  return (err == 0 ? thrd_success : thrd_error);
}

int
cnd_timedwait (cnd_t *cond, mtx_t *mutex, const struct timespec *abstime)
{
  int err = glwthread_cond_timedwait (cond, mutex,
                                      (int (*) (void *)) mutex_lock,
                                      (int (*) (void *)) mutex_unlock,
                                      abstime);
  return (err == 0 ? thrd_success :
          err == ETIMEDOUT ? thrd_timedout :
          thrd_error);
}

int
cnd_signal (cnd_t *cond)
{
  int err = glwthread_cond_signal (cond);
  return (err == 0 ? thrd_success :
          err == ENOMEM ? thrd_nomem :
          thrd_error);
}

int
cnd_broadcast (cnd_t *cond)
{
  int err = glwthread_cond_broadcast (cond);
  return (err == 0 ? thrd_success : thrd_error);
}

void
cnd_destroy (cnd_t *cond)
{
  glwthread_cond_destroy (cond);
}

#else
/* Use POSIX threads.  */

int
cnd_init (cnd_t *cond)
{
  int err = pthread_cond_init (cond, NULL);
  return (err == 0 ? thrd_success :
          err == ENOMEM ? thrd_nomem :
          thrd_error);
}

int
cnd_wait (cnd_t *cond, mtx_t *mutex)
{
  int err = pthread_cond_wait (cond, mutex);
  return (err == 0 ? thrd_success : thrd_error);
}

int
cnd_timedwait (cnd_t *cond, mtx_t *mutex, const struct timespec *abstime)
{
  int err = pthread_cond_timedwait (cond, mutex, abstime);
  return (err == 0 ? thrd_success :
          err == ETIMEDOUT ? thrd_timedout :
          thrd_error);
}

int
cnd_signal (cnd_t *cond)
{
  int err = pthread_cond_signal (cond);
  return (err == 0 ? thrd_success :
          err == ENOMEM ? thrd_nomem :
          thrd_error);
}

int
cnd_broadcast (cnd_t *cond)
{
  int err = pthread_cond_broadcast (cond);
  return (err == 0 ? thrd_success : thrd_error);
}

void
cnd_destroy (cnd_t *cond)
{
  pthread_cond_destroy (cond);
}

#endif
