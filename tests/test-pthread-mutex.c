/* Test of locking in multithreaded situations.
   Copyright (C) 2005, 2008-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.  */

#include <config.h>

#if USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS || USE_WINDOWS_THREADS

/* Whether to enable locking.
   Uncomment this to get a test program without locking, to verify that
   it crashes.  */
#define ENABLE_LOCKING 1

/* Which tests to perform.
   Uncomment some of these, to verify that all tests crash if no locking
   is enabled.  */
#define DO_TEST_LOCK 1
#define DO_TEST_RECURSIVE_LOCK 1

/* Whether to help the scheduler through explicit sched_yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to use 'volatile' on some variables that communicate information
   between threads.  If set to 0, a semaphore or a lock is used to protect
   these variables.  If set to 1, 'volatile' is used; this is theoretically
   equivalent but can lead to much slower execution (e.g. 30x slower total
   run time on a 40-core machine), because 'volatile' does not imply any
   synchronization/communication between different CPUs.  */
#define USE_VOLATILE 0

#if USE_POSIX_THREADS && HAVE_SEMAPHORE_H
/* Whether to use a semaphore to communicate information between threads.
   If set to 0, a lock is used. If set to 1, a semaphore is used.
   Uncomment this to reduce the dependencies of this test.  */
# define USE_SEMAPHORE 1
/* Mac OS X provides only named semaphores (sem_open); its facility for
   unnamed semaphores (sem_init) does not work.  */
# if defined __APPLE__ && defined __MACH__
#  define USE_NAMED_SEMAPHORE 1
# else
#  define USE_UNNAMED_SEMAPHORE 1
# endif
#endif

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

/* Number of simultaneous threads.  */
#define THREAD_COUNT 10

/* Number of operations performed in each thread.
   This is quite high, because with a smaller count, say 5000, we often get
   an "OK" result even without ENABLE_LOCKING (on Linux/x86).  */
#define REPEAT_COUNT 50000

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if EXPLICIT_YIELD
# include <sched.h>
#endif

#if USE_SEMAPHORE
# include <errno.h>
# include <fcntl.h>
# include <semaphore.h>
# include <unistd.h>
#endif

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
# define yield() sched_yield ()
#else
# define yield()
#endif

#if USE_VOLATILE
struct atomic_int {
  volatile int value;
};
static void
init_atomic_int (struct atomic_int *ai)
{
}
static int
get_atomic_int_value (struct atomic_int *ai)
{
  return ai->value;
}
static void
set_atomic_int_value (struct atomic_int *ai, int new_value)
{
  ai->value = new_value;
}
#elif USE_SEMAPHORE
/* This atomic_int implementation can only support the values 0 and 1.
   It is initially 0 and can be set to 1 only once.  */
# if USE_UNNAMED_SEMAPHORE
struct atomic_int {
  sem_t semaphore;
};
#define atomic_int_semaphore(ai) (&(ai)->semaphore)
static void
init_atomic_int (struct atomic_int *ai)
{
  sem_init (&ai->semaphore, 0, 0);
}
# endif
# if USE_NAMED_SEMAPHORE
struct atomic_int {
  sem_t *semaphore;
};
#define atomic_int_semaphore(ai) ((ai)->semaphore)
static void
init_atomic_int (struct atomic_int *ai)
{
  sem_t *s;
  unsigned int count;
  for (count = 0; ; count++)
    {
      char name[80];
      /* Use getpid() in the name, so that different processes running at the
         same time will not interfere.  Use ai in the name, so that different
         atomic_int in the same process will not interfere.  Use a count in
         the name, so that even in the (unlikely) case that a semaphore with
         the specified name already exists, we can try a different name.  */
      sprintf (name, "test-lock-%lu-%p-%u",
               (unsigned long) getpid (), ai, count);
      s = sem_open (name, O_CREAT | O_EXCL, 0600, 0);
      if (s == SEM_FAILED)
        {
          if (errno == EEXIST)
            /* Retry with a different name.  */
            continue;
          else
            {
              perror ("sem_open failed");
              abort ();
            }
        }
      else
        {
          /* Try not to leave a semaphore hanging around on the file system
             eternally, if we can avoid it.  */
          sem_unlink (name);
          break;
        }
    }
  ai->semaphore = s;
}
# endif
static int
get_atomic_int_value (struct atomic_int *ai)
{
  if (sem_trywait (atomic_int_semaphore (ai)) == 0)
    {
      if (sem_post (atomic_int_semaphore (ai)))
        abort ();
      return 1;
    }
  else if (errno == EAGAIN)
    return 0;
  else
    abort ();
}
static void
set_atomic_int_value (struct atomic_int *ai, int new_value)
{
  if (new_value == 0)
    /* It's already initialized with 0.  */
    return;
  /* To set the value 1: */
  if (sem_post (atomic_int_semaphore (ai)))
    abort ();
}
#else
struct atomic_int {
  pthread_mutex_t lock;
  int value;
};
static void
init_atomic_int (struct atomic_int *ai)
{
  pthread_mutexattr_t attr;

  ASSERT (pthread_mutexattr_init (&attr) == 0);
  ASSERT (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_NORMAL) == 0);
  ASSERT (pthread_mutex_init (&ai->lock, &attr) == 0);
  ASSERT (pthread_mutexattr_destroy (&attr) == 0);
}
static int
get_atomic_int_value (struct atomic_int *ai)
{
  ASSERT (pthread_mutex_lock (&ai->lock) == 0);
  int ret = ai->value;
  ASSERT (pthread_mutex_unlock (&ai->lock) == 0);
  return ret;
}
static void
set_atomic_int_value (struct atomic_int *ai, int new_value)
{
  ASSERT (pthread_mutex_lock (&ai->lock) == 0);
  ai->value = new_value;
  ASSERT (pthread_mutex_unlock (&ai->lock) == 0);
}
#endif

/* Returns a reference to the current thread as a pointer, for debugging.  */
#if defined __MVS__
  /* On IBM z/OS, pthread_t is a struct with an 8-byte '__' field.
     The first three bytes of this field appear to uniquely identify a
     pthread_t, though not necessarily representing a pointer.  */
# define pthread_self_pointer() (*((void **) pthread_self ().__))
#else
# define pthread_self_pointer() ((void *) (uintptr_t) pthread_self ())
#endif

#define ACCOUNT_COUNT 4

static int account[ACCOUNT_COUNT];

static int
random_account (void)
{
  return ((unsigned int) rand () >> 3) % ACCOUNT_COUNT;
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

static pthread_mutex_t my_lock;

static void *
lock_mutator_thread (void *arg)
{
  int repeat;

  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      int i1, i2, value;

      dbgprintf ("Mutator %p before lock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_lock (&my_lock) == 0);
      dbgprintf ("Mutator %p after  lock\n", pthread_self_pointer ());

      i1 = random_account ();
      i2 = random_account ();
      value = ((unsigned int) rand () >> 3) % 10;
      account[i1] += value;
      account[i2] -= value;

      dbgprintf ("Mutator %p before unlock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_unlock (&my_lock) == 0);
      dbgprintf ("Mutator %p after  unlock\n", pthread_self_pointer ());

      dbgprintf ("Mutator %p before check lock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_lock (&my_lock) == 0);
      check_accounts ();
      ASSERT (pthread_mutex_unlock (&my_lock) == 0);
      dbgprintf ("Mutator %p after  check unlock\n", pthread_self_pointer ());

      yield ();
    }

  dbgprintf ("Mutator %p dying.\n", pthread_self_pointer ());
  return NULL;
}

static struct atomic_int lock_checker_done;

static void *
lock_checker_thread (void *arg)
{
  while (get_atomic_int_value (&lock_checker_done) == 0)
    {
      dbgprintf ("Checker %p before check lock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_lock (&my_lock) == 0);
      check_accounts ();
      ASSERT (pthread_mutex_unlock (&my_lock) == 0);
      dbgprintf ("Checker %p after  check unlock\n", pthread_self_pointer ());

      yield ();
    }

  dbgprintf ("Checker %p dying.\n", pthread_self_pointer ());
  return NULL;
}

static void
test_pthread_mutex_normal (void)
{
  int i;
  pthread_t checkerthread;
  pthread_t threads[THREAD_COUNT];

  /* Initialization.  */
  for (i = 0; i < ACCOUNT_COUNT; i++)
    account[i] = 1000;
  init_atomic_int (&lock_checker_done);
  set_atomic_int_value (&lock_checker_done, 0);

  /* Spawn the threads.  */
  ASSERT (pthread_create (&checkerthread, NULL, lock_checker_thread, NULL)
          == 0);
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (pthread_create (&threads[i], NULL, lock_mutator_thread, NULL) == 0);

  /* Wait for the threads to terminate.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (pthread_join (threads[i], NULL) == 0);
  set_atomic_int_value (&lock_checker_done, 1);
  ASSERT (pthread_join (checkerthread, NULL) == 0);
  check_accounts ();
}


/* -------------------------- Test recursive locks -------------------------- */

/* Test recursive locks by having several bank accounts and several threads
   which shuffle around money between the accounts (recursively) and another
   thread checking that all the money is still there.  */

static pthread_mutex_t my_reclock;

static void
recshuffle (void)
{
  int i1, i2, value;

  dbgprintf ("Mutator %p before lock\n", pthread_self_pointer ());
  ASSERT (pthread_mutex_lock (&my_reclock) == 0);
  dbgprintf ("Mutator %p after  lock\n", pthread_self_pointer ());

  i1 = random_account ();
  i2 = random_account ();
  value = ((unsigned int) rand () >> 3) % 10;
  account[i1] += value;
  account[i2] -= value;

  /* Recursive with probability 0.5.  */
  if (((unsigned int) rand () >> 3) % 2)
    recshuffle ();

  dbgprintf ("Mutator %p before unlock\n", pthread_self_pointer ());
  ASSERT (pthread_mutex_unlock (&my_reclock) == 0);
  dbgprintf ("Mutator %p after  unlock\n", pthread_self_pointer ());
}

static void *
reclock_mutator_thread (void *arg)
{
  int repeat;

  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      recshuffle ();

      dbgprintf ("Mutator %p before check lock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_lock (&my_reclock) == 0);
      check_accounts ();
      ASSERT (pthread_mutex_unlock (&my_reclock) == 0);
      dbgprintf ("Mutator %p after  check unlock\n", pthread_self_pointer ());

      yield ();
    }

  dbgprintf ("Mutator %p dying.\n", pthread_self_pointer ());
  return NULL;
}

static struct atomic_int reclock_checker_done;

static void *
reclock_checker_thread (void *arg)
{
  while (get_atomic_int_value (&reclock_checker_done) == 0)
    {
      dbgprintf ("Checker %p before check lock\n", pthread_self_pointer ());
      ASSERT (pthread_mutex_lock (&my_reclock) == 0);
      check_accounts ();
      ASSERT (pthread_mutex_unlock (&my_reclock) == 0);
      dbgprintf ("Checker %p after  check unlock\n", pthread_self_pointer ());

      yield ();
    }

  dbgprintf ("Checker %p dying.\n", pthread_self_pointer ());
  return NULL;
}

static void
test_pthread_mutex_recursive (void)
{
  int i;
  pthread_t checkerthread;
  pthread_t threads[THREAD_COUNT];

  /* Initialization.  */
  for (i = 0; i < ACCOUNT_COUNT; i++)
    account[i] = 1000;
  init_atomic_int (&reclock_checker_done);
  set_atomic_int_value (&reclock_checker_done, 0);

  /* Spawn the threads.  */
  ASSERT (pthread_create (&checkerthread, NULL, reclock_checker_thread, NULL)
          == 0);
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (pthread_create (&threads[i], NULL, reclock_mutator_thread, NULL)
            == 0);

  /* Wait for the threads to terminate.  */
  for (i = 0; i < THREAD_COUNT; i++)
    ASSERT (pthread_join (threads[i], NULL) == 0);
  set_atomic_int_value (&reclock_checker_done, 1);
  ASSERT (pthread_join (checkerthread, NULL) == 0);
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

  {
    pthread_mutexattr_t attr;

    ASSERT (pthread_mutexattr_init (&attr) == 0);
    ASSERT (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_NORMAL) == 0);
    ASSERT (pthread_mutex_init (&my_lock, &attr) == 0);
    ASSERT (pthread_mutexattr_destroy (&attr) == 0);
  }

  {
    pthread_mutexattr_t attr;

    ASSERT (pthread_mutexattr_init (&attr) == 0);
    ASSERT (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE) == 0);
    ASSERT (pthread_mutex_init (&my_reclock, &attr) == 0);
    ASSERT (pthread_mutexattr_destroy (&attr) == 0);
  }

#if DO_TEST_LOCK
  printf ("Starting test_pthread_mutex_normal ..."); fflush (stdout);
  test_pthread_mutex_normal ();
  printf (" OK\n"); fflush (stdout);
#endif
#if DO_TEST_RECURSIVE_LOCK
  printf ("Starting test_pthread_mutex_recursive ..."); fflush (stdout);
  test_pthread_mutex_recursive ();
  printf (" OK\n"); fflush (stdout);
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
