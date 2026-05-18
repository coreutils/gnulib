/* Test of optimization of multithreaded code..
   Copyright (C) 2026 Free Software Foundation, Inc.

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
#include "thread-optim.h"

#include "glthread/thread.h"

#include "macros.h"

#if USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS || USE_WINDOWS_THREADS

static int dummy;

static void *
thread_func (void *arg)
{
  return arg;
}

int
main ()
{
#if !defined __gnu_hurd__
# if defined _GL_MULTITHREADED_ALWAYS_TRUE || defined _GL_MULTITHREADED_VIA_ELF
  ASSERT (gl_multithreaded ());
# else
  ASSERT (! gl_multithreaded ());
# endif
#endif

  (void) gl_thread_create (thread_func, &dummy);

  ASSERT (gl_multithreaded ());

  return test_exit_status;
}

#else

/* No multithreading available.
   glthread_create is a dummy that just returns ENOSYS.
   gl_thread_create therefore aborts.  */

#include <stdio.h>

int
main ()
{
  fputs ("Skipping test: multithreading not enabled\n", stderr);
  return 77;
}

#endif
