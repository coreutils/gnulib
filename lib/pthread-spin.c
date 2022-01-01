/* POSIX spin locks.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

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

/* Written by Paul Eggert, 2010, and Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* Specification.  */
#include <pthread.h>

#include <stdbool.h>

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
# include "windows-spin.h"
#endif

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
/* Use Windows threads.  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   _GL_UNUSED int shared_across_processes)
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

# if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) \
      || __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1)) \
     && !defined __ibmxl__
/* Use GCC built-ins (available in GCC >= 4.7 and clang >= 3.1) that operate on
   the first byte of the lock.
   Documentation:
   <https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/_005f_005fatomic-Builtins.html>  */

#  if 1
/* An implementation that verifies the unlocks.  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   _GL_UNUSED int shared_across_processes)
{
  __atomic_store_n ((unsigned int *) lock, 0, __ATOMIC_SEQ_CST);
  return 0;
}

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  /* Wait until *lock becomes 0, then replace it with 1.  */
  unsigned int zero;
  while (!(zero = 0,
           __atomic_compare_exchange_n ((unsigned int *) lock, &zero, 1, false,
                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)))
    ;
  return 0;
}

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  unsigned int zero;
  if (!(zero = 0,
        __atomic_compare_exchange_n ((unsigned int *) lock, &zero, 1, false,
                                     __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)))
    return EBUSY;
  return 0;
}

int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  /* If *lock is 1, then replace it with 0.  */
  unsigned int one = 1;
  if (!__atomic_compare_exchange_n ((unsigned int *) lock, &one, 0, false,
                                    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
    abort ();
  return 0;
}

#  else
/* An implementation that is a little bit more optimized, but does not verify
   the unlocks.  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   _GL_UNUSED int shared_across_processes)
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

#  endif

int
pthread_spin_destroy (pthread_spinlock_t *lock)
{
  return 0;
}

# elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) \
        || __clang_major__ >= 3) \
       && !defined __ibmxl__
/* Use GCC built-ins (available in GCC >= 4.1 and clang >= 3.0).
   Documentation:
   <https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html>  */

int
pthread_spin_init (pthread_spinlock_t *lock,
                   _GL_UNUSED int shared_across_processes)
{
  * (volatile unsigned int *) lock = 0;
  __sync_synchronize ();
  return 0;
}

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  /* Wait until *lock becomes 0, then replace it with 1.  */
  while (__sync_val_compare_and_swap ((unsigned int *) lock, 0, 1) != 0)
    ;
  return 0;
}

int
pthread_spin_trylock (pthread_spinlock_t *lock)
{
  if (__sync_val_compare_and_swap ((unsigned int *) lock, 0, 1) != 0)
    return EBUSY;
  return 0;
}

int
pthread_spin_unlock (pthread_spinlock_t *lock)
{
  /* If *lock is 1, then replace it with 0.  */
  if (__sync_val_compare_and_swap ((unsigned int *) lock, 1, 0) != 1)
    abort ();
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
                   _GL_UNUSED int shared_across_processes)
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
pthread_spin_init (_GL_UNUSED pthread_spinlock_t *lock,
                   _GL_UNUSED int shared_across_processes)
{
  return 0;
}

int
pthread_spin_lock (_GL_UNUSED pthread_spinlock_t *lock)
{
  return 0;
}

int
pthread_spin_trylock (_GL_UNUSED pthread_spinlock_t *lock)
{
  return 0;
}

int
pthread_spin_unlock (_GL_UNUSED pthread_spinlock_t *lock)
{
  return 0;
}

int
pthread_spin_destroy (_GL_UNUSED pthread_spinlock_t *lock)
{
  return 0;
}

#endif
