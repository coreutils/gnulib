/* Test of lstat() function.
   Copyright (C) 2008, 2009 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson, 2008; and Eric Blake, 2009.  */

#include <config.h>

#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "same-inode.h"

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

#define BASE "test-lstat.t"

int
main ()
{
  struct stat st1;
  struct stat st2;

  /* Remove any leftovers from a previous partial run.  */
  ASSERT (system ("rm -rf " BASE "*") == 0);

  /* Test for common directories.  */
  ASSERT (lstat (".", &st1) == 0);
  ASSERT (lstat ("./", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (S_ISDIR (st1.st_mode));
  ASSERT (S_ISDIR (st2.st_mode));
  ASSERT (lstat ("/", &st1) == 0);
  ASSERT (lstat ("///", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (S_ISDIR (st1.st_mode));
  ASSERT (S_ISDIR (st2.st_mode));
  ASSERT (lstat ("..", &st1) == 0);
  ASSERT (S_ISDIR (st1.st_mode));

  /* Test for error conditions.  */
  errno = 0;
  ASSERT (lstat ("", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (lstat ("nosuch", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (lstat ("nosuch/", &st1) == -1);
  ASSERT (errno == ENOENT);

  ASSERT (close (creat (BASE "file", 0600)) == 0);
  ASSERT (lstat (BASE "file", &st1) == 0);
  ASSERT (S_ISREG (st1.st_mode));
  errno = 0;
  ASSERT (lstat (BASE "file/", &st1) == -1);
  ASSERT (errno == ENOTDIR);

  /* Now for some symlink tests, where supported.  We set up:
     link1 -> directory
     link2 -> file
     link3 -> dangling
     link4 -> loop
     then test behavior both with and without trailing slash.
  */
  if (symlink (".", BASE "link1") != 0)
    {
      ASSERT (unlink (BASE "file") == 0);
      fputs ("skipping test: symlinks not supported on this filesystem\n",
             stderr);
      return 77;
    }
  ASSERT (symlink (BASE "file", BASE "link2") == 0);
  ASSERT (symlink (BASE "nosuch", BASE "link3") == 0);
  ASSERT (symlink (BASE "link4", BASE "link4") == 0);

  ASSERT (lstat (BASE "link1", &st1) == 0);
  ASSERT (S_ISLNK (st1.st_mode));
  ASSERT (lstat (BASE "link1/", &st1) == 0);
  ASSERT (stat (BASE "link1", &st2) == 0);
  ASSERT (S_ISDIR (st1.st_mode));
  ASSERT (S_ISDIR (st2.st_mode));
  ASSERT (SAME_INODE (st1, st2));

  ASSERT (lstat (BASE "link2", &st1) == 0);
  ASSERT (S_ISLNK (st1.st_mode));
  errno = 0;
  ASSERT (lstat (BASE "link2/", &st1) == -1);
  ASSERT (errno == ENOTDIR);

  ASSERT (lstat (BASE "link3", &st1) == 0);
  ASSERT (S_ISLNK (st1.st_mode));
  errno = 0;
  ASSERT (lstat (BASE "link3/", &st1) == -1);
  ASSERT (errno == ENOENT);

  ASSERT (lstat (BASE "link4", &st1) == 0);
  ASSERT (S_ISLNK (st1.st_mode));
  errno = 0;
  ASSERT (lstat (BASE "link4/", &st1) == -1);
  ASSERT (errno == ELOOP);

  /* Cleanup.  */
  ASSERT (unlink (BASE "file") == 0);
  ASSERT (unlink (BASE "link1") == 0);
  ASSERT (unlink (BASE "link2") == 0);
  ASSERT (unlink (BASE "link3") == 0);
  ASSERT (unlink (BASE "link4") == 0);

  return 0;
}
