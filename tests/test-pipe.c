/* Test of create_pipe_bidi/wait_subprocess.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

#include "pipe.h"
#include "wait-process.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Depending on arguments, this test intentionally closes stderr or
   starts life with stderr closed.  So, the error messages might not
   always print, but at least the exit status will be reliable.  */
#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);                                                   \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

/* Create a bi-directional pipe to a test child, and validate that the
   child program returns the expected output.  The child is the same
   program as the parent ARGV0, but with different arguments.
   STDERR_CLOSED is true if we have already closed fd 2.  */
static void
test_pipe (const char *argv0, bool stderr_closed)
{
  int fd[2];
  const char *argv[3];
  pid_t pid;
  char buffer[2] = { 'a', 'a' };

  /* Set up child.  */
  argv[0] = argv0;
  argv[1] = stderr_closed ? "9" : "8";
  argv[2] = NULL;
  pid = create_pipe_bidi (argv0, argv0, (char **) argv,
			  false, true, true, fd);
  ASSERT (0 <= pid);
  ASSERT (STDERR_FILENO < fd[0]);
  ASSERT (STDERR_FILENO < fd[1]);

  /* Push child's input.  */
  ASSERT (write (fd[1], buffer, 1) == 1);

  /* Get child's output.  */
  ASSERT (read (fd[0], buffer, 2) == 1);

  /* Wait for child.  */
  ASSERT (wait_subprocess (pid, argv0, true, false, true, true, NULL) == 0);
  ASSERT (close (fd[0]) == 0);
  ASSERT (close (fd[1]) == 0);

  /* Check the result.  */
  ASSERT (buffer[0] == 'b');
  ASSERT (buffer[1] == 'a');
}

int
main (int argc, const char *argv[])
{
  int i;
  int test;
  ASSERT (argc == 2);
  test = atoi (argv[1]);
  switch (test)
    {
      /* Driver cases.  Selectively close various standard fds, to
         ensure the child process is not impacted by this.  */
    case 0:
      break;
    case 1:
      close (0);
      break;
    case 2:
      close (1);
      break;
    case 3:
      close (0);
      close (1);
      break;
    case 4:
      close (2);
      break;
    case 5:
      close (0);
      close (2);
      break;
    case 6:
      close (1);
      close (2);
      break;
    case 7:
      close (0);
      close (1);
      close (2);
      break;

      /* Slave cases.  Read one byte from fd 0, and write its value
         plus one to fd 1.  fd 2 should be closed iff the argument is
         9.  Check that no other fd's leaked.  */
    case 8:
    case 9:
      {
        char buffer[1];
        ASSERT (read (STDIN_FILENO, buffer, 1) == 1);
        buffer[0]++;
        ASSERT (write (STDOUT_FILENO, buffer, 1) == 1);
        errno = 0;
#ifdef F_GETFL
        /* Try to keep stderr open for better diagnostics.  */
        i = fcntl (STDERR_FILENO, F_GETFL);
#else
        /* But allow compilation on mingw.  */
        i = close (STDERR_FILENO);
#endif
        if (test == 8)
          ASSERT (0 <= i);
        else
          {
            ASSERT (i < 0);
            ASSERT (errno == EBADF);
          }
        for (i = 3; i < 7; i++)
          {
            errno = 0;
            ASSERT (close (i) == -1);
            ASSERT (errno == EBADF);
          }
        return 0;
      }
    default:
      ASSERT (0);
    }
  /* All remaining code is for the driver.  Plug any leaks inherited
     from outside world before starting, so that child has a clean
     slate (at least for the fds that we might be manipulating).  */
  for (i = 3; i < 7; i++)
    close (i);
  test_pipe (argv[0], 3 < test);
  return 0;
}
