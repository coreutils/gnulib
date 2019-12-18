/* Return the internal lock used by setlocale_null_r.
   Copyright (C) 2019 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2019.  */

#include <config.h>

/* This file defines the internal lock used by setlocale_null_r.
   It is a separate compilation unit, so that only one copy of it is
   present when linking statically.  */

/* Prohibit renaming this symbol.  */
#undef gl_get_setlocale_null_lock

#if defined _WIN32 && !defined __CYGWIN__

# define WIN32_LEAN_AND_MEAN  /* avoid including junk */
# include <windows.h>

# include "windows-initguard.h"

/* The return type is a 'CRITICAL_SECTION *', not a 'glwthread_mutex_t *',
   because the latter is not guaranteed to be a stable ABI in the future.  */

/* Make sure the function gets exported from DLLs.  */
__declspec(dllexport) CRITICAL_SECTION *gl_get_setlocale_null_lock (void);

static glwthread_initguard_t guard = GLWTHREAD_INITGUARD_INIT;
static CRITICAL_SECTION lock;

/* Returns the internal lock used by setlocale_null_r.  */
CRITICAL_SECTION *
gl_get_setlocale_null_lock (void)
{
  if (!guard.done)
    {
      if (InterlockedIncrement (&guard.started) == 0)
        {
          /* This thread is the first one to need the lock.  Initialize it.  */
          InitializeCriticalSection (&lock);
          guard.done = 1;
        }
      else
        {
          /* Don't let guard.started grow and wrap around.  */
          InterlockedDecrement (&guard.started);
          /* Yield the CPU while waiting for another thread to finish
             initializing this mutex.  */
          while (!guard.done)
            Sleep (0);
        }
    }
  return &lock;
}

#elif HAVE_PTHREAD_API

# include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

# if defined _WIN32 || defined __CYGWIN__
/* Make sure the function gets exported from DLLs.  */
__declspec(dllexport) pthread_mutex_t *gl_get_setlocale_null_lock (void);
# endif

/* Returns the internal lock used by setlocale_null_r.  */
pthread_mutex_t *
gl_get_setlocale_null_lock (void)
{
  return &mutex;
}

#elif HAVE_THREADS_H

# include <threads.h>
# include <stdlib.h>

static int volatile init_needed = 1;
static once_flag init_once = ONCE_FLAG_INIT;
static mtx_t mutex;

static void
atomic_init (void)
{
  if (mtx_init (&mutex, mtx_plain) != thrd_success)
    abort ();
  init_needed = 0;
}

/* Returns the internal lock used by setlocale_null_r.  */
mtx_t *
gl_get_setlocale_null_lock (void)
{
  if (init_needed)
    call_once (&init_once, atomic_init);
  return &mutex;
}

#endif

#if defined _WIN32 || defined __CYGWIN__
/* Make sure the '__declspec(dllimport)' in setlocale_null.c does not cause
   a link failure when no DLLs are involved.  */
# if defined _WIN64 || defined _LP64
#  define IMP(x) __imp_##x
# else
#  define IMP(x) _imp__##x
# endif
void * IMP(gl_get_setlocale_null_lock) = &gl_get_setlocale_null_lock;
#endif
