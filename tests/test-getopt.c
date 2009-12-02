/* Test of command line argument processing.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2009.  */

#include <config.h>

/* None of the files accessed by this test are large, so disable the
   ftell link warning if we are not using the gnulib ftell module.  */
#if !GNULIB_FTELL
# undef GL_LINK_WARNING
# define GL_LINK_WARNING(ignored) ((void) 0)
#endif

#if GNULIB_GETOPT_GNU
# include <getopt.h>
#endif
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This test intentionally remaps stderr.  So, we arrange to have fd 10
   (outside the range of interesting fd's during the test) set up to
   duplicate the original stderr.  */

#define BACKUP_STDERR_FILENO 10
static FILE *myerr;

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (myerr, "%s:%d: assertion failed\n", __FILE__, __LINE__);  \
          fflush (myerr);                                                    \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

#include "test-getopt.h"
#if GNULIB_GETOPT_GNU
# include "test-getopt_long.h"
#endif

int
main (void)
{
   /* This test validates that stderr is used correctly, so move the
      original into fd 10.  */
  if (dup2 (STDERR_FILENO, BACKUP_STDERR_FILENO) != BACKUP_STDERR_FILENO
      || (myerr = fdopen (BACKUP_STDERR_FILENO, "w")) == NULL)
    return 2;

  ASSERT (freopen ("test-getopt.tmp", "w", stderr) == stderr);

  /* These default values are required by POSIX.  */
  ASSERT (optind == 1);
  ASSERT (opterr != 0);

  setenv ("POSIXLY_CORRECT", "1", 1);
  test_getopt ();

#if GNULIB_GETOPT_GNU
  test_getopt_long_posix ();
#endif

  unsetenv ("POSIXLY_CORRECT");
  test_getopt ();

#if GNULIB_GETOPT_GNU
  test_getopt_long ();
  test_getopt_long_only ();
#endif

  ASSERT (fclose (stderr) == 0);
  ASSERT (remove ("test-getopt.tmp") == 0);

  return 0;
}
