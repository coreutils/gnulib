/* Test of async-safe sequential list data type implementation.
   Copyright (C) 2021 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#include <config.h>

/* Work around GCC bug 44511.  */
#if 4 < __GNUC__ + (3 <= __GNUC_MINOR__)
# pragma GCC diagnostic ignored "-Wreturn-type"
#endif

#include "gl_linked_list.h"

#if SIGNAL_SAFE_LIST

# if USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS /* || USE_WINDOWS_THREADS */
/* This test works only with POSIX compatible threads.
   With Windows threads, send_signal() has to be implemented as
     raise (SIGINT);
   hence only test == 2 tests anything, and in fact only 5 signals arrive.
   This small test is not able to detect a buggy implementation.  Therefore
   mark the test as skipped in this case.  */

#  include <signal.h>
#  include <stdint.h>
#  include <stdlib.h>
#  include <unistd.h>
#  include <time.h>

#  include "glthread/thread.h"
#  include "glthread/yield.h"

#  include "macros.h"

#  define RANDOM(n) (rand () % (n))
#  define RANDOM_OBJECT() ((void *) (uintptr_t) RANDOM (10000))

/* test == 1: signals are executed in an idle thread.
   test == 2: signals are executed in the signal-sending thread.
   test == 3: signals are executed in the mutator thread.  */
static int volatile test;

static uintptr_t volatile sum_before_current_operation;
static uintptr_t volatile sum_after_current_operation;

static gl_list_t volatile list1;

static gl_thread_t volatile signal_target;

/* Statistics.  */
static unsigned int volatile num_mutations;
static unsigned int volatile num_signals_sent;
static unsigned int volatile num_signals_arrived;

/* Blocks the SIGINT signal in the current thread.  */
static void
block_sigint (void)
{
  sigset_t mask;

  sigemptyset (&mask);
  sigaddset (&mask, SIGINT);
  sigprocmask (SIG_BLOCK, &mask, NULL); /* equivalent to pthread_sigmask */
}

/* This thread is idle.  */
static void *
idle_thread (void *arg)
{
  for (;;)
    gl_thread_yield ();

  /*NOTREACHED*/
}

/* The signal handler iterates through the list and verifies that the sum of
   all elements in the list is one of the two allowed values.  */
static _GL_ASYNC_SAFE void
sigint_handler (int signo)
{
  num_signals_arrived++;

  uintptr_t sum = 0;
  {
    gl_list_iterator_t iter = gl_list_iterator (list1);
    const void *elt;

    while (gl_list_iterator_next (&iter, &elt, NULL))
      sum += (uintptr_t) elt;
    gl_list_iterator_free (&iter);
  }
  if (!(sum == sum_before_current_operation
        || sum == sum_after_current_operation))
    {
      /* POSIX does not allow uses of stdio from within a signal handler, but
         it should work here, because the rest of the program does not use
         stdio.  */
      fprintf (stderr, "sum = %lu, expected %lu or %lu\n",
               (unsigned long) sum,
               (unsigned long) sum_before_current_operation,
               (unsigned long) sum_after_current_operation);
      fflush (stderr);
      abort ();
    }
}

/* Sends a SIGINT signal to the current process.  */
static void
send_signal (void)
{
#if 0
  /* This does not work for test != 2, because raise() sends the signal to the
     current thread always, and if test != 2 the signal is eternally blocked
     in the current thread; thus it will never get delivered.  */
  raise (SIGINT);
#else
  /* This works: Either
       kill (getpid (), SIGINT);
     or
       pthread_kill (signal_target, SIGINT);
     The difference is that for kill(), the OS determines the (only) thread that
     has not blocked the signal, whereas for pthread_kill() we have manually
     determined this thread.  */
  kill (getpid (), SIGINT);
#endif
}

/* This thread permanently sends SIGINT signals.  */
static void *
signal_sending_thread (void *arg)
{
  if (test != 2)
    block_sigint ();

  int repeat;

  for (repeat = 1000; repeat > 0; repeat--)
    {
      num_signals_sent++; send_signal ();
      num_signals_sent++; send_signal ();
      num_signals_sent++; send_signal ();
      num_signals_sent++; send_signal ();
      num_signals_sent++; send_signal ();
      {
        struct timespec ts;
        ts.tv_sec = 0; ts.tv_nsec = 1000000;
        nanosleep (&ts, NULL);
      }
      gl_thread_yield ();
    }

  printf ("Sent %u signals. Received %u signals. Done after %u mutations.\n",
          num_signals_sent, num_signals_arrived, num_mutations);

  exit (0);

  /*NOTREACHED*/
}

/* This thread repeatedly adds and removes objects from the list.  */
static void *
mutator_thread (void *arg)
{
  if (test != 3)
    block_sigint ();

  gl_list_t list2 = (gl_list_t) arg;

  for (num_mutations = 0; ; num_mutations++)
    {
      unsigned int operation = RANDOM (2);
      switch (operation)
        {
        case 0: /* Add an element.  */
          {
            const void *obj = RANDOM_OBJECT ();
            ASSERT (gl_list_nx_add_first (list2, obj) != NULL);
            sum_after_current_operation =
              sum_before_current_operation + (uintptr_t) obj;
            ASSERT (gl_list_nx_add_first (list1, obj) != NULL);
            sum_before_current_operation = sum_after_current_operation;
          }
          break;
        case 1: /* Remove an element.  */
          if (gl_list_size (list2) > 0)
            {
              size_t index = RANDOM (gl_list_size (list2));
              const void *obj = gl_list_get_at (list2, index);
              ASSERT (gl_list_remove (list2, obj));
              sum_after_current_operation =
                sum_before_current_operation - (uintptr_t) obj;
              ASSERT (gl_list_remove (list1, obj));
              sum_before_current_operation = sum_after_current_operation;
            }
          break;
        }
    }

  /*NOTREACHED*/
}

int
main (int argc, char *argv[])
{
  test = atoi (argv[1]);

  /* Allow the user to provide a non-default random seed on the command line.  */
  if (argc > 2)
    srand (atoi (argv[2]));

  gl_list_t list2;
  /* Initialize list1 and list2.  */
  {
    size_t initial_size = RANDOM (50);
    const void **contents =
      (const void **) malloc (initial_size * sizeof (const void *));
    size_t i;

    for (i = 0; i < initial_size; i++)
      contents[i] = RANDOM_OBJECT ();

    list1 = gl_list_nx_create_empty (GL_LINKED_LIST, NULL, NULL, NULL, true);
    ASSERT (list1 != NULL);
    for (i = 0; i < initial_size; i++)
      ASSERT (gl_list_nx_add_first (list1, contents[i]) != NULL);

    list2 = gl_list_nx_create_empty (GL_LINKED_LIST, NULL, NULL, NULL, true);
    ASSERT (list2 != NULL);
    for (i = 0; i < initial_size; i++)
      ASSERT (gl_list_nx_add_last (list2, contents[i]) != NULL);

    uintptr_t initial_sum = 0;
    for (i = 0; i < initial_size; i++)
      initial_sum += (uintptr_t) contents[i];
    sum_before_current_operation = initial_sum;
    sum_after_current_operation = initial_sum;
  }

  /* Install the signal handler.
     It needs to be done with sigaction(), not signal(), otherwise on Solaris
     and AIX the program crashes at the second incoming SIGINT.  */
  #if 0
  signal (SIGINT, sigint_handler);
  #else
  {
    struct sigaction action;
    action.sa_handler = sigint_handler;
    action.sa_flags = SA_RESTART | SA_NODEFER;
    sigemptyset (&action.sa_mask);
    ASSERT (sigaction (SIGINT, &action, NULL) == 0);
  }
  #endif

  /* Launch the threads.  */
  switch (test)
    {
    case 1:
      {
        signal_target = gl_thread_create (idle_thread, NULL);
        gl_thread_create (mutator_thread, list2);
        signal_sending_thread (NULL);
        abort ();
      }

    case 2:
      {
        gl_thread_create (mutator_thread, list2);
        signal_target = gl_thread_self (); signal_sending_thread (NULL);
        abort ();
      }

    case 3:
      {
        signal_target = gl_thread_create (mutator_thread, list2);
        signal_sending_thread (NULL);
        abort ();
      }

    default:
      ASSERT (false);
    }
}

# else

#  include <stdio.h>

int
main ()
{
  fputs ("Skipping test: POSIX compatible multithreading not enabled\n", stderr);
  return 77;
}

# endif

#else

# include <stdio.h>

int
main ()
{
  fputs ("Skipping test: signal-safety of linked lists is not enabled\n", stderr);
  return 77;
}

#endif
