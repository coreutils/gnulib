/* Tests of mkfifoat and mknodat.
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
#include <string.h>
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

typedef int (*test_func) (int, char const *, mode_t);

/* Wrapper to make testing mknodat easier.  */
static int
test_mknodat (int fd, char const *name, mode_t mode)
{
  /* This is the only portable use of mknodat, per POSIX.  */
  return mknodat (fd, name, mode | S_IFIFO, 0);
}

int
main ()
{
  int i;
  test_func funcs[2] = { mkfifoat, test_mknodat };
  const char *fifo = "test-mkfifoat.fifo";

  /* Create handle for future use.  */
  int dfd = openat (AT_FDCWD, ".", O_RDONLY);
  ASSERT (0 <= dfd);

#if !HAVE_MKFIFO
  fputs ("skipping test: no support for named fifos\n", stderr);
  return 77;
#endif

  /* Clean up anything from previous incomplete test.  */
  remove (fifo);

  /* Test both functions.  */
  for (i = 0; i < 2; i++)
    {
      struct stat st;
      test_func func = funcs[i];

      /* Sanity checks of failures.  */
      errno = 0;
      ASSERT (func (AT_FDCWD, "", 0600) == -1);
      ASSERT (errno == ENOENT);
      errno = 0;
      ASSERT (func (dfd, "", S_IRUSR | S_IWUSR) == -1);
      ASSERT (errno == ENOENT);
      errno = 0;
      ASSERT (func (AT_FDCWD, ".", 0600) == -1);
      /* POSIX requires EEXIST, but Solaris gives EINVAL.  */
      ASSERT (errno == EEXIST || errno == EINVAL);
      errno = 0;
      ASSERT (func (dfd, ".", 0600) == -1);
      ASSERT (errno == EEXIST || errno == EINVAL);

      /* Create fifo while cwd is '.', then stat it from '..'.  */
      ASSERT (func (AT_FDCWD, fifo, 0600) == 0);
      errno = 0;
      ASSERT (func (dfd, fifo, 0600) == -1);
      ASSERT (errno == EEXIST);
      ASSERT (chdir ("..") == 0);
      errno = 0;
      ASSERT (fstatat (AT_FDCWD, fifo, &st, 0) == -1);
      ASSERT (errno == ENOENT);
      memset (&st, 0, sizeof st);
      ASSERT (fstatat (dfd, fifo, &st, 0) == 0);
      ASSERT (S_ISFIFO (st.st_mode));
      ASSERT (unlinkat (dfd, fifo, 0) == 0);

      /* Create fifo while cwd is '..', then stat it from '.'.  */
      ASSERT (func (dfd, fifo, 0600) == 0);
      ASSERT (fchdir (dfd) == 0);
      errno = 0;
      ASSERT (func (AT_FDCWD, fifo, 0600) == -1);
      ASSERT (errno == EEXIST);
      memset (&st, 0, sizeof st);
      ASSERT (fstatat (AT_FDCWD, fifo, &st, AT_SYMLINK_NOFOLLOW) == 0);
      ASSERT (S_ISFIFO (st.st_mode));
      memset (&st, 0, sizeof st);
      ASSERT (fstatat (dfd, fifo, &st, AT_SYMLINK_NOFOLLOW) == 0);
      ASSERT (S_ISFIFO (st.st_mode));
      ASSERT (unlink (fifo) == 0);
    }

  ASSERT (close (dfd) == 0);

  return 0;
}
