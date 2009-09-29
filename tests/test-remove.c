/* Tests of remove.
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

#include <stdio.h>

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

#define BASE "test-remove.t"

int
main ()
{
  /* Remove any leftovers from a previous partial run.  */
  ASSERT (system ("rm -rf " BASE "*") == 0);

  /* Setup.  */
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  ASSERT (close (creat (BASE "dir/file", 0600)) == 0);

  /* Basic error conditions.  */
  errno = 0;
  ASSERT (remove ("") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (remove ("nosuch") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (remove ("nosuch/") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (remove (".") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY);
  /* Resulting errno after ".." or "/" is too varied to test; it is
     reasonable to see any of EINVAL, EEXIST, ENOTEMPTY, EACCES.  */
  ASSERT (remove ("..") == -1);
  ASSERT (remove ("/") == -1);
  ASSERT (remove ("///") == -1);
  errno = 0;
  ASSERT (remove (BASE "dir/file/") == -1);
  ASSERT (errno == ENOTDIR);

  /* Non-empty directory.  */
  errno = 0;
  ASSERT (remove (BASE "dir") == -1);
  ASSERT (errno == EEXIST || errno == ENOTEMPTY);

  /* Non-directory.  */
  ASSERT (remove (BASE "dir/file") == 0);

  /* Empty directory.  */
  errno = 0;
  ASSERT (remove (BASE "dir/.//") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY);
  ASSERT (remove (BASE "dir") == 0);

  /* Test symlink behavior.  Specifying trailing slash should remove
     referent directory, or cause ENOTDIR failure, but not touch
     symlink.  */
  if (symlink (BASE "dir", BASE "link") != 0)
    {
      fputs ("skipping test: symlinks not supported on this filesystem\n",
             stderr);
      return 77;
    }
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  errno = 0;
  if (remove (BASE "link/") == 0)
    {
      struct stat st;
      errno = 0;
      ASSERT (stat (BASE "link", &st) == -1);
      ASSERT (errno == ENOENT);
    }
  else
    ASSERT (remove (BASE "dir") == 0);
  {
    struct stat st;
    ASSERT (lstat (BASE "link", &st) == 0);
    ASSERT (S_ISLNK (st.st_mode));
  }
  ASSERT (remove (BASE "link") == 0);

  return 0;
}
