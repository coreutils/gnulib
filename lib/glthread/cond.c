/* Condition variables for multithreading.
   Copyright (C) 2008 Free Software Foundation, Inc.

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

/* Written by Yoann Vandoorselaere <yoann@prelude-ids.org>, 2008,
   and Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#include "glthread/cond.h"

/* ========================================================================= */

#if USE_PTH_THREADS

/* -------------------------- gl_cond_t datatype -------------------------- */

int
glthread_cond_timedwait_multithreaded (gl_cond_t *cond,
				       gl_lock_t *lock,
				       struct timespec *abstime)
{
  int ret, status;
  pth_event_t ev;

  ev = pth_event (PTH_EVENT_TIME, pth_time (abstime->tv_sec, abstime->tv_nsec / 1000));
  ret = pth_cond_await (cond, lock, ev);

  status = pth_event_status (ev);
  pth_event_free (ev, PTH_FREE_THIS);

  if (status == PTH_STATUS_OCCURRED)
    return ETIMEDOUT;

  return ret;
}

#endif

/* ========================================================================= */

#if USE_SOLARIS_THREADS

/* -------------------------- gl_cond_t datatype -------------------------- */

int
glthread_cond_timedwait_multithreaded (gl_cond_t *cond,
				       gl_lock_t *lock,
				       struct timespec *abstime)
{
  int ret;

  ret = cond_timedwait (cond, lock, abstime);
  if (ret == ETIME)
    return ETIMEDOUT;
  return ret;
}

#endif

/* ========================================================================= */

#if USE_WIN32_THREADS

#include <sys/time.h>

/* -------------------------- gl_cond_t datatype -------------------------- */

/* This implementation is based on the article
   Douglas C. Schmidt, Irfan Pyarali
   "Strategies for Implementing POSIX Condition Variables on Win32"
   <http://www.cs.wustl.edu/~schmidt/win32-cv-1.html>  */

int
glthread_cond_init_func (gl_cond_t *cond)
{
  InitializeCriticalSection (&cond->lock);
  /* Create a manual-reset event.  */
  cond->event = CreateEvent (NULL, TRUE, FALSE, NULL);
  cond->waiters_count = 0;
  cond->release_count = 0;
  cond->wait_generation_count = 0;

  cond->guard.done = 1;

  return 0;
}

int
glthread_cond_wait_func (gl_cond_t *cond, gl_lock_t *lock)
{
  if (!cond->guard.done)
    {
      if (InterlockedIncrement (&cond->guard.started) == 0)
	/* This thread is the first one to need this condition variable.
	   Initialize it.  */
	glthread_cond_init (cond);
      else
	/* Yield the CPU while waiting for another thread to finish
	   initializing this condition variable.  */
	while (!cond->guard.done)
	  Sleep (0);
    }

  {
    unsigned old_generation_count;
    HANDLE event;

    EnterCriticalSection (&cond->lock);
    /* Increment waiters_count,
       and get a copy of the current wait_generation_count.  */
    cond->waiters_count++;
    old_generation_count = cond->wait_generation_count;
    event = cond->event;
    LeaveCriticalSection (&cond->lock);

    {
      /* Now release the lock and let any other thread take it.  */
      int err = glthread_lock_unlock (lock);
      if (err != 0)
	{
	  EnterCriticalSection (&cond->lock);
	  cond->waiters_count--;
	  LeaveCriticalSection (&cond->lock);
	  return err;
	}

      {
	/* Wait until another thread signals this event.  */
	DWORD result;

	for (;;)
	  {
	    bool wait_done;

	    result = WaitForSingleObject (event, INFINITE);
	    if (result != WAIT_OBJECT_0)
	      break;
	    /* Distingish intended from spurious wakeups.  */
	    EnterCriticalSection (&cond->lock);
	    wait_done =
	      (cond->release_count > 0
	       && cond->wait_generation_count != old_generation_count);
	    LeaveCriticalSection (&cond->lock);
	    if (wait_done)
	      break;
	  }

	/* Take the lock again.  It does not matter whether this is done
	   before or after the bookkeeping.  */
	err = glthread_lock_lock (lock);

	/* Do the bookkeeping.  */
	EnterCriticalSection (&cond->lock);
	cond->waiters_count--;
	if (result == WAIT_OBJECT_0)
	  {
	    /* The wait terminated because the event was signaled.
	       Acknowledge the receipt.  */
	    if (--cond->release_count == 0)
	      {
		/* The last waiting thread to be notified has to reset
		   the event.  */
		ResetEvent (cond->event);
	      }
	  }
	LeaveCriticalSection (&cond->lock);

	return (err ? err :
		result == WAIT_OBJECT_0 ? 0 :
		/* WAIT_FAILED shouldn't happen */ EAGAIN);
      }
    }
  }
}

int
glthread_cond_timedwait_func (gl_cond_t *cond, gl_lock_t *lock, struct timespec *abstime)
{
  struct timeval currtime;

  gettimeofday (&currtime, NULL);
  if (currtime.tv_sec > abstime->tv_sec
      || (currtime.tv_sec == abstime->tv_sec
	  && currtime.tv_usec * 1000 >= abstime->tv_nsec))
    return ETIMEDOUT;

  if (!cond->guard.done)
    {
      if (InterlockedIncrement (&cond->guard.started) == 0)
	/* This thread is the first one to need this condition variable.
	   Initialize it.  */
	glthread_cond_init (cond);
      else
	/* Yield the CPU while waiting for another thread to finish
	   initializing this condition variable.  */
	while (!cond->guard.done)
	  Sleep (0);
    }

  {
    unsigned old_generation_count;
    HANDLE event;

    EnterCriticalSection (&cond->lock);
    /* Increment waiters_count,
       and get a copy of the current wait_generation_count.  */
    cond->waiters_count++;
    old_generation_count = cond->wait_generation_count;
    event = cond->event;
    LeaveCriticalSection (&cond->lock);

    {
      /* Now release the lock and let any other thread take it.  */
      int err = glthread_lock_unlock (lock);
      if (err != 0)
	{
	  EnterCriticalSection (&cond->lock);
	  cond->waiters_count--;
	  LeaveCriticalSection (&cond->lock);
	  return err;
	}

      {
	/* Wait until another thread signals this event or until the abstime
	   passes.  */
	DWORD result;

	for (;;)
	  {
	    DWORD timeout;
	    bool wait_done;

	    gettimeofday (&currtime, NULL);
	    if (currtime.tv_sec > abstime->tv_sec)
	      timeout = 0;
	    else
	      {
		unsigned long seconds = abstime->tv_sec - currtime.tv_sec;
		timeout = seconds * 1000;
		if (timeout / 1000 != seconds) /* overflow? */
		  timeout = INFINITE;
		else
		  {
		    long milliseconds =
		      abstime->tv_nsec / 1000000 - currtime.tv_usec / 1000;
		    if (milliseconds >= 0)
		      {
			timeout += milliseconds;
			if (timeout < milliseconds) /* overflow? */
			  timeout = INFINITE;
		      }
		    else
		      {
			if (timeout >= - milliseconds)
			  timeout -= (- milliseconds);
			else
			  timeout = 0;
		      }
		  }
	      }

	    result = WaitForSingleObject (event, timeout);
	    if (result != WAIT_OBJECT_0)
	      break;
	    /* Distingish intended from spurious wakeups.  */
	    EnterCriticalSection (&cond->lock);
	    wait_done =
	      (cond->release_count > 0
	       && cond->wait_generation_count != old_generation_count);
	    LeaveCriticalSection (&cond->lock);
	    if (wait_done)
	      break;
	  }

	/* Take the lock again.  It does not matter whether this is done
	   before or after the bookkeeping.  */
	err = glthread_lock_lock (lock);

	/* Do the bookkeeping.  */
	EnterCriticalSection (&cond->lock);
	cond->waiters_count--;
	if (result == WAIT_OBJECT_0)
	  {
	    /* The wait terminated because the event was signaled.
	       Acknowledge the receipt.  */
	    if (--cond->release_count == 0)
	      {
		/* The last waiting thread to be notified has to reset
		   the event.  */
		ResetEvent (cond->event);
	      }
	  }
	LeaveCriticalSection (&cond->lock);

	return (err ? err :
		result == WAIT_OBJECT_0 ? 0 :
		result == WAIT_TIMEOUT ? ETIMEDOUT :
		/* WAIT_FAILED shouldn't happen */ EAGAIN);
      }
    }
  }
}

int
glthread_cond_signal_func (gl_cond_t *cond)
{
  if (!cond->guard.done)
    return EINVAL;

  EnterCriticalSection (&cond->lock);
  /* POSIX says:
      "The pthread_cond_broadcast() and pthread_cond_signal() functions shall
       have no effect if there are no threads currently blocked on cond."
     Also, if  waiters_count == release_count > 0, all blocked threads will
     be unblocked soon anyway; do nothing in this case as well.  */
  if (cond->waiters_count > cond->release_count)
    {
      /* Signal the manual-reset event.  */
      SetEvent (cond->event);
      /* ... and reset it is soon as one of the currently waiting threads has
	 acknowledged the receipt of the signal.  */
      cond->release_count++;
      cond->wait_generation_count++;
    }
  LeaveCriticalSection (&cond->lock);

  return 0;
}

int
glthread_cond_broadcast_func (gl_cond_t *cond)
{
  if (!cond->guard.done)
    return EINVAL;

  EnterCriticalSection (&cond->lock);
  /* POSIX says:
      "The pthread_cond_broadcast() and pthread_cond_signal() functions shall
       have no effect if there are no threads currently blocked on cond."  */
  if (cond->waiters_count > 0)
    {
      /* Signal the manual-reset event.  */
      SetEvent (cond->event);
      /* ... and reset it only after all currently waiting threads have
	 acknowledged the receipt of the signal.  */
      cond->release_count = cond->waiters_count;
      cond->wait_generation_count++;
    }
  LeaveCriticalSection (&cond->lock);

  return 0;
}

int
glthread_cond_destroy_func (gl_cond_t *cond)
{
  if (!cond->guard.done)
    return EINVAL;
  if (cond->waiters_count > 0)
    return EBUSY;
  CloseHandle (cond->event);
  DeleteCriticalSection (&cond->lock);
  cond->guard.done = 0;
  return 0;
}

#endif

/* ========================================================================= */
