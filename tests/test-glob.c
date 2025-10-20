/* Test of glob/globfree functions.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson <simon@josefsson.org>, 2009.  */

#include <config.h>

#include <glob.h>

#include "signature.h"
SIGNATURE_CHECK (glob, int, (char const *, int, int (*) (char const *, int),
                             glob_t *));
SIGNATURE_CHECK (globfree, void, (glob_t *));

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"

#define BASE "test-glob.t"
#define GL_NO_SUCH_FILE "/gnulib-magic-does-not-exist"

int
main ()
{
  int res;
  glob_t g;

  res = glob (".", 0, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob (".", GLOB_NOSORT, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob (".", GLOB_MARK, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);

  res = glob (".", GLOB_APPEND, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 2);
  globfree (&g);

  res = glob (".", GLOB_NOESCAPE, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob ("./*", 0, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc >= 1);
  globfree (&g);

  res = glob (GL_NO_SUCH_FILE, 0, NULL, &g);
  ASSERT (res == GLOB_NOMATCH);

  res = glob (GL_NO_SUCH_FILE, GLOB_NOCHECK, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  ASSERT (strcmp (g.gl_pathv[0], GL_NO_SUCH_FILE) == 0);
  globfree (&g);

  if ((symlink (GL_NO_SUCH_FILE, BASE "globlink1") == 0 || errno == EEXIST)
      && (symlink (".", BASE "globlink2") == 0 || errno == EEXIST)
      && (symlink (BASE "globfile", BASE "globlink3") == 0 || errno == EEXIST)
      && close (creat (BASE "globfile", 0666)) == 0)
    {
      res = glob (BASE "globlink[12]", 0, NULL, &g);
      ASSERT (res == 0 && g.gl_pathc == 2);
      ASSERT (strcmp (g.gl_pathv[0], BASE "globlink1") == 0);
      ASSERT (strcmp (g.gl_pathv[1], BASE "globlink2") == 0);
      globfree (&g);

      res = glob (BASE "globlink[123]/", 0, NULL, &g);
      ASSERT (res == 0 && g.gl_pathc == 1);
      ASSERT (strcmp (g.gl_pathv[0], BASE "globlink2/") == 0);
      globfree (&g);

      res = glob (BASE "globlink[12]", GLOB_MARK, NULL, &g);
      ASSERT (res == 0 && g.gl_pathc == 2);
      ASSERT (strcmp (g.gl_pathv[0], BASE "globlink1") == 0);
      ASSERT (strcmp (g.gl_pathv[1], BASE "globlink2/") == 0);
      globfree (&g);
    }

#if !(defined _WIN32 && !defined __CYGWIN__)
  /* Check for a glibc 2.42 bug where recursive calls cause the stack to
     overflow.  Test cases based on the following:
     <https://sourceware.org/PR30635>.
     <https://sourceware.org/PR33537>.  */
  char *pattern = malloc (10000);
  if (pattern != NULL)
    {
      /* "/////////".  */
      memset (pattern, '/', 9999);
      pattern[9999] = '\0';
      res = glob (pattern, 0, NULL, &g);
      ASSERT (res == 0);
      globfree (&g);

      /* On Android, /bin/sh does not exist.  It's /system/bin/sh instead.  */
# if !defined __ANDROID__
      /* "/*/////sh".  */
      memset (pattern, '/', 9997);
      pattern[1] = '*';
      strcpy (pattern + 9997, "sh");
      res = glob (pattern, 0, NULL, &g);
      ASSERT (res == 0);
      globfree (&g);
# endif

      free (pattern);
    }
#endif

  return test_exit_status;
}
