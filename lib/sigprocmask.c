/* POSIX compatible signal blocking.
   Copyright (C) 2006-2026 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2006.

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

#include <config.h>

/* Specification.  */
#include <signal.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#if HAVE_MSVC_INVALID_PARAMETER_HANDLER
# include "msvc-inval.h"
#endif

#if GNULIB_SIGPROCMASK_SINGLE_THREAD
# define glwthread_spin_lock(lock)
# define glwthread_spin_unlock(lock)
#else
# include "windows-spin.h"
#endif

/* We assume that a platform without POSIX signal blocking functions
   also does not have the POSIX sigaction() function, only the
   signal() function.  We also assume signal() has SysV semantics,
   where any handler is uninstalled prior to being invoked.  This is
   true for native Windows platforms.  Documentation:
   <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/signal>  */

/* We use raw signal(), but also provide a wrapper rpl_signal() so
   that applications can query or change a blocked signal.  */
#undef signal

/* Provide invalid signal numbers as fallbacks if the uncatchable
   signals are not defined.  */
#ifndef SIGKILL
# define SIGKILL (-1)
#endif
#ifndef SIGSTOP
# define SIGSTOP (-1)
#endif

/* On native Windows, as of 2008, the signal SIGABRT_COMPAT is an alias
   for the signal SIGABRT.  Only one signal handler is stored for both
   SIGABRT and SIGABRT_COMPAT.  SIGABRT_COMPAT is not a signal of its own.  */
#if defined _WIN32 && ! defined __CYGWIN__
# undef SIGABRT_COMPAT
# define SIGABRT_COMPAT 6
#endif
#ifdef SIGABRT_COMPAT
# define SIGABRT_COMPAT_MASK (1U << SIGABRT_COMPAT)
#else
# define SIGABRT_COMPAT_MASK 0
#endif

typedef void (*handler_t) (int);

#if HAVE_MSVC_INVALID_PARAMETER_HANDLER
static handler_t
signal_nothrow (int sig, handler_t handler)
{
  handler_t result;

  TRY_MSVC_INVAL
    {
      result = signal (sig, handler);
    }
  CATCH_MSVC_INVAL
    {
      result = SIG_ERR;
      errno = EINVAL;
    }
  DONE_MSVC_INVAL;

  return result;
}
# define signal signal_nothrow
#endif

/* Handling of gnulib defined signals.  */

#if GNULIB_defined_SIGPIPE
static handler_t SIGPIPE_handler = SIG_DFL;
#endif

#if GNULIB_defined_SIGPIPE
static handler_t
ext_signal (int sig, handler_t handler)
{
  switch (sig)
    {
    case SIGPIPE:
      {
        handler_t old_handler = SIGPIPE_handler;
        SIGPIPE_handler = handler;
        return old_handler;
      }
    default: /* System defined signal */
      return signal (sig, handler);
    }
}
# undef signal
# define signal ext_signal
#endif

int
sigismember (const sigset_t *set, int sig)
{
  if (sig >= 0 && sig < NSIG)
    {
      #ifdef SIGABRT_COMPAT
      if (sig == SIGABRT_COMPAT)
        sig = SIGABRT;
      #endif

      return (*set >> sig) & 1;
    }
  else
    return 0;
}

int
sigemptyset (sigset_t *set)
{
  *set = 0;
  return 0;
}

int
sigaddset (sigset_t *set, int sig)
{
  if (sig >= 0 && sig < NSIG)
    {
      #ifdef SIGABRT_COMPAT
      if (sig == SIGABRT_COMPAT)
        sig = SIGABRT;
      #endif

      *set |= 1U << sig;
      return 0;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}

int
sigdelset (sigset_t *set, int sig)
{
  if (sig >= 0 && sig < NSIG)
    {
      #ifdef SIGABRT_COMPAT
      if (sig == SIGABRT_COMPAT)
        sig = SIGABRT;
      #endif

      *set &= ~(1U << sig);
      return 0;
    }
  else
    {
      errno = EINVAL;
      return -1;
    }
}


int
sigfillset (sigset_t *set)
{
  *set = ((2U << (NSIG - 1)) - 1) & ~ SIGABRT_COMPAT_MASK;
  return 0;
}

/* Storage class specifier for thread-local storage.  */
#undef thread_local
#if defined _MSC_VER
/* <https://learn.microsoft.com/en-us/cpp/parallel/thread-local-storage-tls> */
# define thread_local __declspec(thread)
#else
/* <https://gcc.gnu.org/onlinedocs/gcc/Thread-Local.html> */
# define thread_local __thread
#endif

/* Set of currently blocked signals.  */
static thread_local sigset_t blocked_set /* = 0 */;

/* Set of currently blocked and pending signals.  */
static thread_local volatile sig_atomic_t pending_array[NSIG] /* = { 0 } */;

int
sigpending (sigset_t *set)
{
  sigset_t pending = 0;
  for (int sig = 0; sig < NSIG; sig++)
    if (pending_array[sig])
      pending |= 1U << sig;

  *set = pending;
  return 0;
}

/* A registry which signal handlers are overridden.  */
struct override
{
  /* True if the signal is or was blocked in some thread.  */
  volatile int overridden;
  /* The original signal handler, if overridden.  */
  volatile handler_t original_handler;
};
static struct override overrides[NSIG] /* = { { 0, NULL }, ... } */;

#if !GNULIB_SIGPROCMASK_SINGLE_THREAD
/* A spin lock that protects overrides against simultaneous use from
   different threads, outside signal handlers.  */
static glwthread_spinlock_t overrides_mt_lock = GLWTHREAD_SPIN_INIT;
/* A spin lock that protects overrides against simultaneous use from
   a signal handler and a pthread_sigmask invocation.  */
static glwthread_spinlock_t overrides_handler_lock = GLWTHREAD_SPIN_INIT;
#endif

/* Signal handler that overrides an original one.  */
static void
override_handler (int sig)
{
  /* Reinstall the handler, in case the signal occurs multiple times
     while blocked.  There is an inherent race where an asynchronous
     signal in between when the kernel uninstalled the handler and
     when we reinstall it will trigger the default handler; oh well.  */
  signal (sig, override_handler);

  if ((blocked_set >> sig) & 1)
    {
      /* The signal is blocked in the current thread.  */
      pending_array[sig] = 1;
    }
  else
    {
      if (!overrides[sig].overridden)
        {
          /* Another thread has already installed the override_handler but
             is not yet done.  Wait until it has finished the operation.  */
          glwthread_spin_lock (&overrides_handler_lock);
          glwthread_spin_unlock (&overrides_handler_lock);
          /* Now overrides[sig].overridden must be true.  */
          if (!overrides[sig].overridden)
            abort ();
        }
      handler_t handler = overrides[sig].original_handler;
      if (handler == SIG_DFL)
        {
          /* Restore the original handler.  Then raise the signal again.  */
          signal (sig, SIG_DFL);
          raise (sig);
        }
      else if (handler != SIG_IGN)
        (*handler) (sig);
    }
}

int
pthread_sigmask (int operation, const sigset_t *set, sigset_t *old_set)
{
  if (old_set != NULL)
    *old_set = blocked_set;

  if (set != NULL)
    {
      sigset_t new_blocked_set;
      switch (operation)
        {
        case SIG_BLOCK:
          new_blocked_set = blocked_set | *set;
          break;
        case SIG_SETMASK:
          new_blocked_set = *set;
          break;
        case SIG_UNBLOCK:
          new_blocked_set = blocked_set & ~*set;
          break;
        default:
          return EINVAL;
        }

      sigset_t to_unblock = blocked_set & ~new_blocked_set;
      sigset_t to_block = new_blocked_set & ~blocked_set;

      if (to_block != 0)
        for (int sig = 0; sig < NSIG; sig++)
          if ((to_block >> sig) & 1)
            {
              /* All pending_array[sig] remain zero.  */
              blocked_set |= 1U << sig;
              if (!overrides[sig].overridden)
                {
                  glwthread_spin_lock (&overrides_mt_lock);
                  glwthread_spin_lock (&overrides_handler_lock);
                  if (!overrides[sig].overridden)
                    {
                      /* Now it's OK to install the override_handler:
                         - If it gets invoked in this thread, there is no race
                           condition since blocked_set already has the sig bit
                           set.
                         - If it gets invoked in another thread, the
                           overrides_handler_lock protects it from proceeding
                           until we have stored the old_handler.  For this case,
                           it is important that the overrides_handler_lock is
                           the innermost taken lock.  */
                      handler_t old_handler = signal (sig, override_handler);
                      if (old_handler != SIG_ERR)
                        {
                          if (old_handler == override_handler)
                            /* Different threads calling pthread_sigmask at the
                               same time (race condition).  This shouldn't
                               happen, thanks to the second test of
                               overrides[sig].overridden, above.  */
                            abort ();
                          overrides[sig].original_handler = old_handler;
                          overrides[sig].overridden = 1;
                        }
                    }
                  glwthread_spin_unlock (&overrides_handler_lock);
                  glwthread_spin_unlock (&overrides_mt_lock);
                }
            }

      if (to_unblock != 0)
        {
          sig_atomic_t received[NSIG];

          for (int sig = 0; sig < NSIG; sig++)
            if ((to_unblock >> sig) & 1)
              {
                received[sig] = pending_array[sig];
                blocked_set &= ~(1U << sig);
                pending_array[sig] = 0;
              }
            else
              received[sig] = 0;

          for (int sig = 0; sig < NSIG; sig++)
            if (received[sig])
              raise (sig);
        }
    }

  return 0;
}

/* sigprocmask is like pthread_sigmask, but has unspecified behaviour
   in multithreaded processes and a different return value convention.  */
int
sigprocmask (int operation, const sigset_t *set, sigset_t *old_set)
{
  int ret = pthread_sigmask (operation, set, old_set);
  if (ret == 0)
    return 0;
  else
    {
      errno = ret;
      return -1;
    }
}

/* Install the handler FUNC for signal SIG, and return the previous
   handler.
   This override transparently hides the override_handler.  */
handler_t
rpl_signal (int sig, handler_t handler)
{
  /* We must provide a wrapper, so that a user can query what handler
     they installed even if that signal is currently blocked.  */
  if (sig >= 0 && sig < NSIG && sig != SIGKILL && sig != SIGSTOP
      && handler != SIG_ERR)
    {
      #ifdef SIGABRT_COMPAT
      if (sig == SIGABRT_COMPAT)
        sig = SIGABRT;
      #endif

      handler_t result;
      if (overrides[sig].overridden)
        {
          /* There is an inherent race condition, when one thread calls
             rpl_signal to install a different signal handler, while another
             thread invokes the signal handler (via override_handler).
             It is unavoidable.  Nothing we can do about it.  */
          result = overrides[sig].original_handler;
          overrides[sig].original_handler = handler;
        }
      else
        {
          /* Lock, in case of another thread calling pthread_sigmask, with the
             effect of installing the override_handler (race condition).  */
          glwthread_spin_lock (&overrides_mt_lock);
          if (overrides[sig].overridden)
            {
              result = overrides[sig].original_handler;
              overrides[sig].original_handler = handler;
            }
          else
            result = signal (sig, handler);
          glwthread_spin_unlock (&overrides_mt_lock);
        }
      return result;
    }
  else
    {
      errno = EINVAL;
      return SIG_ERR;
    }
}

#if GNULIB_defined_SIGPIPE
/* Raise the signal SIGPIPE.  */
int
_gl_raise_SIGPIPE (void)
{
  /* On POSIX platforms, SIGPIPE is generated by the kernel and delivered to
     any thread of the current process.  In the SIGPIPE emulation here, we do
     it slightly differently: we deliver it to the current thread always,
     like  raise (SIGPIPE)  would do.  */
  if (blocked_set & (1U << SIGPIPE))
    /* The signal is blocked in the current thread.  */
    pending_array[SIGPIPE] = 1;
  else
    {
      handler_t handler = SIGPIPE_handler;
      if (handler == SIG_DFL)
        exit (128 + SIGPIPE);
      else if (handler != SIG_IGN)
        (*handler) (SIGPIPE);
    }
  return 0;
}
#endif
