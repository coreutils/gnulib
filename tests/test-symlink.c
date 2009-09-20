/* Tests of symlink.
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

#define BASE "test-symlink.t"

int
main ()
{
  /* Remove any leftovers from a previous partial run.  */
  ASSERT (system ("rm -rf " BASE "*") == 0);

  if (symlink ("nowhere", BASE "link1"))
    {
      fputs ("skipping test: symlinks not supported on this filesystem\n",
	     stderr);
      return 77;
    }

  /* Some systems allow the creation of 0-length symlinks as a synonym
     for "."; but most reject it.  */
  errno = 0;
  if (symlink ("", BASE "link2") == -1)
    ASSERT (errno == ENOENT || errno == EINVAL);
  else
    ASSERT (unlink (BASE "link2") == 0);

  /* Sanity checks of failures.  */
  errno = 0;
  ASSERT (symlink ("nowhere", "") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (symlink ("nowhere", ".") == -1);
  ASSERT (errno == EEXIST || errno == EINVAL);
  errno = 0;
  ASSERT (symlink ("somewhere", BASE "link1") == -1);
  ASSERT (errno == EEXIST);
  errno = 0;
  ASSERT (symlink ("nowhere", BASE "link2/") == -1);
  ASSERT (errno == ENOTDIR || errno == ENOENT);
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  errno = 0;
  ASSERT (symlink ("nowhere", BASE "dir") == -1);
  ASSERT (errno == EEXIST);
  errno = 0;
  ASSERT (symlink ("nowhere", BASE "dir/") == -1);
  ASSERT (errno == EEXIST);
  ASSERT (close (creat (BASE "file", 0600)) == 0);
  errno = 0;
  ASSERT (symlink ("nowhere", BASE "file") == -1);
  ASSERT (errno == EEXIST);
  errno = 0;
  ASSERT (symlink ("nowhere", BASE "file/") == -1);
  ASSERT (errno == EEXIST || errno == ENOTDIR);

  ASSERT (rmdir (BASE "dir") == 0);
  ASSERT (unlink (BASE "file") == 0);
  ASSERT (unlink (BASE "link1") == 0);

  return 0;
}
