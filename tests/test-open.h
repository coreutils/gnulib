/* Test of opening a file descriptor.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

/* Include <config.h> and a form of <fcntl.h> first.  */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);						     \
          abort ();							     \
        }								     \
    }									     \
  while (0)

/* Test fopen.  Assumes BASE is defined.  */

static int
test_open (void)
{
  int fd;
  /* Remove anything from prior partial run.  */
  unlink (BASE "file");

  /* Cannot create directory.  */
  errno = 0;
  ASSERT (open ("nonexist.ent/", O_CREAT | O_RDONLY, 0600) == -1);
  ASSERT (errno == ENOTDIR || errno == EISDIR || errno == ENOENT);

  /* Create a regular file.  */
  fd = open (BASE "file", O_CREAT | O_RDONLY, 0600);
  ASSERT (0 <= fd);
  ASSERT (close (fd) == 0);

  /* Trailing slash handling.  */
  errno = 0;
  ASSERT (open (BASE "file/", O_RDONLY) == -1);
  ASSERT (errno == ENOTDIR || errno == EISDIR);

  /* Directories cannot be opened for writing.  */
  errno = 0;
  ASSERT (open (".", O_WRONLY) == -1);
  ASSERT (errno == EISDIR);

  /* /dev/null must exist, and be writable.  */
  fd = open ("/dev/null", O_RDONLY);
  ASSERT (0 <= fd);
  {
    char c;
    ASSERT (read (fd, &c, 1) == 0);
  }
  ASSERT (close (fd) == 0);
  fd = open ("/dev/null", O_WRONLY);
  ASSERT (0 <= fd);
  ASSERT (write (fd, "c", 1) == 1);
  ASSERT (close (fd) == 0);

  /* Cleanup.  */
  ASSERT (unlink (BASE "file") == 0);

  return 0;
}
