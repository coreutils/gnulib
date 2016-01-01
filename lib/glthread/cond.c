/* Condition variables for multithreading.
   Copyright (C) 2008-2016 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

/* Written by Yoann Vandoorselaere <yoann@prelude-ids.org>, 2008,
   and Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

#define _GLTHREAD_COND_INLINE _GL_EXTERN_INLINE
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

#if USE_WINDOWS_THREADS

#include <sys/time.h>

/* -------------------------- gl_cond_t datatype -------------------------- */

/* In this file, the waitqueues are implemented as linked lists.  */
#define gl_waitqueue_t gl_linked_waitqueue_t

/* All links of a circular list, except the anchor, are of this type, carrying
   a payload.  */
struct gl_waitqueue_element
{
  struct gl_waitqueue_link link; /* must be the first field! */
  HANDLE event; /* Waiting thread, represented by an event.
                   This field is immutable once initialized. */
};

static void
gl_waitqueue_init (gl_waitqueue_t *wq)
{
  wq->wq_list.wql_next = &wq->wq_list;
  wq->wq_list.wql_prev = &wq->wq_list;
}

/* Enqueues the current thread, represented by an event, in a wait queue.
   Returns NULL if an allocation failure occurs.  */
static struct gl_waitqueue_element *
gl_waitqueue_add (gl_waitqueue_t *wq)
{
  struct gl_waitqueue_element *elt;
  HANDLE event;

  /* Allocate the memory for the waitqueue element on the heap, not on the
     thread's stack.  If the thread exits unexpectedly, we prefer to leak
     some memory rather than to access unavailable memory and crash.  */
  elt =
    (struct gl_waitqueue_element *)
    malloc (sizeof (struct gl_waitqueue_element));
  if (elt == NULL)
    /* No more memory.  */
    return NULL;

  /* Whether the created event is a manual-reset one or an auto-reset one,
     does not matter, since we will wait on it only once.  */
  event = CreateEvent (NULL, TRUE, FALSE, NULL);
  if (event == INVALID_HANDLE_VALUE)
    {
      /* No way to allocate an event.  */
      free (elt);
      return NULL;
    }
  elt->event = event;
  /* Insert elt at the end of the circular list.  */
  (elt->link.wql_prev = wq->wq_list.wql_prev)->wql_next = &elt->link;
  (elt->link.wql_next = &wq->wq_list)->wql_prev = &elt->link;
  return elt;
}

/* Removes the current thread, represented by a 'struct gl_waitqueue_element *',
   from a wait queue.
   Returns true if is was found and removed, false if it was not present.  */
static bool
gl_waitqueue_remove (gl_waitqueue_t *wq, struct gl_waitqueue_element *elt)
{
  if (elt->link.wql_next != NULL && elt->link.wql_prev != NULL)
    {
      /* Remove elt from the circular list.  */
      struct gl_waitqueue_link *prev = elt->link.wql_prev;
      struct gl_waitqueue_link *next = elt->link.wql_next;
      prev->wql_next = next;
      next->wql_prev = prev;
      elt->link.wql_next = NULL;
      elt->link.wql_prev = NULL;
      return true;
    }
  else
    return false;
}

/* Notifies the first thread from a wait queue and dequeues it.  */
static void
gl_waitqueue_notify_first (gl_waitqueue_t *wq)
{
  if (wq->wq_list.wql_next != &wq->wq_list)
    {
      struct gl_waitqueue_element *elt =
        (struct gl_waitqueue_element *) wq->wq_list.wql_next;
      struct gl_waitqueue_link *prev;
      struct gl_waitqueue_link *next;

      /* Remove elt from the circular list.  */
      prev = &wq->wq_list; /* = elt->link.wql_prev; */
      next = elt->link.wql_next;
      prev->wql_next = next;
      next->wql_prev = prev;
      elt->link.wql_next = NULL;
      elt->link.wql_prev = NULL;

      SetEvent (elt->event);
      /* After the SetEvent, this thread cannot access *elt any more, because
         the woken-up thread will quickly call  free (elt).  */
    }
}

/* Notifies all threads from a wait queue and dequeues them all.  */
static void
gl_waitqueue_notify_all (gl_waitqueue_t *wq)
{
  struct gl_waitqueue_link *l;

  for (l = wq->wq_list.wql_next; l != &wq->wq_list; )
    {
      struct gl_waitqueue_element *elt = (struct gl_waitqueue_element *) l;
      struct gl_waitqueue_link *prev;
      struct gl_waitqueue_link *next;

      /* Remove elt from the circular list.  */
      prev = &wq->wq_list; /* = elt->link.wql_prev; */
      next = elt->link.wql_next;
      prev->wql_next = next;
      next->wql_prev = prev;
      elt->link.wql_next = NULL;
      elt->link.wql_prev = NULL;

      SetEvent (elt->event);
      /* After the SetEvent, this thread cannot access *elt any more, because
         the woken-up thread will quickly call  free (elt).  */

      l = next;
    }
  if (!(wq->wq_list.wql_next == &wq->wq_list
        && wq->wq_list.wql_prev == &wq->wq_list))
    abort ();
}

int
glthread_cond_init_func (gl_cond_t *cond)
{
  InitializeCriticalSection (&cond->lock);
  gl_waitqueue_init (&cond->waiters);

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

  EnterCriticalSection (&cond->lock);
  {
    struct gl_waitqueue_element *elt = gl_waitqueue_add (&cond->waiters);
    LeaveCriticalSection (&cond->lock);
    if (elt == NULL)
      {
        /* Allocation failure.  Weird.  */
        return EAGAIN;
      }
    else
      {
        HANDLE event = elt->event;
        int err;
        DWORD result;

        /* Now release the lock and let any other thread take it.  */
        err = glthread_lock_unlock (lock);
        if (err != 0)
          {
            EnterCriticalSection (&cond->lock);
            gl_waitqueue_remove (&cond->waiters, elt);
            LeaveCriticalSection (&cond->lock);
            CloseHandle (event);
            free (elt);
            return err;
          }
        /* POSIX says:
            "If another thread is able to acquire the mutex after the
             about-to-block thread has released it, then a subsequent call to
             pthread_cond_broadcast() or pthread_cond_signal() in that thread
             shall behave as if it were issued after the about-to-block thread
             has blocked."
           This is fulfilled here, because the thread signalling is done
           through SetEvent, not PulseEvent.  */
        /* Wait until another thread signals this event.  */
        result = WaitForSingleObject (event, INFINITE);
        if (result == WAIT_FAILED || result == WAIT_TIMEOUT)
          abort ();
        CloseHandle (event);
        free (elt);
        /* The thread which signalled the event already did the bookkeeping:
           removed us from the waiters.  */
        return glthread_lock_lock (lock);
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

  EnterCriticalSection (&cond->lock);
  {
    struct gl_waitqueue_element *elt = gl_waitqueue_add (&cond->waiters);
    LeaveCriticalSection (&cond->lock);
    if (elt == NULL)
      {
        /* Allocation failure.  Weird.  */
        return EAGAIN;
      }
    else
      {
        HANDLE event = elt->event;
        int err;
        DWORD timeout;
        DWORD result;

        /* Now release the lock and let any other thread take it.  */
        err = glthread_lock_unlock (lock);
        if (err != 0)
          {
            EnterCriticalSection (&cond->lock);
            gl_waitqueue_remove (&cond->waiters, elt);
            LeaveCriticalSection (&cond->lock);
            CloseHandle (event);
            free (elt);
            return err;
          }
        /* POSIX says:
            "If another thread is able to acquire the mutex after the
             about-to-block thread has released it, then a subsequent call to
             pthread_cond_broadcast() or pthread_cond_signal() in that thread
             shall behave as if it were issued after the about-to-block thread
             has blocked."
           This is fulfilled here, because the thread signalling is done
           through SetEvent, not PulseEvent.  */
        /* Wait until another thread signals this event or until the abstime
           passes.  */
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
        if (result == WAIT_FAILED)
          abort ();
        if (result == WAIT_TIMEOUT)
          {
            EnterCriticalSection (&cond->lock);
            if (gl_waitqueue_remove (&cond->waiters, elt))
              {
                /* The event was not signaled between the WaitForSingleObject
                   call and the EnterCriticalSection call.  */
                if (!(WaitForSingleObject (event, 0) == WAIT_TIMEOUT))
                  abort ();
              }
            else
              {
                /* The event was signaled between the WaitForSingleObject
                   call and the EnterCriticalSection call.  */
                if (!(WaitForSingleObject (event, 0) == WAIT_OBJECT_0))
                  abort ();
                /* Produce the right return value.  */
                result = WAIT_OBJECT_0;
              }
            LeaveCriticalSection (&cond->lock);
          }
        else
          {
            /* The thread which signalled the event already did the
               bookkeeping: removed us from the waiters.  */
          }
        CloseHandle (event);
        free (elt);
        /* Take the lock again.  It does not matter whether this is done
           before or after the bookkeeping for WAIT_TIMEOUT.  */
        err = glthread_lock_lock (lock);
        return (err ? err :
                result == WAIT_OBJECT_0 ? 0 :
                result == WAIT_TIMEOUT ? ETIMEDOUT :
                /* WAIT_FAILED shouldn't happen */ EAGAIN);
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
       have no effect if there are no threads currently blocked on cond."  */
  if (cond->waiters.wq_list.wql_next != &cond->waiters.wq_list)
    gl_waitqueue_notify_first (&cond->waiters);
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
       have no effect if there are no threads currently blocked on cond."
     gl_waitqueue_notify_all is a nop in this case.  */
  gl_waitqueue_notify_all (&cond->waiters);
  LeaveCriticalSection (&cond->lock);

  return 0;
}

int
glthread_cond_destroy_func (gl_cond_t *cond)
{
  if (!cond->guard.done)
    return EINVAL;
  if (cond->waiters.wq_list.wql_next != &cond->waiters.wq_list)
    return EBUSY;
  DeleteCriticalSection (&cond->lock);
  cond->guard.done = 0;
  return 0;
}

#endif

/* ========================================================================= */
