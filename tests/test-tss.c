/* Test of thread-local storage in multithreaded situations.
   Copyright (C) 2005, 2008-2019 Free Software Foundation, Inc.

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

/* Whether to help the scheduler through explicit yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

/* Number of simultaneous threads.  */
#define THREAD_COUNT 16

/* Number of operations performed in each thread.  */
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

static void
perhaps_yield (void)
{
  /* Call yield () only with a certain probability, otherwise the
     sequence of thread activations may be too predictable.  */
  if ((((unsigned int) rand () >> 3) % 4) == 0)
    yield ();
}


/* ----------------------- Test thread-local storage ----------------------- */

#define KEYS_COUNT 4

static tss_t mykeys[KEYS_COUNT];

static int
worker_thread (void *arg)
{
  unsigned int id = (unsigned int) (uintptr_t) arg;
  int i, j, repeat;
  unsigned int values[KEYS_COUNT];

  dbgprintf ("Worker %p started\n", thrd_current_pointer ());

  /* Initialize the per-thread storage.  */
  for (i = 0; i < KEYS_COUNT; i++)
    {
      values[i] = (((unsigned int) rand () >> 3) % 1000000) * THREAD_COUNT + id;
      /* Hopefully no arithmetic overflow.  */
      if ((values[i] % THREAD_COUNT) != id)
        abort ();
    }
  perhaps_yield ();

  /* Verify that the initial value is NULL.  */
  dbgprintf ("Worker %p before initial verify\n", thrd_current_pointer ());
  for (i = 0; i < KEYS_COUNT; i++)
    if (tss_get (mykeys[i]) != NULL)
      abort ();
  dbgprintf ("Worker %p after  initial verify\n", thrd_current_pointer ());
  perhaps_yield ();

  /* Initialize the per-thread storage.  */
  dbgprintf ("Worker %p before first tls_set\n", thrd_current_pointer ());
  for (i = 0; i < KEYS_COUNT; i++)
    {
      unsigned int *ptr = (unsigned int *) malloc (sizeof (unsigned int));
      *ptr = values[i];
      ASSERT (tss_set (mykeys[i], ptr) == thrd_success);
    }
  dbgprintf ("Worker %p after  first tls_set\n", thrd_current_pointer ());
  perhaps_yield ();

  /* Shuffle around the pointers.  */
  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      dbgprintf ("Worker %p doing value swapping\n", thrd_current_pointer ());
      i = ((unsigned int) rand () >> 3) % KEYS_COUNT;
      j = ((unsigned int) rand () >> 3) % KEYS_COUNT;
      if (i != j)
        {
          void *vi = tss_get (mykeys[i]);
          void *vj = tss_get (mykeys[j]);

          ASSERT (tss_set (mykeys[i], vj) == thrd_success);
          ASSERT (tss_set (mykeys[j], vi) == thrd_success);
        }
      perhaps_yield ();
    }

  /* Verify that all the values are from this thread.  */
  dbgprintf ("Worker %p before final verify\n", thrd_current_pointer ());
  for (i = 0; i < KEYS_COUNT; i++)
    if ((*(unsigned int *) tss_get (mykeys[i]) % THREAD_COUNT) != id)
      abort ();
  dbgprintf ("Worker %p after  final verify\n", thrd_current_pointer ());
  perhaps_yield ();

  dbgprintf ("Worker %p dying.\n", thrd_current_pointer ());
  return 0;
}

static void
test_tss (void)
{
  int pass, i;

  for (pass = 0; pass < 2; pass++)
    {
      thrd_t threads[THREAD_COUNT];

      if (pass == 0)
        for (i = 0; i < KEYS_COUNT; i++)
          ASSERT (tss_create (&mykeys[i], free) == thrd_success);
      else
        for (i = KEYS_COUNT - 1; i >= 0; i--)
          ASSERT (tss_create (&mykeys[i], free) == thrd_success);

      /* Spawn the threads.  */
      for (i = 0; i < THREAD_COUNT; i++)
        ASSERT (thrd_create (&threads[i], worker_thread, NULL) == thrd_success);

      /* Wait for the threads to terminate.  */
      for (i = 0; i < THREAD_COUNT; i++)
        ASSERT (thrd_join (threads[i], NULL) == thrd_success);

      for (i = 0; i < KEYS_COUNT; i++)
        tss_delete (mykeys[i]);
    }
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

  printf ("Starting test_tss ..."); fflush (stdout);
  test_tss ();
  printf (" OK\n"); fflush (stdout);

  return 0;
}
