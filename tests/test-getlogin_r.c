/* Test of getting user name.
   Copyright (C) 2010-2017 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include <unistd.h>

#include "signature.h"
#ifdef TEST_GETLOGIN
SIGNATURE_CHECK (getlogin, char *, (void));
#else
SIGNATURE_CHECK (getlogin_r, int, (char *, size_t));
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if !((defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__)
# include <pwd.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include "macros.h"

int
main (void)
{
  /* Test value.  */
#ifdef TEST_GETLOGIN
  char *buf = getlogin ();
  int err = buf ? 0 : errno;
  ASSERT (buf || err);
#else
  /* Test with a large enough buffer.  */
  char buf[1024];
  int err = getlogin_r (buf, sizeof buf);
#endif

  if (err != 0)
    {
      if (err == ENOENT)
        {
          /* This can happen on GNU/Linux.  */
          fprintf (stderr, "Skipping test: no entry in utmp file.\n");
          return 77;
        }

      /* It fails when stdin is not connected to a tty.  */
      ASSERT (err == ENOTTY
              || err == EINVAL /* seen on Linux/SPARC */
              || err == ENXIO
             );
#if !defined __hpux /* On HP-UX 11.11 it fails anyway.  */
      ASSERT (! isatty (0));
#endif
      fprintf (stderr, "Skipping test: stdin is not a tty.\n");
      return 77;
    }

  /* Compare against the value from the environment.  */
#if !((defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__)
  /* Unix platform */
  {
    struct stat stat_buf;
    struct passwd *pwd;

    if (!isatty (STDIN_FILENO))
      {
         fprintf (stderr, "Skipping test: stdin is not a tty.\n");
         return 77;
      }

    ASSERT (fstat (STDIN_FILENO, &stat_buf) == 0);

    pwd = getpwnam (buf);
    if (! pwd)
      {
        fprintf (stderr, "Skipping test: %s: no such user\n", buf);
        return 77;
      }

    ASSERT (pwd->pw_uid == stat_buf.st_uid);
  }
#endif
#if (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__
  /* Native Windows platform.
     Note: This test would fail on Cygwin in an ssh session, because sshd
     sets USERNAME=SYSTEM.  */
  {
    const char *name = getenv ("USERNAME");
    if (name != NULL && name[0] != '\0')
      ASSERT (strcmp (buf, name) == 0);
  }
#endif

#ifndef TEST_GETLOGIN
  /* Test with a small buffer.  */
  {
    char smallbuf[1024];
    size_t n = strlen (buf);
    size_t i;

    for (i = 0; i <= n; i++)
      {
        err = getlogin_r (smallbuf, i);
        if (i == 0)
          ASSERT (err == ERANGE || err == EINVAL);
        else
          ASSERT (err == ERANGE);
      }
  }

  /* Test with a huge buffer.  */
  {
    static char hugebuf[70000];

    ASSERT (getlogin_r (hugebuf, sizeof (hugebuf)) == 0);
    ASSERT (strcmp (hugebuf, buf) == 0);
  }
#endif

  return 0;
}
