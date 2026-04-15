/* Test of sigdelay in a multi-threaded process.
   Copyright (C) 2011-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

/* Specification.  */
#include "sigdelay.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "virtualbox.h"
#include "macros.h"

#if USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS

static pthread_t main_thread;
static pthread_t killer_thread1;
static pthread_t killer_thread2;

static void *
killer_thread1_func (_GL_UNUSED void *arg)
{
  sleep (1);
  pthread_kill (main_thread, SIGINT);
  return NULL;
}

static void *
killer_thread2_func (_GL_UNUSED void *arg)
{
  sleep (1);
  pthread_kill (pthread_self (), SIGINT);
  return NULL;
}

static volatile int sigint_occurred;

static void
sigint_handler (_GL_UNUSED int sig)
{
  sigint_occurred++;
}

int
main ()
{
  /* This test occasionally fails on Linux (glibc or musl libc), in a
     VirtualBox VM with paravirtualization = Default or KVM, with ≥ 2 CPUs,
     when "Nested VT-x/AMD-V" and "PAE/NX" are not both enabled.
     Skip the test in this situation.  */
  if (is_running_under_virtualbox_kvm () && num_cpus () > 1)
    {
      fputs ("Skipping test: avoiding VirtualBox bug with KVM paravirtualization\n",
             stderr);
      return 77;
    }

  signal (SIGINT, sigint_handler);

  sigset_t set;
  sigemptyset (&set);
  sigaddset (&set, SIGINT);

  /* Check error handling.  */
  ASSERT (sigdelay (1729, &set, NULL) == -1);
  ASSERT (errno == EINVAL);

  /* Block SIGINT.  */
  ASSERT (sigdelay (SIG_BLOCK, &set, NULL) == 0);

  /* Request a SIGINT signal from another thread.  */
  main_thread = pthread_self ();
  ASSERT (pthread_create (&killer_thread1, NULL, killer_thread1_func, NULL)
          == 0);

  /* Wait.  */
  sleep (2);

  /* The signal should not have arrived yet, because it is blocked.  */
  ASSERT (sigint_occurred == 0);

  /* Unblock SIGINT.  */
  ASSERT (sigdelay (SIG_UNBLOCK, &set, NULL) == 0);

  /* The signal should have arrived now.  */
  ASSERT (sigint_occurred == 1);

  /* Request a SIGINT signal from another thread.  */
  ASSERT (pthread_create (&killer_thread2, NULL, killer_thread2_func, NULL)
          == 0);

  /* Block SIGINT.  */
  ASSERT (sigdelay (SIG_BLOCK, &set, NULL) == 0);

  /* Wait.  */
  sleep (2);

  /* The signal should not have arrived yet, because it is blocked.  */
  ASSERT (sigint_occurred == 1);

  /* Unblock SIGINT.  */
  ASSERT (sigdelay (SIG_UNBLOCK, &set, NULL) == 0);

  /* The signal should have arrived now.  */
  ASSERT (sigint_occurred == 2);

  /* Clean up the threads.  This avoids a "ThreadSanitizer: thread leak" warning
     from "gcc -fsanitize=thread".  */
  ASSERT (pthread_join (killer_thread1, NULL) == 0);
  ASSERT (pthread_join (killer_thread2, NULL) == 0);

  return test_exit_status;
}

#else

int
main ()
{
  fputs ("Skipping test: POSIX threads not enabled\n", stderr);
  return 77;
}

#endif
