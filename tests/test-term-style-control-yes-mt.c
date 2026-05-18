/* Interactive test program for the term-style-control module.
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
#include "term-style-control.h"

#if USE_ISOC_THREADS || USE_POSIX_THREADS || USE_ISOC_AND_POSIX_THREADS || USE_WINDOWS_THREADS

#include <time.h>
#include "glthread/thread.h"

#include "test-term-style-control-yes.h"

/* Thread that just sleeps and is ready to receive signals.  */
static void *
other_thread_func (_GL_UNUSED void *arg)
{
  for (;;)
    {
      struct timespec duration;
      duration.tv_sec = 1;
      duration.tv_nsec = 0;
      nanosleep (&duration, NULL);
    }
}

int
main ()
{
  /* Create another thread.  */
  (void) gl_thread_create (other_thread_func, NULL);

  styled_yes_loop ();
}

#else

/* No multithreading available.  */

#include <stdio.h>

int
main ()
{
  fputs ("Skipping test: multithreading not enabled\n", stderr);
  return 77;
}

#endif
