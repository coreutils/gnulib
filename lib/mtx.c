/* ISO C 11 locking in multithreaded situations.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2005, 2019.
   Based on GCC's gthr-posix.h, gthr-posix95.h, gthr-win32.h.  */

#include <config.h>

#include <threads.h>

#include <errno.h>

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# include <stdlib.h>

#else
/* Use POSIX threads.  */

# include <pthread.h>

#endif

#if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

int
mtx_init (mtx_t *mutex, int type)
{
  switch (type)
    {
    case mtx_plain:
      glwthread_mutex_init (&mutex->u.u_mutex);
      break;
    case mtx_plain | mtx_recursive:
      glwthread_recmutex_init (&mutex->u.u_recmutex);
      break;
    case mtx_timed:
      if (glwthread_timedmutex_init (&mutex->u.u_timedmutex) != 0)
        return thrd_error;
      break;
    case mtx_timed | mtx_recursive:
      if (glwthread_timedrecmutex_init (&mutex->u.u_timedrecmutex) != 0)
        return thrd_error;
      break;
    default:
      return thrd_error;
    }
  mutex->type = type;
  return thrd_success;
}

int
mtx_lock (mtx_t *mutex)
{
  int err;

  switch (mutex->type)
    {
    case mtx_plain:
      err = glwthread_mutex_lock (&mutex->u.u_mutex);
      break;
    case mtx_plain | mtx_recursive:
      err = glwthread_recmutex_lock (&mutex->u.u_recmutex);
      break;
    case mtx_timed:
      err = glwthread_timedmutex_lock (&mutex->u.u_timedmutex);
      break;
    case mtx_timed | mtx_recursive:
      err = glwthread_timedrecmutex_lock (&mutex->u.u_timedrecmutex);
      break;
    default:
      abort ();
    }
  return (err == 0 ? thrd_success : thrd_error);
}

int
mtx_trylock (mtx_t *mutex)
{
  int err;

  switch (mutex->type)
    {
    case mtx_plain:
      err = glwthread_mutex_trylock (&mutex->u.u_mutex);
      break;
    case mtx_plain | mtx_recursive:
      err = glwthread_recmutex_trylock (&mutex->u.u_recmutex);
      break;
    case mtx_timed:
      err = glwthread_timedmutex_trylock (&mutex->u.u_timedmutex);
      break;
    case mtx_timed | mtx_recursive:
      err = glwthread_timedrecmutex_trylock (&mutex->u.u_timedrecmutex);
      break;
    default:
      abort ();
    }
  return (err == 0 ? thrd_success : err == EBUSY ? thrd_busy : thrd_error);
}

int
mtx_timedlock (mtx_t *mutex, const struct timespec *abstime)
{
  int err;

  switch (mutex->type)
    {
    case mtx_plain:
    case mtx_plain | mtx_recursive:
      return thrd_error;
    case mtx_timed:
      err = glwthread_timedmutex_timedlock (&mutex->u.u_timedmutex, abstime);
      break;
    case mtx_timed | mtx_recursive:
      err =
        glwthread_timedrecmutex_timedlock (&mutex->u.u_timedrecmutex, abstime);
      break;
    default:
      abort ();
    }
  return (err == 0 ? thrd_success : err == EBUSY ? thrd_busy : thrd_error);
}

int
mtx_unlock (mtx_t *mutex)
{
  int err;

  switch (mutex->type)
    {
    case mtx_plain:
      err = glwthread_mutex_unlock (&mutex->u.u_mutex);
      break;
    case mtx_plain | mtx_recursive:
      err = glwthread_recmutex_unlock (&mutex->u.u_recmutex);
      break;
    case mtx_timed:
      err = glwthread_timedmutex_unlock (&mutex->u.u_timedmutex);
      break;
    case mtx_timed | mtx_recursive:
      err = glwthread_timedrecmutex_unlock (&mutex->u.u_timedrecmutex);
      break;
    default:
      abort ();
    }
  return (err == 0 ? thrd_success : thrd_error);
}

void
mtx_destroy (mtx_t *mutex)
{
  switch (mutex->type)
    {
    case mtx_plain:
      glwthread_mutex_destroy (&mutex->u.u_mutex);
      break;
    case mtx_plain | mtx_recursive:
      glwthread_recmutex_destroy (&mutex->u.u_recmutex);
      break;
    case mtx_timed:
      glwthread_timedmutex_destroy (&mutex->u.u_timedmutex);
      break;
    case mtx_timed | mtx_recursive:
      glwthread_timedrecmutex_destroy (&mutex->u.u_timedrecmutex);
      break;
    default:
      abort ();
    }
}

void
call_once (once_flag *flagp, void (*func) (void))
{
  glwthread_once (flagp, func);
}

#else
/* Use POSIX threads.  */

int
mtx_init (mtx_t *mutex, int type)
{
  switch (type)
    {
    case mtx_plain:
    case mtx_timed:
    case mtx_plain | mtx_recursive:
    case mtx_timed | mtx_recursive:
      break;
    default:
      return thrd_error;
    }

  if ((type & mtx_recursive) != 0)
    {
      pthread_mutexattr_t attributes;
      int err;

      err = pthread_mutexattr_init (&attributes);
      if (err != 0)
        return thrd_error;
      err = pthread_mutexattr_settype (&attributes, PTHREAD_MUTEX_RECURSIVE);
      if (err != 0)
        {
          pthread_mutexattr_destroy (&attributes);
          return thrd_error;
        }
      err = pthread_mutex_init (mutex, &attributes);
      if (err != 0)
        {
          pthread_mutexattr_destroy (&attributes);
          return thrd_error;
        }
      err = pthread_mutexattr_destroy (&attributes);
      if (err != 0)
        return thrd_error;
    }
  else
    {
      int err = pthread_mutex_init (mutex, NULL);
      if (err != 0)
        return thrd_error;
    }
  return thrd_success;
}

int
mtx_lock (mtx_t *mutex)
{
  int err = pthread_mutex_lock (mutex);
  return (err == 0 ? thrd_success : thrd_error);
}

int
mtx_trylock (mtx_t *mutex)
{
  int err = pthread_mutex_trylock (mutex);
  return (err == 0 ? thrd_success : err == EBUSY ? thrd_busy : thrd_error);
}

int
mtx_timedlock (mtx_t *mutex, const struct timespec *abstime)
{
  int err = pthread_mutex_timedlock (mutex, abstime);
  return (err == 0 ? thrd_success :
          err == ETIMEDOUT ? thrd_timedout :
          thrd_error);
}

int
mtx_unlock (mtx_t *mutex)
{
  int err = pthread_mutex_unlock (mutex);
  return (err == 0 ? thrd_success : thrd_error);
}

void
mtx_destroy (mtx_t *mutex)
{
  pthread_mutex_destroy (mutex);
}

void
call_once (once_flag *flagp, void (*func) (void))
{
  pthread_once (flagp, func);
}

#endif
