/* Test of sigaction() function.
   Copyright (C) 2008 Free Software Foundation, Inc.

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

/* Written by Eric Blake <ebb9@byu.net>, 2008.  */

#include <config.h>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          signal (SIGABRT, SIG_DFL);					     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

#ifndef SA_SIGINFO
# define SA_SIGINFO 0
#endif

/* This test is unsafe in the presence of an asynchronous SIGABRT,
   because we install a signal-handler that is intentionally not
   async-safe.  Hopefully, this does not lead to too many reports of
   false failures, since people don't generally use 'kill -s SIGABRT'
   to end a runaway program.  */

static void
handler (int sig)
{
  static int entry_count;
  struct sigaction sa;
  ASSERT (sig == SIGABRT);
  ASSERT (sigaction (SIGABRT, NULL, &sa) == 0);
  ASSERT ((sa.sa_flags & SA_SIGINFO) == 0);
  switch (entry_count++)
    {
    case 0:
      ASSERT ((sa.sa_flags & SA_RESETHAND) == 0);
      ASSERT (sa.sa_handler == handler);
      break;
    case 1:
      ASSERT (sa.sa_handler == SIG_DFL);
      break;
    default:
      ASSERT (0);
    }
}

int
main (int argc, char *argv[])
{
  struct sigaction sa;
  struct sigaction old_sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  ASSERT (sigemptyset (&sa.sa_mask) == 0);
  ASSERT (sigaction (SIGABRT, &sa, NULL) == 0);
  ASSERT (raise (SIGABRT) == 0);
  sa.sa_flags = SA_RESETHAND | SA_NODEFER;
  ASSERT (sigaction (SIGABRT, &sa, &old_sa) == 0);
  ASSERT (old_sa.sa_flags == 0);
  ASSERT (old_sa.sa_handler == handler);
  ASSERT (raise (SIGABRT) == 0);
  sa.sa_handler = SIG_DFL;
  ASSERT (sigaction (SIGABRT, &sa, &old_sa) == 0);
  ASSERT ((old_sa.sa_flags & SA_SIGINFO) == 0);
  ASSERT (old_sa.sa_handler == SIG_DFL);
  sa.sa_handler = SIG_IGN;
  ASSERT (sigaction (SIGABRT, &sa, NULL) == 0);
  ASSERT (raise (SIGABRT) == 0);
  ASSERT (sigaction (SIGABRT, NULL, &old_sa) == 0);
  ASSERT (old_sa.sa_handler == SIG_IGN);
  ASSERT (raise (SIGABRT) == 0);
  return 0;
}
