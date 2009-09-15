/* Tests of stat.
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

#include <sys/stat.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pathmax.h"
#include "same-inode.h"

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

#define BASE "test-stat.t"

int
main ()
{
  struct stat st1;
  struct stat st2;
  char cwd[PATH_MAX];

  ASSERT (getcwd (cwd, PATH_MAX) == cwd);
  ASSERT (stat (".", &st1) == 0);
  ASSERT (stat ("./", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (stat (cwd, &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (stat ("/", &st1) == 0);
  ASSERT (stat ("///", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));

  errno = 0;
  ASSERT (stat ("", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (stat ("nosuch", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (stat ("nosuch/", &st1) == -1);
  ASSERT (errno == ENOENT);

  ASSERT (close (creat (BASE "file", 0600)) == 0);
  ASSERT (stat (BASE "file", &st1) == 0);
  errno = 0;
  ASSERT (stat (BASE "file/", &st1) == -1);
  ASSERT (errno == ENOTDIR);
  ASSERT (unlink (BASE "file") == 0);

  return 0;
}
