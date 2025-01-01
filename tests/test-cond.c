/* Test of condition variables in multithreaded situations.
   Copyright (C) 2008-2025 Free Software Foundation, Inc.

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

#include <config.h>

#if USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS || USE_WINDOWS_THREADS

/* Which tests to perform.
   Uncomment some of these, to verify that all tests crash if no locking
   is enabled.  */
#define DO_TEST_COND 1
#define DO_TEST_TIMEDCOND 1


/* Whether to help the scheduler through explicit yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "glthread/cond.h"
#include "glthread/lock.h"
#include "glthread/thread.h"
#include "glthread/yield.h"

#include "virtualbox.h"

#if ENABLE_DEBUGGING
# define dbgprintf printf
#else
# define dbgprintf if (0) printf
#endif

#if EXPLICIT_YIELD
# define yield() gl_thread_yield ()
#else
# define yield()
#endif


/*
 * Condition check
 */

/* Marked volatile so that different threads see the same value.  This is
   good enough in practice, although in theory stdatomic.h should be used.  */
static int volatile cond_value;
gl_cond_define_initialized(static, condtest)
gl_lock_define_initialized(static, lockcond)

static void *
cond_routine (void *arg)
{
  gl_lock_lock (lockcond);
  if (cond_value)
    {
      /* The main thread already slept, and nevertheless this thread comes
         too late.  */
      *(int *)arg = 1;
    }
  else
    {
      do
        {
          gl_cond_wait (condtest, lockcond);
        }
      while (!cond_value);
    }
  gl_lock_unlock (lockcond);

  cond_value = 2;

  return NULL;
}

static int
test_cond ()
{
  int skipped = 0;
  gl_thread_t thread;

  cond_value = 0;

  /* Create a separate thread.  */
  thread = gl_thread_create (cond_routine, &skipped);

  /* Sleep for 2 seconds.  */
  {
    int remaining = 2;

    do
      {
        yield ();
       remaining = sleep (remaining);
      }
    while (remaining);
  }

  /* Tell one of the waiting threads (if any) to continue.  */
  gl_lock_lock (lockcond);
  cond_value = 1;
  gl_cond_signal (condtest);
  gl_lock_unlock (lockcond);

  gl_thread_join (thread, NULL);

  if (cond_value != 2)
    abort ();

  return skipped;
}


/*
 * Timed Condition check
 */

/* Marked volatile so that different threads see the same value.  This is
   good enough in practice, although in theory stdatomic.h should be used.  */
static int volatile cond_timed_out;

/* Stores in *TS the current time plus 1 second.  */
static void
get_ts (struct timespec *ts)
{
  struct timeval now;

  gettimeofday (&now, NULL);

  ts->tv_sec = now.tv_sec + 1;
  ts->tv_nsec = now.tv_usec * 1000;
}

static void *
timedcond_routine (void *arg)
{
  int ret;
  struct timespec ts;

  gl_lock_lock (lockcond);
  if (cond_value)
    {
      /* The main thread already slept, and nevertheless this thread comes
         too late.  */
      *(int *)arg = 1;
    }
  else
    {
      do
        {
          get_ts (&ts);
          ret = glthread_cond_timedwait (&condtest, &lockcond, &ts);
          if (ret == ETIMEDOUT)
            cond_timed_out = 1;
        }
      while (!cond_value);
    }
  gl_lock_unlock (lockcond);

  return NULL;
}

static int
test_timedcond (void)
{
  int skipped = 0;
  gl_thread_t thread;

  cond_value = cond_timed_out = 0;

  /* Create a separate thread.  */
  thread = gl_thread_create (timedcond_routine, &skipped);

  /* Sleep for 2 seconds.  */
  {
    int remaining = 2;

    do
      {
        yield ();
        remaining = sleep (remaining);
      }
    while (remaining);
  }

  /* Tell one of the waiting threads (if any) to continue.  */
  gl_lock_lock (lockcond);
  cond_value = 1;
  gl_cond_signal (condtest);
  gl_lock_unlock (lockcond);

  gl_thread_join (thread, NULL);

  if (!cond_timed_out)
    abort ();

  return skipped;
}


int
main ()
{
  /* This test occasionally fails on Linux (glibc or musl libc), in a
     VirtualBox VM with paravirtualization = Default or KVM, with ≥ 2 CPUs.
     Skip the test in this situation.  */
  if (is_running_under_virtualbox_kvm () && num_cpus () > 1)
    {
      fputs ("Skipping test: avoiding VirtualBox bug with KVM paravirtualization\n",
             stderr);
      return 77;
    }

#if DO_TEST_COND
  printf ("Starting test_cond ..."); fflush (stdout);
  {
    int skipped = test_cond ();
    printf (skipped ? " SKIP\n" : " OK\n"); fflush (stdout);
  }
#endif
#if DO_TEST_TIMEDCOND
  printf ("Starting test_timedcond ..."); fflush (stdout);
  {
    int skipped = test_timedcond ();
    printf (skipped ? " SKIP\n" : " OK\n"); fflush (stdout);
  }
#endif

  return 0;
}

#else

/* No multithreading available.  */

#include <stdio.h>

int
main ()
{
  fputs ("Skipping test: multithreading not enabled\n", stderr);
  return 77;
}

#endif
