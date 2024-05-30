/* POSIX once-only control.
   Copyright (C) 2019-2024 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* Specification.  */
#include <pthread.h>

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
# include "windows-once.h"
#endif

#if (defined _WIN32 && ! defined __CYGWIN__) && USE_WINDOWS_THREADS
/* Use Windows threads.  */

int
pthread_once (pthread_once_t *once_control, void (*initfunction) (void))
{
  glwthread_once (once_control, initfunction);
  return 0;
}

#elif HAVE_PTHREAD_H
/* Provide workarounds for POSIX threads.  */

# if defined __CYGWIN__

#  include <stdlib.h>

int
pthread_once (pthread_once_t *once_control, void (*initfunction) (void))
{
#  if 0
  /* This would be the code, for
       typedef struct
         {
           pthread_mutex_t mutex;
           _Atomic unsigned int num_threads;
           _Atomic unsigned int done;
         }
       pthread_once_t;
   */
  if (once_control->done == 0)
    {
      once_control->num_threads += 1;
      pthread_mutex_lock (&once_control->mutex);
      if (once_control->done == 0)
        {
          (*initfunction) ();
          once_control->done = 1;
        }
      pthread_mutex_unlock (&once_control->mutex);
      if ((once_control->num_threads -= 1) == 0)
        pthread_mutex_destroy (&once_control->mutex);
    }
#  else
  /* In this implementation, we reuse the type
       typedef struct { pthread_mutex_t mutex; int state; } pthread_once_t;
       #define PTHREAD_ONCE_INIT { PTHREAD_MUTEX_INITIALIZER, 0 }
     while assigning the following meaning to the state:
       state = 2 * <number of waiting threads> + <1 if done>
     In other words:
       state = { unsigned int num_threads : 31; unsigned int done : 1; }
   */
  _Atomic unsigned int *state_p = (_Atomic unsigned int *) &once_control->state;
  /* Test the 'done' bit.  */
  if ((*state_p & 1) == 0)
    {
      /* The 'done' bit is still zero.  Increment num_threads (atomically).  */
      *state_p += 2;
      /* We have incremented num_threads.  Now take the lock.  */
      pthread_mutex_lock (&once_control->mutex);
      /* Test the 'done' bit again.  */
      if ((*state_p & 1) == 0)
        {
          /* Execute the initfunction.  */
          (*initfunction) ();
          /* Set the 'done' bit to 1 (atomically).  */
          *state_p |= 1;
        }
      /* Now the 'done' bit is 1.  Release the lock.  */
      pthread_mutex_unlock (&once_control->mutex);
      /* Decrement num_threads (atomically).  */
      unsigned int new_state = (*state_p -= 2);
      if (new_state == 1)
        /* num_threads is now zero, and done is 1.
           No other thread will need to use the lock.
           We can therefore destroy the lock, to free resources.  */
        pthread_mutex_destroy (&once_control->mutex);
    }
#  endif
  return 0;
}

# endif

#else
/* Provide a dummy implementation for single-threaded applications.  */

int
pthread_once (pthread_once_t *once_control, void (*initfunction) (void))
{
  if (*once_control == 0)
    {
      *once_control = ~ 0;
      initfunction ();
    }
  return 0;
}

#endif
