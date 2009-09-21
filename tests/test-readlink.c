/* Tests of readlink.
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

#define BASE "test-readlink.t"

int
main ()
{
  char buf[80];

  /* Remove any leftovers from a previous partial run.  */
  ASSERT (system ("rm -rf " BASE "*") == 0);

  /* Sanity checks of failures.  Mingw lacks symlink, but readlink can
     still distinguish between various errors.  */
  memset (buf, 0xff, sizeof buf);
  errno = 0;
  ASSERT (readlink ("no_such", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlink ("no_such/", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlink ("", buf, sizeof buf) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (readlink (".", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  errno = 0;
  ASSERT (readlink ("./", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  ASSERT (close (creat (BASE "file", 0600)) == 0);
  errno = 0;
  ASSERT (readlink (BASE "file", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  errno = 0;
  ASSERT (readlink (BASE "file/", buf, sizeof buf) == -1);
  ASSERT (errno == ENOTDIR);
  ASSERT (unlink (BASE "file") == 0);

  /* Now test actual symlinks.  */
  if (symlink (BASE "dir", BASE "link"))
    {
      fputs ("skipping test: symlinks not supported on this filesystem\n",
	     stderr);
      return 77;
    }
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  errno = 0;
  ASSERT (readlink (BASE "link/", buf, sizeof buf) == -1);
  ASSERT (errno == EINVAL);
  {
    /* Up till now, no readlink has been successful, so buf should be
       unchanged.  */
    int i;
    for (i = 0; i < sizeof buf; i++)
      ASSERT (buf[i] == (char) 0xff);
  }
  {
    size_t len = strlen (BASE "dir");
    /* When passing too small of a buffer, expect the truncated
       length, or an ERANGE failure.  However, a size of 0 is not
       portable enough to test.  */
    ssize_t result;
    errno = 0;
    result = readlink (BASE "link", buf, 1);
    if (result == -1)
      {
	ASSERT (errno == ERANGE);
	ASSERT (buf[0] == (char) 0xff);
      }
    else
      {
	ASSERT (result == 1);
	ASSERT (buf[0] == BASE[0]);
      }
    ASSERT (buf[1] == (char) 0xff);
    ASSERT (readlink (BASE "link", buf, len) == len);
    ASSERT (strncmp (buf, BASE "dir", len) == 0);
    ASSERT (buf[len] == (char) 0xff);
    ASSERT (readlink (BASE "link", buf, sizeof buf) == len);
    ASSERT (strncmp (buf, BASE "dir", len) == 0);
    /* POSIX says rest of buf is unspecified; but in practice, it is
       either left alone, or NUL-terminated.  */
    ASSERT (buf[len] == '\0' || buf[len] == (char) 0xff);
  }
  ASSERT (rmdir (BASE "dir") == 0);
  ASSERT (unlink (BASE "link") == 0);

  return 0;
}
