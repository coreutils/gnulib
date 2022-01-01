/* Creating and controlling ISO C 11 threads.
   Copyright (C) 2005-2022 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2005, 2019.
   Based on GCC's gthr-posix.h, gthr-posix95.h, gthr-win32.h.  */

#include <config.h>

#include <threads.h>

#include <stdlib.h>

#if HAVE_THREADS_H
/* Provide workarounds.  */

# if BROKEN_THRD_START_T

#  undef thrd_t

/* AIX 7.1..7.2 defines thrd_start_t incorrectly, namely as
   'void * (*) (void *)' instead of 'int (*) (void *)'.
   As a consequence, its thrd_join function never stores an exit code.  */

/* The Thread-Specific Storage (TSS) key that allows to access each thread's
   'struct thrd_with_exitcode *' pointer.  */
static tss_t thrd_with_exitcode_key;

/* Initializes thrd_with_exitcode_key.
   This function must only be called once.  */
static void
do_init_thrd_with_exitcode_key (void)
{
  if (tss_create (&thrd_with_exitcode_key, NULL) != thrd_success)
    abort ();
}

/* Initializes thrd_with_exitcode_key.  */
static void
init_thrd_with_exitcode_key (void)
{
  static once_flag once = ONCE_FLAG_INIT;
  call_once (&once, do_init_thrd_with_exitcode_key);
}

typedef union
        {
          struct thrd_with_exitcode t;
          struct
          {
            thrd_t tid; /* reserve memory for t.tid */
            int detached; /* reserve memory for t.detached */
            thrd_start_t mainfunc;
            void *arg;
          } a;
        }
        main_arg_t;

static void *
thrd_main_func (void *pmarg)
{
  /* Unpack the object that combines mainfunc and arg.  */
  main_arg_t *main_arg = (main_arg_t *) pmarg;
  thrd_start_t mainfunc = main_arg->a.mainfunc;
  void *arg = main_arg->a.arg;

  if (tss_set (thrd_with_exitcode_key, &main_arg->t) != thrd_success)
    abort ();

  /* Execute mainfunc, with arg as argument.  */
  {
    int exitcode = mainfunc (arg);
    /* Store the exitcode, for use by thrd_join().  */
    main_arg->t.exitcode = exitcode;
    if (main_arg->t.detached)
      {
        /* Clean up the thread, like thrd_join would do.  */
        free (&main_arg->t);
      }
    return NULL;
  }
}

int
rpl_thrd_create (rpl_thrd_t *threadp, thrd_start_t mainfunc, void *arg)
#  undef thrd_create
{
  init_thrd_with_exitcode_key ();
  {
    /* Combine mainfunc and arg in a single object.
       A stack-allocated object does not work, because it would be out of
       existence when thrd_create returns before pthread_main_func is
       entered.  So, allocate it in the heap.  */
    main_arg_t *main_arg = (main_arg_t *) malloc (sizeof (main_arg_t));
    if (main_arg == NULL)
      return thrd_nomem;
    main_arg->a.mainfunc = mainfunc;
    main_arg->a.arg = arg;
    main_arg->t.detached = 0;
    {
      int err =
        thrd_create ((thrd_t *) &main_arg->t.tid, thrd_main_func, main_arg);
      if (err == thrd_success)
        *threadp = &main_arg->t;
      else
        free (main_arg);
      return err;
    }
  }
}

rpl_thrd_t
rpl_thrd_current (void)
#  undef thrd_current
{
  init_thrd_with_exitcode_key ();
  {
    rpl_thrd_t thread =
      (struct thrd_with_exitcode *) tss_get (thrd_with_exitcode_key);
    if (thread == NULL)
      {
        /* This happens only in threads that have not been created through
           thrd_create(), such as the main thread.  */
        for (;;)
          {
            thread =
              (struct thrd_with_exitcode *)
              malloc (sizeof (struct thrd_with_exitcode));
            if (thread != NULL)
              break;
            /* Memory allocation failed.  There is not much we can do.  Have to
               busy-loop, waiting for the availability of memory.  */
            {
              struct timespec ts;
              ts.tv_sec = 1;
              ts.tv_nsec = 0;
              thrd_sleep (&ts, NULL);
            }
          }
        thread->tid = thrd_current ();
        thread->detached = 0; /* This can lead to a memory leak.  */
        thread->exitcode = 0; /* just to be deterministic */
        if (tss_set (thrd_with_exitcode_key, thread) != thrd_success)
          abort ();
      }
    return thread;
  }
}

int
rpl_thrd_equal (rpl_thrd_t thread1, rpl_thrd_t thread2)
{
  return thread1 == thread2;
}

int
rpl_thrd_detach (rpl_thrd_t thread)
#  undef thrd_detach
{
  if (thread->detached)
    return thrd_error;
  {
    int err =
      thrd_detach (thread == rpl_thrd_current ()
                   ? /* thread->tid may not be initialized at this point.  */
                     thrd_current ()
                   : thread->tid);
    if (err == thrd_success)
      thread->detached = 1;
    return err;
  }
}

int
rpl_thrd_join (rpl_thrd_t thread, int *exitcodep)
#  undef thrd_join
{
  if (thread == rpl_thrd_current () || thread->detached)
    return thrd_error;
  {
    int err = thrd_join (thread->tid, NULL);
    if (err == thrd_success)
      {
        if (exitcodep != NULL)
          *exitcodep = thread->exitcode;
        free (thread);
      }
    return err;
  }
}

# endif

# if BROKEN_THRD_JOIN

/* On Solaris 11.4, thrd_join crashes when the second argument is NULL.  */
int
rpl_thrd_join (thrd_t thread, int *exitcodep)
#  undef thrd_join
{
  int exitcode;
  int err = thrd_join (thread, &exitcode);
  if (err == 0 && exitcodep != NULL)
    *exitcodep = exitcode;
  return err;
}

# endif

#else

# include <errno.h>
# include <stdint.h>

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

#  define WIN32_LEAN_AND_MEAN  /* avoid including junk */
#  include <windows.h>

# else
/* Use POSIX threads.  */

#  include <pthread.h>
#  include <sched.h>

# endif

/* The main functions passed to thrd_create and
   pthread_create/glwthread_thread_create have different return types:
   'int' vs. 'void *'.  */

struct pthread_main_arg_t
{
  thrd_start_t mainfunc;
  void *arg;
};

static void *
pthread_main_func (void *pmarg)
{
  /* Unpack the object that combines mainfunc and arg.  */
  struct pthread_main_arg_t *pthread_main_arg =
    (struct pthread_main_arg_t *) pmarg;
  thrd_start_t mainfunc = pthread_main_arg->mainfunc;
  void *arg = pthread_main_arg->arg;

  /* Free it.  */
  free (pmarg);

  /* Execute mainfunc, with arg as argument.  */
  {
    int exitcode = mainfunc (arg);
    /* Note: When using Windows threads, this exit code is different from the
       argument passed to ExitThread(), because the latter should never be 259,
       see <https://docs.microsoft.com/de-de/windows/desktop/api/processthreadsapi/nf-processthreadsapi-getexitcodethread>,
       whereas the exit code passed to thrd_exit() is not constrained.  */
    return (void *) (intptr_t) exitcode;
  }
}

# if defined _WIN32 && ! defined __CYGWIN__
/* Use Windows threads.  */

int
thrd_create (thrd_t *threadp, thrd_start_t mainfunc, void *arg)
{
  /* Combine mainfunc and arg in a single object.
     A stack-allocated object does not work, because it would be out of
     existence when thrd_create returns before pthread_main_func is
     entered.  So, allocate it in the heap.  */
  struct pthread_main_arg_t *pthread_main_arg =
    (struct pthread_main_arg_t *) malloc (sizeof (struct pthread_main_arg_t));
  if (pthread_main_arg == NULL)
    return thrd_nomem;
  pthread_main_arg->mainfunc = mainfunc;
  pthread_main_arg->arg = arg;

  {
    int err = glwthread_thread_create (threadp, 0,
                                       pthread_main_func, pthread_main_arg);
    if (err != 0)
      free (pthread_main_arg);
    return (err == 0 ? thrd_success :
            err == ENOMEM /* || err == EAGAIN */ ? thrd_nomem :
            thrd_error);
  }
}

thrd_t
thrd_current (void)
{
  return glwthread_thread_self ();
}

int
thrd_equal (thrd_t thread1, thrd_t thread2)
{
  return thread1 == thread2;
}

void
thrd_yield (void)
{
  Sleep (0);
}

int
thrd_detach (thrd_t thread)
{
  int err = glwthread_thread_detach (thread);
  return (err == 0 ? thrd_success : thrd_error);
}

int
thrd_join (thrd_t thread, int *exitcodep)
{
  void *exitptr;
  int err = glwthread_thread_join (thread, &exitptr);
  if (err == 0)
    {
      if (exitcodep != NULL)
        *exitcodep = (int) (intptr_t) exitptr;
      return thrd_success;
    }
  else
    return thrd_error;
}

_Noreturn void
thrd_exit (int exitcode)
{
  glwthread_thread_exit ((void *) (intptr_t) exitcode);
}

# else
/* Use POSIX threads.  */

int
thrd_create (thrd_t *threadp, thrd_start_t mainfunc, void *arg)
{
  /* Combine mainfunc and arg in a single object.
     A stack-allocated object does not work, because it would be out of
     existence when thrd_create returns before pthread_main_func is
     entered.  So, allocate it in the heap.  */
  struct pthread_main_arg_t *pthread_main_arg =
    (struct pthread_main_arg_t *) malloc (sizeof (struct pthread_main_arg_t));
  if (pthread_main_arg == NULL)
    return thrd_nomem;
  pthread_main_arg->mainfunc = mainfunc;
  pthread_main_arg->arg = arg;

  {
    int err = pthread_create (threadp, NULL,
                              pthread_main_func, pthread_main_arg);
    if (err != 0)
      free (pthread_main_arg);
    return (err == 0 ? thrd_success :
            err == ENOMEM /* || err == EAGAIN */ ? thrd_nomem :
            thrd_error);
  }
}

thrd_t
thrd_current (void)
{
  return pthread_self ();
}

int
thrd_equal (thrd_t thread1, thrd_t thread2)
{
  return pthread_equal (thread1, thread2);
}

void
thrd_yield (void)
{
  sched_yield ();
}

int
thrd_detach (thrd_t thread)
{
  int err = pthread_detach (thread);
  return (err == 0 ? thrd_success : thrd_error);
}

int
thrd_join (thrd_t thread, int *exitcodep)
{
  void *exitptr;
  int err = pthread_join (thread, &exitptr);
  if (err == 0)
    {
      if (exitcodep != NULL)
        *exitcodep = (int) (intptr_t) exitptr;
      return thrd_success;
    }
  else
    return thrd_error;
}

_Noreturn void
thrd_exit (int exitcode)
{
  pthread_exit ((void *) (intptr_t) exitcode);
}

# endif

int
thrd_sleep (const struct timespec *duration, struct timespec *remaining)
{
  int ret = nanosleep (duration, remaining);
  return (ret == 0 ? 0 : errno == EINTR ? -1 : -2);
}

#endif
