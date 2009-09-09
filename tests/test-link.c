/* Test of link() function.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);                                                   \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

#define BASE "test-link.t"

int
main (int argc, char **argv)
{
  int fd;
  int ret;

  /* Remove any garbage left from previous partial runs.  */
  ASSERT (system ("rm -rf " BASE "*") == 0);

  /* Create first file.  */
  fd = open (BASE "a", O_CREAT | O_EXCL | O_WRONLY, 0600);
  ASSERT (0 <= fd);
  ASSERT (write (fd, "hello", 5) == 5);
  ASSERT (close (fd) == 0);

  /* Not all file systems support link.  Mingw doesn't have reliable
     st_nlink on hard links, but our implementation does fail with
     EPERM on poor file systems, and we can detect the inferior stat()
     via st_ino.  Cygwin 1.5.x copies rather than links files on those
     file systems, but there, st_nlink and st_ino are reliable.  */
  ret = link (BASE "a", BASE "b");
  if (!ret)
  {
    struct stat st;
    ASSERT (stat (BASE "b", &st) == 0);
    if (st.st_ino && st.st_nlink != 2)
      {
        ASSERT (unlink (BASE "b") == 0);
        errno = EPERM;
        ret = -1;
      }
  }
  if (ret == -1)
    {
      /* If the device does not support hard links, errno is
	 EPERM on Linux, EOPNOTSUPP on FreeBSD.  */
      switch (errno)
	{
	case EPERM:
	case EOPNOTSUPP:
          fputs ("skipping test: "
                 "hard links are not supported on this file system\n", stderr);
          ASSERT (unlink (BASE "a") == 0);
	  return 77;
	default:
	  perror ("link");
	  return 1;
	}
    }
  ASSERT (ret == 0);

  /* Now, for some behavior tests.  Modify the contents of 'b', and
     ensure that 'a' can see it, both while 'b' exists and after.  */
  fd = open (BASE "b", O_APPEND | O_WRONLY);
  ASSERT (0 <= fd);
  ASSERT (write (fd, "world", 5) == 5);
  ASSERT (close (fd) == 0);
  {
    char buf[11] = { 0 };
    fd = open (BASE "a", O_RDONLY);
    ASSERT (0 <= fd);
    ASSERT (read (fd, buf, 10) == 10);
    ASSERT (strcmp (buf, "helloworld") == 0);
    ASSERT (close (fd) == 0);
    ASSERT (unlink (BASE "b") == 0);
    fd = open (BASE "a", O_RDONLY);
    ASSERT (0 <= fd);
    ASSERT (read (fd, buf, 10) == 10);
    ASSERT (strcmp (buf, "helloworld") == 0);
    ASSERT (close (fd) == 0);
  }

  /* Test for various error conditions.  Assumes hard links to
     directories are not permitted.  */
  ASSERT (mkdir (BASE "d", 0700) == 0);
  errno = 0;
  ASSERT (link (BASE "a", ".") == -1);
  ASSERT (errno == EEXIST || errno == EINVAL);
  errno = 0;
  ASSERT (link (BASE "a", BASE "a") == -1);
  ASSERT (errno == EEXIST);
  ASSERT (link (BASE "a", BASE "b") == 0);
  errno = 0;
  ASSERT (link (BASE "a", BASE "b") == -1);
  ASSERT (errno == EEXIST);
  errno = 0;
  ASSERT (link (BASE "a", BASE "d") == -1);
  ASSERT (errno == EEXIST);
  errno = 0;
  ASSERT (link (BASE "c", BASE "e") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (link (BASE "a", BASE "c/.") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (link (BASE "a/", BASE "c") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (link (BASE "a", BASE "c/") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (link (BASE "d", BASE "c") == -1);
  ASSERT (errno == EPERM || errno == EACCES);

  /* Clean up.  */
  ASSERT (unlink (BASE "a") == 0);
  ASSERT (unlink (BASE "b") == 0);
  errno = 0;
  ASSERT (unlink (BASE "c") == -1);
  ASSERT (errno == ENOENT);
  ASSERT (rmdir (BASE "d") == 0);

  return 0;
}
