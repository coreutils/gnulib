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

/* Which tests to perform.
   Uncomment some of these, to verify that all tests crash if no locking
   is enabled.  */
#define DO_TEST_COND 1
#define DO_TEST_TIMEDCOND 1


/* Whether to help the scheduler through explicit thrd_yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

#include <threads.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if HAVE_DECL_ALARM
# include <signal.h>
# include <unistd.h>
#endif

#include "virtualbox.h"
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


/*
 * Condition check
 */

/* Marked volatile so that different threads see the same value.  This is
   good enough in practice, although in theory stdatomic.h should be used.  */
static int volatile cond_value;
static cnd_t condtest;
static mtx_t lockcond;

static int
cnd_wait_routine (void *arg)
{
  ASSERT (mtx_lock (&lockcond) == thrd_success);
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
          ASSERT (cnd_wait (&condtest, &lockcond) == thrd_success);
        }
      while (!cond_value);
    }
  ASSERT (mtx_unlock (&lockcond) == thrd_success);

  cond_value = 2;

  return 0;
}

static int
test_cnd_wait ()
{
  int skipped = 0;
  thrd_t thread;
  int ret;

  cond_value = 0;

  /* Create a separate thread.  */
  ASSERT (thrd_create (&thread, cnd_wait_routine, &skipped) == thrd_success);

  /* Sleep for 2 seconds.  */
  {
    struct timespec remaining;

    remaining.tv_sec = 2;
    remaining.tv_nsec = 0;

    do
      {
        yield ();
        ret = thrd_sleep (&remaining, &remaining);
        ASSERT (ret >= -1);
      }
    while (ret == -1 && (remaining.tv_sec != 0 || remaining.tv_nsec != 0));
  }

  /* Tell one of the waiting threads (if any) to continue.  */
  ASSERT (mtx_lock (&lockcond) == thrd_success);
  cond_value = 1;
  ASSERT (cnd_signal (&condtest) == thrd_success);
  ASSERT (mtx_unlock (&lockcond) == thrd_success);

  ASSERT (thrd_join (thread, NULL) == thrd_success);

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

static int
cnd_timedwait_routine (void *arg)
{
  int ret;
  struct timespec ts;

  ASSERT (mtx_lock (&lockcond) == thrd_success);
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
          ret = cnd_timedwait (&condtest, &lockcond, &ts);
          if (ret == thrd_timedout)
            cond_timed_out = 1;
        }
      while (!cond_value);
    }
  ASSERT (mtx_unlock (&lockcond) == thrd_success);

  return 0;
}

static int
test_cnd_timedwait (void)
{
  int skipped = 0;
  thrd_t thread;
  int ret;

  cond_value = cond_timed_out = 0;

  /* Create a separate thread.  */
  ASSERT (thrd_create (&thread, cnd_timedwait_routine, &skipped)
          == thrd_success);

  /* Sleep for 2 seconds.  */
  {
    struct timespec remaining;

    remaining.tv_sec = 2;
    remaining.tv_nsec = 0;

    do
      {
        yield ();
        ret = thrd_sleep (&remaining, &remaining);
        ASSERT (ret >= -1);
      }
    while (ret == -1 && (remaining.tv_sec != 0 || remaining.tv_nsec != 0));
  }

  /* Tell one of the waiting threads (if any) to continue.  */
  ASSERT (mtx_lock (&lockcond) == thrd_success);
  cond_value = 1;
  ASSERT (cnd_signal (&condtest) == thrd_success);
  ASSERT (mtx_unlock (&lockcond) == thrd_success);

  ASSERT (thrd_join (thread, NULL) == thrd_success);

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

#if HAVE_DECL_ALARM
  /* Declare failure if test takes too long, by using default abort
     caused by SIGALRM.  */
  int alarm_value = 600;
  signal (SIGALRM, SIG_DFL);
  alarm (alarm_value);
#endif

  ASSERT (cnd_init (&condtest) == thrd_success);
  ASSERT (mtx_init (&lockcond, mtx_plain) == thrd_success);

#if DO_TEST_COND
  printf ("Starting test_cnd_wait ..."); fflush (stdout);
  {
    int skipped = test_cnd_wait ();
    printf (skipped ? " SKIP\n" : " OK\n"); fflush (stdout);
  }
#endif
#if DO_TEST_TIMEDCOND
  printf ("Starting test_cnd_timedwait ..."); fflush (stdout);
  {
    int skipped = test_cnd_timedwait ();
    printf (skipped ? " SKIP\n" : " OK\n"); fflush (stdout);
  }
#endif

  return test_exit_status;
}
