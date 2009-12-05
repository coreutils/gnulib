/* Test duplicating non-inheritable file descriptors.
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

#include "cloexec.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* Get declarations of the Win32 API functions.  */
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

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

/* Return non-zero if FD is open and inheritable across exec/spawn.  */
static int
is_inheritable (int fd)
{
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  /* On Win32, the initial state of unassigned standard file
     descriptors is that they are open but point to an
     INVALID_HANDLE_VALUE, and there is no fcntl.  */
  HANDLE h = (HANDLE) _get_osfhandle (fd);
  DWORD flags;
  if (h == INVALID_HANDLE_VALUE || GetHandleInformation (h, &flags) == 0)
    return 0;
  return (flags & HANDLE_FLAG_INHERIT) != 0;
#else
# ifndef F_GETFD
#  error Please port fcntl to your platform
# endif
  int i = fcntl (fd, F_GETFD);
  return 0 <= i && (i & FD_CLOEXEC) == 0;
#endif
}

int
main (void)
{
  const char *file = "test-cloexec.tmp";
  int fd = creat (file, 0600);
  int fd2;

  /* Assume std descriptors were provided by invoker.  */
  ASSERT (STDERR_FILENO < fd);
  ASSERT (is_inheritable (fd));

  /* Normal use of set_cloexec_flag.  */
  ASSERT (set_cloexec_flag (fd, true) == 0);
#if !((defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__)
  ASSERT (!is_inheritable (fd));
#endif
  ASSERT (set_cloexec_flag (fd, false) == 0);
  ASSERT (is_inheritable (fd));

  /* Normal use of dup_cloexec.  */
  fd2 = dup_cloexec (fd);
  ASSERT (fd < fd2);
  ASSERT (!is_inheritable (fd2));
  ASSERT (close (fd) == 0);
  ASSERT (dup_cloexec (fd2) == fd);
  ASSERT (!is_inheritable (fd));
  ASSERT (close (fd2) == 0);

  /* Test error handling.  */
  errno = 0;
  ASSERT (set_cloexec_flag (-1, false) == -1);
  ASSERT (errno == EBADF);
  errno = 0;
  ASSERT (set_cloexec_flag (10000000, false) == -1);
  ASSERT (errno == EBADF);
  errno = 0;
  ASSERT (set_cloexec_flag (fd2, false) == -1);
  ASSERT (errno == EBADF);
  errno = 0;
  ASSERT (dup_cloexec (-1) == -1);
  ASSERT (errno == EBADF);
  errno = 0;
  ASSERT (dup_cloexec (10000000) == -1);
  ASSERT (errno == EBADF);
  errno = 0;
  ASSERT (dup_cloexec (fd2) == -1);
  ASSERT (errno == EBADF);

  /* Clean up.  */
  ASSERT (close (fd) == 0);
  ASSERT (unlink (file) == 0);

  return 0;
}
