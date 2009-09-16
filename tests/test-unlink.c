/* Tests of unlink.
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

#include "unlinkdir.h"

#if !HAVE_SYMLINK
# define symlink(a,b) (-1)
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

#define BASE "test-unlink.t"

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
  ASSERT (unlink ("") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (unlink (BASE "nosuch") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (unlink (BASE "nosuch/") == -1);
  ASSERT (errno == ENOENT);
  /* Resulting errno after directories is rather varied across
     implementations (EPERM, EINVAL, EACCES, EBUSY, EISDIR, ENOTSUP);
     however, we must be careful to not attempt unlink on a directory
     unless we know it must fail.  */
  if (cannot_unlink_dir ())
    {
      ASSERT (unlink (".") == -1);
      ASSERT (unlink ("..") == -1);
      ASSERT (unlink ("/") == -1);
      ASSERT (unlink (BASE "dir") == -1);
      ASSERT (mkdir (BASE "dir1", 0700) == 0);
      ASSERT (unlink (BASE "dir1") == -1);
      ASSERT (rmdir (BASE "dir1") == 0);
    }
  errno = 0;
  ASSERT (unlink (BASE "dir/file/") == -1);
  ASSERT (errno == ENOTDIR);

  /* Test symlink behavior.  Specifying trailing slash will attempt
     unlink of a directory, so only attempt it if we know it must
     fail.  */
  if (symlink (BASE "dir", BASE "link") != 0)
    {
      ASSERT (unlink (BASE "dir/file") == 0);
      ASSERT (rmdir (BASE "dir") == 0);
      fputs ("skipping test: symlinks not supported on this filesystem\n",
             stderr);
      return 77;
    }
  if (cannot_unlink_dir ())
    ASSERT (unlink (BASE "link/") == -1);
  ASSERT (unlink (BASE "link") == 0);
  ASSERT (symlink (BASE "dir/file", BASE "link") == 0);
  /* Order here proves unlink of a symlink does not follow through to
     the file.  */
  ASSERT (unlink (BASE "link") == 0);
  ASSERT (unlink (BASE "dir/file") == 0);
  ASSERT (rmdir (BASE "dir") == 0);

  return 0;
}
