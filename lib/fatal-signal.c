/* Emergency actions in case of a fatal signal.
   Copyright (C) 2003 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2003.

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* Specification.  */
#include "fatal-signal.h"

#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "xalloc.h"

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))


/* ========================================================================= */


/* The list of fatal signals.
   These are those signals whose default action is to terminate the process
   without a core dump, except
     SIGKILL - because it cannot be caught,
     SIGALRM SIGUSR1 SIGUSR2 SIGPOLL SIGIO SIGLOST - because applications
       often use them for their own purpose,
     SIGPROF SIGVTALRM - because they are used for profiling,
     SIGSTKFLT - because it is more similar to SIGFPE, SIGSEGV, SIGBUS,
     SIGSYS - because it is more similar to SIGABRT, SIGSEGV,
     SIGPWR - because it of too special use,
   plus
     SIGXCPU, SIGXFSZ - because they are quite similar to SIGTERM.  */

static const int fatal_signals[] =
  {
    /* ISO C 99 signals.  */
#ifdef SIGINT
    SIGINT,
#endif
#ifdef SIGTERM
    SIGTERM,
#endif
    /* POSIX:2001 signals.  */
#ifdef SIGHUP
    SIGHUP,
#endif
#ifdef SIGPIPE
    SIGPIPE,
#endif
    /* BSD signals.  */
#ifdef SIGXCPU
    SIGXCPU,
#endif
#ifdef SIGXFSZ
    SIGXFSZ,
#endif
    0
  };

#define num_fatal_signals (SIZEOF (fatal_signals) - 1)


/* ========================================================================= */


/* The registered cleanup actions.  */
typedef void (*action_t) (void);
static action_t static_actions[32];
static action_t * volatile actions = static_actions;
static size_t volatile actions_count = 0;
static size_t actions_allocated = SIZEOF (static_actions);


/* Uninstall the handlers.  */
static inline void
uninstall_handlers ()
{
  size_t i;

  for (i = 0; i < num_fatal_signals; i++)
    signal (fatal_signals[i], SIG_DFL);
}


/* The signal handler.  It gets called asynchronously.  */
static void
fatal_signal_handler (int sig)
{
  for (;;)
    {
      /* Get the last registered cleanup action, in a reentrant way.  */
      action_t action;
      size_t n = actions_count;
      if (n == 0)
	break;
      n--;
      actions_count = n;
      action = actions[n];
      /* Execute the action.  */
      action ();
    }

  /* Now execute the signal's default action.  */
  uninstall_handlers ();
#if HAVE_RAISE
  raise (sig);
#else
  kill (getpid (), sig);
#endif
}


/* Install the handlers.  */
static inline void
install_handlers ()
{
  size_t i;

  for (i = 0; i < num_fatal_signals; i++)
    signal (fatal_signals[i], &fatal_signal_handler);
}


/* Register a cleanup function to be executed when a catchable fatal signal
   occurs.  */
void
at_fatal_signal (action_t action)
{
  static bool cleanup_initialized = false;
  if (!cleanup_initialized)
    {
      install_handlers ();
      cleanup_initialized = true;
    }

  if (actions_count == actions_allocated)
    {
      /* Extend the actions array.  Note that we cannot use xrealloc(),
	 because then the cleanup() function could access an already
	 deallocated array.  */
      action_t *old_actions = actions;
      size_t new_actions_allocated = 2 * actions_allocated;
      action_t *new_actions =
	xmalloc (new_actions_allocated * sizeof (action_t));

      memcpy (new_actions, actions, actions_allocated * sizeof (action_t));
      actions = new_actions;
      actions_allocated = new_actions_allocated;
      /* Now we can free the old actions array.  */
      if (old_actions != static_actions)
	free (old_actions);
    }
  actions[actions_count] = action;
  actions_count++;
}


/* ========================================================================= */


#if HAVE_POSIX_SIGNALBLOCKING

static sigset_t fatal_signal_set;

static void
init_fatal_signal_set ()
{
  static bool fatal_signal_set_initialized = false;
  if (!fatal_signal_set_initialized)
    {
      size_t i;

      sigemptyset (&fatal_signal_set);
      for (i = 0; i < num_fatal_signals; i++)
	sigaddset (&fatal_signal_set, fatal_signals[i]);

      fatal_signal_set_initialized = true;
    }
}

void
block_fatal_signals ()
{
  init_fatal_signal_set ();
  sigprocmask (SIG_BLOCK, &fatal_signal_set, NULL);
}

void
unblock_fatal_signals ()
{
  init_fatal_signal_set ();
  sigprocmask (SIG_UNBLOCK, &fatal_signal_set, NULL);
}

#else

/* Don't bother caring about the old systems which don't have POSIX signal
   blocking.  */

void
block_fatal_signals ()
{
}

void
unblock_fatal_signals ()
{
}

#endif
