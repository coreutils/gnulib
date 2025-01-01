/* Test of locking in multithreaded situations.
   Copyright (C) 2005, 2008-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.  */

#include <config.h>

/* Whether to enable locking.
   Uncomment this to get a test program without locking, to verify that
   it crashes.  */
#define ENABLE_LOCKING 1

/* Whether to help the scheduler through explicit thrd_yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

/* Number of simultaneous threads.  */
#define THREAD_COUNT 10

/* Number of operations performed in each thread.
   This is quite high, because with a smaller count, say 5000, we often get
   an "OK" result even without ENABLE_LOCKING (on Linux/x86).  */
#define REPEAT_COUNT 50000

#include <threads.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glthread/lock.h"

#if HAVE_DECL_ALARM
# include <signal.h>
# include <unistd.h>
#endif

#include "macros.h"

#if ENABLE_DEBUGGING
# define dbgprintf printf
#else
# define dbgprintf if (0) printf
#endif

#if EXPLICIT_YIELD
# define yield() thrd_yield ()
#else
# define yield()
#endif

/* Returns a reference to the current thread as a pointer, for debugging.  */
#if defined __MVS__
  /* On IBM z/OS, pthread_t is a struct with an 8-byte '__' field.
     The first three bytes of this field appear to uniquely identify a
     pthread_t, though not necessarily representing a pointer.  */
# define thrd_current_pointer() (*((void **) thrd_current ().__))
#elif defined __sun
  /* On Solaris, thrd_t is merely an 'unsigned int'.  */
# define thrd_current_pointer() ((void *) (uintptr_t) thrd_current ())
#else
# define thrd_current_pointer() ((void *) thrd_current ())
#endif


/* ------------------------ Test once-only execution ------------------------ */

/* Test once-only execution by having several threads attempt to grab a
   once-only task simultaneously (triggered by releasing a read-write lock).  */

static once_flag fresh_once = ONCE_FLAG_INIT;
static int ready[THREAD_COUNT];
static mtx_t ready_lock[THREAD_COUNT];
#if ENABLE_LOCKING
static gl_rwlock_t fire_signal[REPEAT_COUNT];
#else
static volatile int fire_signal_state;
#endif
static once_flag once_control;
static int performed;
static mtx_t performed_lock;

static void
once_execute (void)
{
  ASSERT (mtx_lock (&performed_lock) == thrd_success);
  performed++;
  ASSERT (mtx_unlock (&performed_lock) == thrd_success);
}

static int
once_contender_thread (void *arg)
{
  int id = (int) (intptr_t) arg;
  int repeat;

  for (repeat = 0; repeat <= REPEAT_COUNT; repeat++)
    {
      /* Tell the main thread that we're ready.  */
      ASSERT (mtx_lock (&ready_lock[id]) == thrd_success);
      ready[id] = 1;
      ASSERT (mtx_unlock (&ready_lock[id]) == thrd_success);

      if (repeat == REPEAT_COUNT)
        break;

      dbgprintf ("Contender %p waiting for signal for round %d\n",
                 thrd_current_pointer (), repeat);
#if ENABLE_LOCKING
      /* Wait for the signal to go.  */
      gl_rwlock_rdlock (fire_signal[repeat]);
      /* And don't hinder the others (if the scheduler is unfair).  */
      gl_rwlock_unlock (fire_signal[repeat]);
#else
      /* Wait for the signal to go.  */
      while (fire_signal_state <= repeat)
        yield ();
#endif
      dbgprintf ("Contender %p got the     signal for round %d\n",
                 thrd_current_pointer (), repeat);

      /* Contend for execution.  */
      call_once (&once_control, once_execute);
    }

  return 0;
}

static void
test_once (void)
{
  int i, repeat;
  thrd_t threads[THREAD_COUNT];

  /* Initialize all variables.  */
  for (i = 0; i < THREAD_COUNT; i++)
    {
      ready[i] = 0;
      ASSERT (mtx_init (&ready_lock[i], mtx_plain) == thrd_success);
    }
#if ENABLE_LOCKING
  for (i = 0; i < REPEAT_COUNT; i++)
    gl_rwlock_init (fire_signal[i]);
#else
  fire_signal_state = 0;
#endif

#if ENABLE_LOCKING
  /* Block all fire_signals.  */
  for (i = REPEAT_COUNT-1; i >= 0; i--)
    gl_rwlock_wrlock (fire_signal[i]);
#endif

  /* Spawn the threads.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_create (&threads[i],
                         once_contender_thread, (void *) (intptr_t) i)
            == thrd_success);

  for (repeat = 0; repeat <= REPEAT_COUNT; repeat++)
    {
      /* Wait until every thread is ready.  */
      dbgprintf ("Main thread before synchronizing for round %d\n", repeat);
      for (;;)
        {
          int ready_count = 0;
          for (i = 0; i < THREAD_COUNT; i++)
            {
              ASSERT (mtx_lock (&ready_lock[i]) == thrd_success);
              ready_count += ready[i];
              ASSERT (mtx_unlock (&ready_lock[i]) == thrd_success);
            }
          if (ready_count == THREAD_COUNT)
            break;
          yield ();
        }
      dbgprintf ("Main thread after  synchronizing for round %d\n", repeat);

      if (repeat > 0)
        {
          /* Check that exactly one thread executed the once_execute()
             function.  */
          if (performed != 1)
            abort ();
        }

      if (repeat == REPEAT_COUNT)
        break;

      /* Preparation for the next round: Initialize once_control.  */
      memcpy (&once_control, &fresh_once, sizeof (once_flag));

      /* Preparation for the next round: Reset the performed counter.  */
      performed = 0;

      /* Preparation for the next round: Reset the ready flags.  */
      for (i = 0; i < THREAD_COUNT; i++)
        {
          ASSERT (mtx_lock (&ready_lock[i]) == thrd_success);
          ready[i] = 0;
          ASSERT (mtx_unlock (&ready_lock[i]) == thrd_success);
        }

      /* Signal all threads simultaneously.  */
      dbgprintf ("Main thread giving signal for round %d\n", repeat);
#if ENABLE_LOCKING
      gl_rwlock_unlock (fire_signal[repeat]);
#else
      fire_signal_state = repeat + 1;
#endif
    }

  /* Wait for the threads to terminate.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_join (threads[i], NULL) == thrd_success);
}


/* -------------------------------------------------------------------------- */

int
main ()
{
#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  */
  int alarm_value = 600;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  ASSERT (mtx_init (&performed_lock, mtx_plain) == thrd_success);

  printf ("Starting test_once ..."); fflush (stdout);
  test_once ();
  printf (" OK\n"); fflush (stdout);

  return test_exit_status;
}
