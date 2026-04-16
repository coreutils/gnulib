/* Delaying the delivery of signals to the current process.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#include <config.h>

/* Specification.  */
#include "sigdelay.h"

#include <errno.h>
#include <stdcountof.h>
#include <stdlib.h>

#include "glthread/lock.h"
#include "thread-optim.h"
#include "sig-handler.h"

/* State regarding a single signal.  */
struct state
{
  /* Number of times sigdelay (SIG_BLOCK, ...) was invoked for this signal.  */
  unsigned int count;
  /* If count > 0: The original action for the signal.  */
  struct sigaction saved_action;
  /* Whether this signal was caught and is waiting to be re-delivered.  */
  unsigned int volatile caught;
};

/* The state for all signals.
   Size 32 would not be sufficient: On HP-UX, SIGXCPU = 33, SIGXFSZ = 34.  */
static struct state states[64];

static _GL_ASYNC_SAFE void
delaying_handler (int sig)
{
  if (sig >= 0 && sig < countof (states))
    states[sig].caught = 1;
}

/* Lock that makes sigdelay multi-thread safe.  */
gl_lock_define_initialized (static, sigdelay_lock)

int
sigdelay (int how, const sigset_t *restrict set, sigset_t *restrict old_set)
{
  if (!(how == SIG_BLOCK || how == SIG_UNBLOCK))
    {
      errno = EINVAL;
      return -1;
    }

  if (old_set != NULL)
    sigemptyset (old_set);

  bool mt = gl_multithreaded ();

  if (mt) gl_lock_lock (sigdelay_lock);

  if (old_set != NULL)
    for (int sig = 1; sig < countof (states); sig++)
      if (states[sig].count > 0)
        sigaddset (old_set, sig);

  for (int sig = 1; sig < countof (states); sig++)
    if (sigismember (set, sig) > 0)
      {
        switch (how)
          {
          case SIG_BLOCK:
            if (states[sig].count == 0)
              {
                states[sig].caught = 0;

                struct sigaction delaying_action;
                delaying_action.sa_handler = delaying_handler;
                delaying_action.sa_flags = SA_NODEFER;
                sigemptyset (&delaying_action.sa_mask);
                if (sigaction (sig, &delaying_action,
                               &states[sig].saved_action) < 0)
                  abort ();
              }
            states[sig].count++;
            break;

          case SIG_UNBLOCK:
            if (states[sig].count == 0)
              /* Invalid call.  */
              abort ();
            states[sig].count--;
            if (states[sig].count == 0)
              {
                unsigned int caught = states[sig].caught;
                if (sigaction (sig, &states[sig].saved_action, NULL) < 0)
                  abort ();
                if (caught)
                  raise (sig);
              }
            break;
          }
      }

  if (mt) gl_lock_unlock (sigdelay_lock);

  return 0;
}
