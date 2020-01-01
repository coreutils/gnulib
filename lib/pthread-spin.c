/* POSIX spin locks.
   Copyright (C) 2010-2020 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert, 2010, and Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* Specification.  */
#include <pthread.h>

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
# include "windows-spin.h"
#endif

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
/* Use Windows threads.  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   int shared_across_processes _GL_UNUSED)
{
  glwthread_spin_init (lock);
  return 0;
}

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  return glwthread_spin_lock (lock);
}

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  return glwthread_spin_trylock (lock);
}

int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  return glwthread_spin_unlock (lock);
}

int
pthread_spin_destroy (pthread_spinlock_t *lock)
{
  return glwthread_spin_destroy (lock);
}

#elif HAVE_PTHREAD_H
/* Provide workarounds for POSIX threads.  */

/* We don't use the C11 <stdatomic.h> (available in GCC >= 4.9) because it would
   require to link with -latomic.  */

# if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
/* Use GCC built-ins (available in GCC >= 4.7) that operate on the first byte
   of the lock.
   Documentation:
   <https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/_005f_005fatomic-Builtins.html>  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   int shared_across_processes _GL_UNUSED)
{
  __atomic_clear (lock, __ATOMIC_SEQ_CST);
  return 0;
}

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  while (__atomic_test_and_set (lock, __ATOMIC_SEQ_CST))
    ;
  return 0;
}

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  if (__atomic_test_and_set (lock, __ATOMIC_SEQ_CST))
    return EBUSY;
  return 0;
}

int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  __atomic_clear (lock, __ATOMIC_SEQ_CST);
  return 0;
}

int
pthread_spin_destroy (pthread_spinlock_t *lock)
{
  return 0;
}

# else
/* Emulate a spin lock through a mutex.  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   int shared_across_processes _GL_UNUSED)
{
  return pthread_mutex_init (lock, NULL);
}

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  return pthread_mutex_lock (lock);
}

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  return pthread_mutex_trylock (lock);
}

int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  return pthread_mutex_unlock (lock);
}

int
pthread_spin_destroy (pthread_spinlock_t *lock)
{
  return pthread_mutex_destroy (lock);
}

# endif

#else
/* Provide a dummy implementation for single-threaded applications.  */

int
pthread_spin_init (pthread_spinlock_t *lock _GL_UNUSED,
                   int shared_across_processes _GL_UNUSED)
{
  return 0;
}

int
pthread_spin_lock (pthread_spinlock_t *lock _GL_UNUSED)
{
  return 0;
}

int
pthread_spin_trylock (pthread_spinlock_t *lock _GL_UNUSED)
{
  return 0;
}

int
pthread_spin_unlock (pthread_spinlock_t *lock _GL_UNUSED)
{
  return 0;
}

int
pthread_spin_destroy (pthread_spinlock_t *lock _GL_UNUSED)
{
  return 0;
}

#endif
