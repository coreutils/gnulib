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

/* Which tests to perform.
   Uncomment some of these, to verify that all tests crash if no locking
   is enabled.  */
#define DO_TEST_LOCK 1
#define DO_TEST_RECURSIVE_LOCK 1

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

#if HAVE_DECL_ALARM
# include <signal.h>
# include <unistd.h>
#endif

#include "macros.h"
#include "atomic-int-isoc.h"

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

#define ACCOUNT_COUNT 4

static int account[ACCOUNT_COUNT];

static int
random_account (void)
{
  return ((unsigned long) random () >> 3) % ACCOUNT_COUNT;
}

static void
check_accounts (void)
{
  int i, sum;

  sum = 0;
  for (i = 0; i < ACCOUNT_COUNT; i++)
    sum += account[i];
  if (sum != ACCOUNT_COUNT * 1000)
    abort ();
}


/* ------------------- Test normal (non-recursive) locks ------------------- */

/* Test normal locks by having several bank accounts and several threads
   which shuffle around money between the accounts and another thread
   checking that all the money is still there.  */

static mtx_t my_lock;

static int
lock_mutator_thread (void *arg)
{
  int repeat;

  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      int i1, i2, value;

      dbgprintf ("Mutator %p before lock\n", thrd_current_pointer ());
      ASSERT (mtx_lock (&my_lock) == thrd_success);
      dbgprintf ("Mutator %p after  lock\n", thrd_current_pointer ());

      i1 = random_account ();
      i2 = random_account ();
      value = ((unsigned long) random () >> 3) % 10;
      account[i1] += value;
      account[i2] -= value;

      dbgprintf ("Mutator %p before unlock\n", thrd_current_pointer ());
      ASSERT (mtx_unlock (&my_lock) == thrd_success);
      dbgprintf ("Mutator %p after  unlock\n", thrd_current_pointer ());

      dbgprintf ("Mutator %p before check lock\n", thrd_current_pointer ());
      ASSERT (mtx_lock (&my_lock) == thrd_success);
      check_accounts ();
      ASSERT (mtx_unlock (&my_lock) == thrd_success);
      dbgprintf ("Mutator %p after  check unlock\n", thrd_current_pointer ());

      yield ();
    }

  dbgprintf ("Mutator %p dying.\n", thrd_current_pointer ());
  return 0;
}

static struct atomic_int lock_checker_done;

static int
lock_checker_thread (void *arg)
{
  while (get_atomic_int_value (&lock_checker_done) == 0)
    {
      dbgprintf ("Checker %p before check lock\n", thrd_current_pointer ());
      ASSERT (mtx_lock (&my_lock) == thrd_success);
      check_accounts ();
      ASSERT (mtx_unlock (&my_lock) == thrd_success);
      dbgprintf ("Checker %p after  check unlock\n", thrd_current_pointer ());

      yield ();
    }

  dbgprintf ("Checker %p dying.\n", thrd_current_pointer ());
  return 0;
}

static void
test_mtx_plain (void)
{
  int i;
  thrd_t checkerthread;
  thrd_t threads[THREAD_COUNT];

  /* Initialization.  */
  for (i = 0; i < ACCOUNT_COUNT; i++)
    account[i] = 1000;
  init_atomic_int (&lock_checker_done);
  set_atomic_int_value (&lock_checker_done, 0);

  /* Spawn the threads.  */
  ASSERT (thrd_create (&checkerthread, lock_checker_thread, NULL)
          == thrd_success);
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_create (&threads[i], lock_mutator_thread, NULL)
            == thrd_success);

  /* Wait for the threads to terminate.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_join (threads[i], NULL) == thrd_success);
  set_atomic_int_value (&lock_checker_done, 1);
  ASSERT (thrd_join (checkerthread, NULL) == thrd_success);
  check_accounts ();
}


/* -------------------------- Test recursive locks -------------------------- */

/* Test recursive locks by having several bank accounts and several threads
   which shuffle around money between the accounts (recursively) and another
   thread checking that all the money is still there.  */

static mtx_t my_reclock;

static void
recshuffle (void)
{
  int i1, i2, value;

  dbgprintf ("Mutator %p before lock\n", thrd_current_pointer ());
  ASSERT (mtx_lock (&my_reclock) == thrd_success);
  dbgprintf ("Mutator %p after  lock\n", thrd_current_pointer ());

  i1 = random_account ();
  i2 = random_account ();
  value = ((unsigned long) random () >> 3) % 10;
  account[i1] += value;
  account[i2] -= value;

  /* Recursive with probability 0.5.  */
  if (((unsigned long) random () >> 3) % 2)
    recshuffle ();

  dbgprintf ("Mutator %p before unlock\n", thrd_current_pointer ());
  ASSERT (mtx_unlock (&my_reclock) == thrd_success);
  dbgprintf ("Mutator %p after  unlock\n", thrd_current_pointer ());
}

static int
reclock_mutator_thread (void *arg)
{
  int repeat;

  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      recshuffle ();

      dbgprintf ("Mutator %p before check lock\n", thrd_current_pointer ());
      ASSERT (mtx_lock (&my_reclock) == thrd_success);
      check_accounts ();
      ASSERT (mtx_unlock (&my_reclock) == thrd_success);
      dbgprintf ("Mutator %p after  check unlock\n", thrd_current_pointer ());

      yield ();
    }

  dbgprintf ("Mutator %p dying.\n", thrd_current_pointer ());
  return 0;
}

static struct atomic_int reclock_checker_done;

static int
reclock_checker_thread (void *arg)
{
  while (get_atomic_int_value (&reclock_checker_done) == 0)
    {
      dbgprintf ("Checker %p before check lock\n", thrd_current_pointer ());
      ASSERT (mtx_lock (&my_reclock) == thrd_success);
      check_accounts ();
      ASSERT (mtx_unlock (&my_reclock) == thrd_success);
      dbgprintf ("Checker %p after  check unlock\n", thrd_current_pointer ());

      yield ();
    }

  dbgprintf ("Checker %p dying.\n", thrd_current_pointer ());
  return 0;
}

static void
test_mtx_recursive (void)
{
  int i;
  thrd_t checkerthread;
  thrd_t threads[THREAD_COUNT];

  /* Initialization.  */
  for (i = 0; i < ACCOUNT_COUNT; i++)
    account[i] = 1000;
  init_atomic_int (&reclock_checker_done);
  set_atomic_int_value (&reclock_checker_done, 0);

  /* Spawn the threads.  */
  ASSERT (thrd_create (&checkerthread, reclock_checker_thread, NULL)
          == thrd_success);
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_create (&threads[i], reclock_mutator_thread, NULL)
            == thrd_success);

  /* Wait for the threads to terminate.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (thrd_join (threads[i], NULL) == thrd_success);
  set_atomic_int_value (&reclock_checker_done, 1);
  ASSERT (thrd_join (checkerthread, NULL) == thrd_success);
  check_accounts ();
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

  ASSERT (mtx_init (&my_lock, mtx_plain) == thrd_success);
  ASSERT (mtx_init (&my_reclock, mtx_plain | mtx_recursive) == thrd_success);

#if DO_TEST_LOCK
  printf ("Starting test_mtx_plain ..."); fflush (stdout);
  test_mtx_plain ();
  printf (" OK\n"); fflush (stdout);
#endif
#if DO_TEST_RECURSIVE_LOCK
  printf ("Starting test_mtx_recursive ..."); fflush (stdout);
  test_mtx_recursive ();
  printf (" OK\n"); fflush (stdout);
#endif

  return test_exit_status;
}
