/* Spin locks for communication between threads and signal handlers.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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
#include "asyncsafe-spin.h"

#include <stdlib.h>

void
asyncsafe_spin_init (asyncsafe_spinlock_t *lock)
{
  glthread_spinlock_init (lock);
}

void
asyncsafe_spin_lock (asyncsafe_spinlock_t *lock,
                     const sigset_t *mask, sigset_t *saved_mask)
{
  sigprocmask (SIG_BLOCK, mask, saved_mask); /* equivalent to pthread_sigmask */
  glthread_spinlock_lock (lock);
}

void
asyncsafe_spin_unlock (asyncsafe_spinlock_t *lock, const sigset_t *saved_mask)
{
  if (glthread_spinlock_unlock (lock))
    abort ();
  sigprocmask (SIG_SETMASK, saved_mask, NULL); /* equivalent to pthread_sigmask */
}

void
asyncsafe_spin_destroy (asyncsafe_spinlock_t *lock)
{
  glthread_spinlock_destroy (lock);
}
