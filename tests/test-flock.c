/* Test of flock() function.
   Copyright (C) 2008-2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <sys/file.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define ASSERT(expr) \
  do                                                                    \
    {                                                                   \
      if (!(expr))                                                      \
        {                                                               \
          fprintf (stderr, "%s:%d: assertion failed, errno = %d\n",     \
                   __FILE__, __LINE__, errno);                          \
          fflush (stderr);                                              \
          abort ();                                                     \
        }                                                               \
    }                                                                   \
  while (0)

static void
test_shared (const char *file, int fd)
{
  /* Should be able to acquire several shared locks on a file, through
   * different file table entries.
   */
  int fd2, r;

  ASSERT (flock (fd, LOCK_SH) == 0);

  fd2 = open (file, O_RDWR, 0644);
  ASSERT (fd2 >= 0);

  r = flock (fd2, LOCK_SH | LOCK_NB);
  ASSERT (r == 0);              /* Was able to acquire a second shared lock. */

  ASSERT (flock (fd, LOCK_UN) == 0);
  ASSERT (close (fd2) == 0);
}

static void
test_exclusive (const char *file, int fd)
{
  /* Should not be able to acquire more than one exclusive lock on a file. */
  int fd2, r;

  ASSERT (flock (fd, LOCK_EX) == 0);

  fd2 = open (file, O_RDWR, 0644);
  ASSERT (fd2 >= 0);

  r = flock (fd2, LOCK_SH | LOCK_NB);
  ASSERT (r == -1);             /* Was unable to acquire a second exclusive lock. */

  ASSERT (flock (fd, LOCK_UN) == 0);
  ASSERT (close (fd2) == 0);
}

int
main (int argc, char *argv[])
{
  int fd;
  const char *file = "test-flock.txt";

  /* Open a non-empty file for testing. */
  fd = open (file, O_RDWR | O_CREAT | O_TRUNC, 0644);
  ASSERT (fd >= 0);
  ASSERT (write (fd, "hello", 5) == 5);

  /* Some impossible operation codes which should never be accepted. */
  ASSERT (flock (fd, LOCK_SH | LOCK_EX) == -1);
  ASSERT (errno == EINVAL);
  ASSERT (flock (fd, LOCK_SH | LOCK_UN) == -1);
  ASSERT (errno == EINVAL);
  ASSERT (flock (fd, LOCK_EX | LOCK_UN) == -1);
  ASSERT (errno == EINVAL);
  ASSERT (flock (fd, 0) == -1);
  ASSERT (errno == EINVAL);

  test_shared (file, fd);
  test_exclusive (file, fd);

  /* Close and remove the test file. */
  ASSERT (close (fd) == 0);
  ASSERT (unlink (file) == 0);

  return 0;
}
