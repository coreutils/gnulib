/* Creating and controlling threads.
   Copyright (C) 2005-2008 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2005.
   Based on GCC's gthr-posix.h, gthr-posix95.h, gthr-solaris.h,
   gthr-win32.h.  */

#include <config.h>

#include "glthread/thread.h"

#include <stdlib.h>
#include "glthread/lock.h"

/* ========================================================================= */

#if USE_WIN32_THREADS

/* -------------------------- gl_thread_t datatype -------------------------- */

/* Use a wrapper function, instead of adding WINAPI through a cast.
   This struct also holds the thread's exit value.  */
struct thread_extra
  {
    /* Fields for managing the association between thread id and handle.  */
    DWORD volatile id;
    HANDLE volatile handle;
    struct thread_extra * volatile next;
    /* Fields for managing the exit value.  */
    void * volatile result;
    /* Fields for managing the thread start.  */
    void * (*func) (void *);
    void *arg;
  };

/* Linked list of thread_extra of running or zombie (not-yet-joined)
   threads.
   TODO: Use a hash table indexed by id instead of a linked list.  */
static struct thread_extra *running_threads /* = NULL */;

/* Lock protecting running_threads.  */
gl_lock_define_initialized(static, running_lock)

static DWORD WINAPI
wrapper_func (void *varg)
{
  struct thread_extra *xarg = (struct thread_extra *)varg;

  xarg->id = GetCurrentThreadId ();
  /* Add xarg to the list of running thread_extra.  */
  gl_lock_lock (running_lock);
  if (!(xarg->id == GetCurrentThreadId ()))
    abort ();
  xarg->next = running_threads;
  running_threads = xarg;
  gl_lock_unlock (running_lock);

  /* Run the thread.  Store the exit value if the thread was not terminated
     otherwise.  */
  xarg->result = xarg->func (xarg->arg);
  return 0;
}

int
glthread_create_func (gl_thread_t *threadp, void * (*func) (void *), void *arg)
{
  struct thread_extra *x =
    (struct thread_extra *) malloc (sizeof (struct thread_extra));
  if (x == NULL)
    return ENOMEM;
  x->result = NULL; /* just to be deterministic */
  x->func = func;
  x->arg = arg;
  {
    DWORD thread_id;
    HANDLE thread_handle;

    gl_lock_lock (running_lock);
    thread_handle = CreateThread (NULL, 100000, wrapper_func, x, 0, &thread_id);
    if (thread_handle == NULL)
      {
	gl_lock_unlock (running_lock);
	return EAGAIN;
      }
    x->id = thread_id;
    x->handle = thread_handle;
    gl_lock_unlock (running_lock);
    *threadp = thread_id;
    return 0;
  }
}

int
glthread_join_func (gl_thread_t thread, void **retvalp)
{
  HANDLE thread_handle;

  if (thread == gl_thread_self ())
    return EDEADLK;

  /* Find the thread handle that corresponds to the thread id.
     The thread argument must come from either the parent thread or from the
     thread itself.  So at this point, either glthread_create_func was
     completed (and x->handle set), or x->func was invoked (and that can
     only be after the running_lock was acquired, hence after
     glthread_create_func released it, hence x->handle is set as well).  */
  thread_handle = NULL;
  gl_lock_lock (running_lock);
  {
    struct thread_extra *x;
    for (x = running_threads; x != NULL; x = x->next)
      if (x->id == thread)
	{
	  thread_handle = x->handle;
	  break;
	}
  }
  gl_lock_unlock (running_lock);
  if (thread_handle == NULL)
    return ESRCH;

  if (WaitForSingleObject (thread_handle, INFINITE) == WAIT_FAILED)
    return EINVAL;

  /* Remove the 'struct thread_extra' from running_threads.  */
  gl_lock_lock (running_lock);
  {
    struct thread_extra **xp;
    for (xp = &running_threads; *xp != NULL; xp = &(*xp)->next)
      if ((*xp)->id == thread)
	{
	  struct thread_extra *x = *xp;
	  if (retvalp != NULL)
	    *retvalp = x->result;
	  if (x->handle != thread_handle)
	    abort ();
	  *xp = x->next;
	  free (x);
	  break;
	}
  }
  gl_lock_unlock (running_lock);

  CloseHandle (thread_handle);

  return 0;
}

int
gl_thread_exit_func (void *retval)
{
  DWORD this_thread = GetCurrentThreadId ();

  /* Store the exit value in the appropriate element of running_threads.  */
  gl_lock_lock (running_lock);
  {
    struct thread_extra *x;
    for (x = running_threads; x != NULL; x = x->next)
      if (x->id == this_thread)
	{
	  x->result = retval;
	  break;
	}
  }
  gl_lock_unlock (running_lock);

  ExitThread (0);
}

#endif

/* ========================================================================= */
