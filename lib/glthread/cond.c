/* Condition variables for multithreading.
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
   and Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#define _GLTHREAD_COND_INLINE _GL_EXTERN_INLINE
#include "glthread/cond.h"

/* ========================================================================= */

#if USE_ISOC_THREADS || USE_ISOC_AND_POSIX_THREADS

int
glthread_cond_init (gl_cond_t *condition)
{
  if (cnd_init (&condition->condition) != thrd_success)
    return ENOMEM;
  condition->init_needed = 0;
  return 0;
}

int
glthread_cond_wait (gl_cond_t *condition, gl_lock_t *lock)
{
  if (condition->init_needed)
    call_once (&condition->init_once, condition->init_func);
  if (lock->init_needed)
    call_once (&lock->init_once, lock->init_func);
  if (cnd_wait (&condition->condition, &lock->mutex) != thrd_success)
    return EINVAL;
  return 0;
}

int
glthread_cond_timedwait (gl_cond_t *condition, gl_lock_t *lock,
                         const struct timespec *abstime)
{
  if (condition->init_needed)
    call_once (&condition->init_once, condition->init_func);
  if (lock->init_needed)
    call_once (&lock->init_once, lock->init_func);
  switch (cnd_timedwait (&condition->condition, &lock->mutex, abstime))
    {
    case thrd_success:
      break;
    case thrd_timedout:
      return ETIMEDOUT;
    default:
      return EINVAL;
    }
  return 0;
}

int
glthread_cond_signal (gl_cond_t *condition)
{
  if (condition->init_needed)
    call_once (&condition->init_once, condition->init_func);
  if (cnd_signal (&condition->condition) != thrd_success)
    return EINVAL;
  return 0;
}

int
glthread_cond_broadcast (gl_cond_t *condition)
{
  if (condition->init_needed)
    call_once (&condition->init_once, condition->init_func);
  if (cnd_broadcast (&condition->condition) != thrd_success)
    return EINVAL;
  return 0;
}

int
glthread_cond_destroy (gl_cond_t *condition)
{
  if (condition->init_needed)
    call_once (&condition->init_once, condition->init_func);
  cnd_destroy (&condition->condition);
  return 0;
}

#endif

/* ========================================================================= */

#if USE_POSIX_THREADS

#endif

/* ========================================================================= */

#if USE_WINDOWS_THREADS

#endif

/* ========================================================================= */
