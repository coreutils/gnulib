/* Implement lchmod on platforms where it does not work correctly.

   Copyright 2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Paul Eggert */

#include <config.h>

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <intprops.h>

/* Work like lchmod, except when FILE is a symbolic link.
   In that case, set errno to EOPNOTSUPP and return -1.  */

int
lchmod (char const *file, mode_t mode)
{
#if HAVE_FCHMODAT
  return fchmodat (AT_FDCWD, file, mode, AT_SYMLINK_NOFOLLOW);
#else
# if defined AT_FDCWD && defined O_PATH && defined AT_EMPTY_PATH
  int fd = openat (AT_FDCWD, file, O_PATH | O_NOFOLLOW | O_CLOEXEC);
  if (fd < 0)
    return fd;

  /* Use fstatat because fstat does not work on O_PATH descriptors
     before Linux 3.6.  */
  struct stat st;
  if (fstatat (fd, "", &st, AT_EMPTY_PATH) != 0)
    {
      int stat_errno = errno;
      close (fd);
      errno = stat_errno;
      return -1;
    }
  if (S_ISLNK (st.st_mode))
    {
      close (fd);
      errno = EOPNOTSUPP;
      return -1;
    }

  static char const fmt[] = "/proc/self/fd/%d";
  char buf[sizeof fmt - sizeof "%d" + INT_BUFSIZE_BOUND (int)];
  sprintf (buf, fmt, fd);
  int chmod_result = chmod (buf, mode);
  int chmod_errno = errno;
  close (fd);
  if (chmod_result == 0)
    return chmod_result;
  if (chmod_errno != ENOENT)
    {
      errno = chmod_errno;
      return chmod_result;
    }
  /* /proc is not mounted.  */
# elif HAVE_LSTAT
  struct stat st;
  int lstat_result = lstat (file, &st);
  if (lstat_result != 0)
    return lstat_result;
  if (S_ISLNK (st.st_mode))
    {
      errno = EOPNOTSUPP;
      return -1;
    }
# endif

  /* Fall back on chmod, despite the race.  */
  return chmod (file, mode);
#endif
}
