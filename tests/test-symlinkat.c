/* Tests of symlinkat and readlinkat.
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

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#include <config.h>

#include <unistd.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifndef HAVE_SYMLINK
# define HAVE_SYMLINK 0
#endif

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
	{                                                                    \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__);  \
	  fflush (stderr);                                                   \
	  abort ();                                                          \
	}                                                                    \
    }                                                                        \
  while (0)

int
main ()
{
  char buf[80];

  /* Create handle for future use.  */
  int dfd = openat (AT_FDCWD, ".", O_RDONLY);
  ASSERT (0 <= dfd);

  /* Sanity checks of failures.  Mingw lacks symlinkat, but readlinkat
     can still distinguish between various errors.  */
  errno = 0;
  ASSERT (readlinkat (AT_FDCWD, "no_such", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlinkat (dfd, "no_such", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlinkat (AT_FDCWD, "", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlinkat (dfd, "", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlinkat (AT_FDCWD, ".", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  errno = 0;
  ASSERT (readlinkat (dfd, ".", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  errno = 0;
  ASSERT (symlinkat ("who cares", AT_FDCWD, "") == -1);
  ASSERT (errno == ENOENT || errno == ENOSYS);
  errno = 0;
  ASSERT (symlinkat ("who cares", dfd, "") == -1);
  ASSERT (errno == ENOENT || errno == ENOSYS);

  /* Skip everything else on mingw.  */
  if (HAVE_SYMLINK)
    {
      const char *linkname = "test-symlinkat.link";
      const char *contents = "don't matter!";
      int exp = strlen (contents);

      /* Create link while cwd is '.', then read it in '..'.  */
      ASSERT (symlinkat (contents, AT_FDCWD, linkname) == 0);
      errno = 0;
      ASSERT (symlinkat (contents, dfd, linkname) == -1);
      ASSERT (errno == EEXIST);
      ASSERT (chdir ("..") == 0);
      errno = 0;
      ASSERT (readlinkat (AT_FDCWD, linkname, buf, sizeof buf) == -1);
      ASSERT (errno == ENOENT);
      ASSERT (readlinkat (dfd, linkname, buf, sizeof buf) == exp);
      ASSERT (strncmp (contents, buf, exp) == 0);
      ASSERT (unlinkat (dfd, linkname, 0) == 0);

      /* Create link while cwd is '..', then read it in '.'.  */
      ASSERT (symlinkat (contents, dfd, linkname) == 0);
      ASSERT (fchdir (dfd) == 0);
      errno = 0;
      ASSERT (symlinkat (contents, AT_FDCWD, linkname) == -1);
      ASSERT (errno == EEXIST);
      buf[0] = '\0';
      ASSERT (readlinkat (AT_FDCWD, linkname, buf, sizeof buf) == exp);
      ASSERT (strncmp (contents, buf, exp) == 0);
      buf[0] = '\0';
      ASSERT (readlinkat (dfd, linkname, buf, sizeof buf) == exp);
      ASSERT (strncmp (contents, buf, exp) == 0);
      ASSERT (unlink (linkname) == 0);
    }

  ASSERT (close (dfd) == 0);

  return 0;
}
