/* Report a SIGPIPE signal and exit.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2008.  */

#include <config.h>

/* Specification.  */
#include "sigpipe-die.h"

#include <signal.h>
#include <stdlib.h>

#include "error.h"
#include "exitfail.h"

#include "gettext.h"
#define _(msgid) gettext (msgid)

void
sigpipe_die (void)
{
  error (exit_failure, 0, "%s",
         _("error writing to a closed pipe or socket"));

  /* Ensure that this function really does not return.  */
  abort ();
}

static void (*prepare_die_hook) (void);

/* This is the signal handler for SIGPIPE.  It is invoked synchronously,
   therefore it can make library calls to malloc(), gettext(), exit() etc. -
   although in general it is undefined behaviour to do such calls from
   within signal handlers.  */
static void
sigpipe_die_handler (int sig)
{
  if (prepare_die_hook != NULL)
    (*prepare_die_hook) ();
  sigpipe_die ();
}

void
install_sigpipe_die_handler (void (*prepare_die) (void))
{
  prepare_die_hook = prepare_die;

  /* Install the handler.  */
  {
    struct sigaction action;

    action.sa_handler = sigpipe_die_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    if (sigaction (SIGPIPE, &action, NULL) >= 0)
      {
        /* Unblock the signal (just in case).  This is needed because if the
           signal was blocked in the parent process, it is also blocked in
           this process: the mask of blocked signals is inherited across
           fork/exec (except for SIGCHLD).  */
        sigset_t sigpipe_set;

        sigemptyset (&sigpipe_set);
        sigaddset (&sigpipe_set, SIGPIPE);
        sigprocmask (SIG_UNBLOCK, &sigpipe_set, NULL);
      }
  }
}
