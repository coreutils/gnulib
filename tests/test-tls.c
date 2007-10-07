/* Test of thread-local storage in multithreaded situations.
   Copyright (C) 2005 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.  */

#include <config.h>

#if USE_POSIX_THREADS || USE_SOLARIS_THREADS || USE_PTH_THREADS || USE_WIN32_THREADS

#if USE_POSIX_THREADS
# define TEST_POSIX_THREADS 1
#endif
#if USE_SOLARIS_THREADS
# define TEST_SOLARIS_THREADS 1
#endif
#if USE_PTH_THREADS
# define TEST_PTH_THREADS 1
#endif
#if USE_WIN32_THREADS
# define TEST_WIN32_THREADS 1
#endif

/* Whether to help the scheduler through explicit yield().
   Uncomment this to see if the operating system has a fair scheduler.  */
#define EXPLICIT_YIELD 1

/* Whether to print debugging messages.  */
#define ENABLE_DEBUGGING 0

/* Number of simultaneous threads.  */
#define THREAD_COUNT 16

/* Number of operations performed in each thread.  */
#define REPEAT_COUNT 50000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tls.h"

#if ENABLE_DEBUGGING
# define dbgprintf printf
#else
# define dbgprintf if (0) printf
#endif

#if TEST_POSIX_THREADS
# include <pthread.h>
# include <sched.h>
typedef pthread_t gl_thread_t;
static inline gl_thread_t gl_thread_create (void * (*func) (void *), void *arg)
{
  pthread_t thread;
  if (pthread_create (&thread, NULL, func, arg) != 0)
    abort ();
  return thread;
}
static inline void gl_thread_join (gl_thread_t thread)
{
  void *retval;
  if (pthread_join (thread, &retval) != 0)
    abort ();
}
static inline void gl_thread_yield (void)
{
  sched_yield ();
}
static inline void * gl_thread_self (void)
{
  return (void *) pthread_self ();
}
#endif
#if TEST_PTH_THREADS
# include <pth.h>
typedef pth_t gl_thread_t;
static inline gl_thread_t gl_thread_create (void * (*func) (void *), void *arg)
{
  pth_t thread = pth_spawn (NULL, func, arg);
  if (thread == NULL)
    abort ();
  return thread;
}
static inline void gl_thread_join (gl_thread_t thread)
{
  if (!pth_join (thread, NULL))
    abort ();
}
static inline void gl_thread_yield (void)
{
  pth_yield (NULL);
}
static inline void * gl_thread_self (void)
{
  return pth_self ();
}
#endif
#if TEST_SOLARIS_THREADS
# include <thread.h>
typedef thread_t gl_thread_t;
static inline gl_thread_t gl_thread_create (void * (*func) (void *), void *arg)
{
  thread_t thread;
  if (thr_create (NULL, 0, func, arg, 0, &thread) != 0)
    abort ();
  return thread;
}
static inline void gl_thread_join (gl_thread_t thread)
{
  void *retval;
  if (thr_join (thread, NULL, &retval) != 0)
    abort ();
}
static inline void gl_thread_yield (void)
{
  thr_yield ();
}
static inline void * gl_thread_self (void)
{
  return (void *) thr_self ();
}
#endif
#if TEST_WIN32_THREADS
# include <windows.h>
typedef HANDLE gl_thread_t;
/* Use a wrapper function, instead of adding WINAPI through a cast.  */
struct wrapper_args { void * (*func) (void *); void *arg; };
static DWORD WINAPI wrapper_func (void *varg)
{
  struct wrapper_args *warg = (struct wrapper_args *)varg;
  void * (*func) (void *) = warg->func;
  void *arg = warg->arg;
  free (warg);
  func (arg);
  return 0;
}
static inline gl_thread_t gl_thread_create (void * (*func) (void *), void *arg)
{
  struct wrapper_args *warg =
    (struct wrapper_args *) malloc (sizeof (struct wrapper_args));
  if (warg == NULL)
    abort ();
  warg->func = func;
  warg->arg = arg;
  {
    DWORD thread_id;
    HANDLE thread =
      CreateThread (NULL, 100000, wrapper_func, warg, 0, &thread_id);
    if (thread == NULL)
      abort ();
    return thread;
  }
}
static inline void gl_thread_join (gl_thread_t thread)
{
  if (WaitForSingleObject (thread, INFINITE) == WAIT_FAILED)
    abort ();
  if (!CloseHandle (thread))
    abort ();
}
static inline void gl_thread_yield (void)
{
  Sleep (0);
}
static inline void * gl_thread_self (void)
{
  return (void *) GetCurrentThreadId ();
}
#endif
#if EXPLICIT_YIELD
# define yield() gl_thread_yield ()
#else
# define yield()
#endif

static inline void
perhaps_yield (void)
{
  /* Call yield () only with a certain probability, otherwise with GNU Pth
     the sequence of thread activations is too predictable.  */
  if ((((unsigned int) rand () >> 3) % 4) == 0)
    yield ();
}

#define KEYS_COUNT 4

static gl_tls_key_t mykeys[KEYS_COUNT];

static void *
worker_thread (void *arg)
{
  unsigned int id = (unsigned int) (unsigned long) arg;
  int i, j, repeat;
  unsigned int values[KEYS_COUNT];

  dbgprintf ("Worker %p started\n", gl_thread_self ());

  /* Initialize the per-thread storage.  */
  for (i = 0; i < KEYS_COUNT; i++)
    {
      values[i] = (((unsigned int) rand() >> 3) % 1000000) * THREAD_COUNT + id;
      /* Hopefully no arithmetic overflow.  */
      if ((values[i] % THREAD_COUNT) != id)
	abort ();
    }
  perhaps_yield ();

  /* Verify that the initial value is NULL.  */
  dbgprintf ("Worker %p before initial verify\n", gl_thread_self ());
  for (i = 0; i < KEYS_COUNT; i++)
    if (gl_tls_get (mykeys[i]) != NULL)
      abort ();
  dbgprintf ("Worker %p after  initial verify\n", gl_thread_self ());
  perhaps_yield ();

  /* Initialize the per-thread storage.  */
  dbgprintf ("Worker %p before first tls_set\n", gl_thread_self ());
  for (i = 0; i < KEYS_COUNT; i++)
    {
      unsigned int *ptr = (unsigned int *) malloc (sizeof (unsigned int));
      *ptr = values[i];
      gl_tls_set (mykeys[i], ptr);
    }
  dbgprintf ("Worker %p after  first tls_set\n", gl_thread_self ());
  perhaps_yield ();

  /* Shuffle around the pointers.  */
  for (repeat = REPEAT_COUNT; repeat > 0; repeat--)
    {
      dbgprintf ("Worker %p doing value swapping\n", gl_thread_self ());
      i = ((unsigned int) rand() >> 3) % KEYS_COUNT;
      j = ((unsigned int) rand() >> 3) % KEYS_COUNT;
      if (i != j)
	{
	  void *vi = gl_tls_get (mykeys[i]);
	  void *vj = gl_tls_get (mykeys[j]);

	  gl_tls_set (mykeys[i], vj);
	  gl_tls_set (mykeys[j], vi);
	}
      perhaps_yield ();
    }

  /* Verify that all the values are from this thread.  */
  dbgprintf ("Worker %p before final verify\n", gl_thread_self ());
  for (i = 0; i < KEYS_COUNT; i++)
    if ((*(unsigned int *) gl_tls_get (mykeys[i]) % THREAD_COUNT) != id)
      abort ();
  dbgprintf ("Worker %p after  final verify\n", gl_thread_self ());
  perhaps_yield ();

  dbgprintf ("Worker %p dying.\n", gl_thread_self ());
  return NULL;
}

void
test_tls (void)
{
  int pass, i;

  for (pass = 0; pass < 2; pass++)
    {
      gl_thread_t threads[THREAD_COUNT];

      if (pass == 0)
	for (i = 0; i < KEYS_COUNT; i++)
	  gl_tls_key_init (mykeys[i], free);
      else
	for (i = KEYS_COUNT - 1; i >= 0; i--)
	  gl_tls_key_init (mykeys[i], free);

      /* Spawn the threads.  */
      for (i = 0; i < THREAD_COUNT; i++)
	threads[i] = gl_thread_create (worker_thread, NULL);

      /* Wait for the threads to terminate.  */
      for (i = 0; i < THREAD_COUNT; i++)
	gl_thread_join (threads[i]);

      for (i = 0; i < KEYS_COUNT; i++)
	gl_tls_key_destroy (mykeys[i]);
    }
}

int
main ()
{
#if TEST_PTH_THREADS
  if (!pth_init ())
    abort ();
#endif

  printf ("Starting test_tls ..."); fflush (stdout);
  test_tls ();
  printf (" OK\n"); fflush (stdout);

  return 0;
}

#else

/* No multithreading available.  */

int
main ()
{
  return 77;
}

#endif
