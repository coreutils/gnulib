/* much like chdir(2), but safer

   Copyright (C) 2005-2006, 2008-2016 Free Software Foundation, Inc.

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

/* written by Jim Meyering */

#include <config.h>

#include "chdir-safer.h"

#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "same-inode.h"

#ifndef HAVE_READLINK
# define HAVE_READLINK 0
#endif

/* Like chdir, but fail if DIR is a symbolic link to a directory (or
   similar funny business).  This avoids a minor race condition
   between when a directory is created or statted and when the process
   chdirs into it.

   On older systems lacking full support for O_SEARCH, this function
   can also fail if DIR is not readable.  */
int
chdir_no_follow (char const *dir)
{
  int result = 0;
  int saved_errno;
  int fd = open (dir,
                 O_SEARCH | O_DIRECTORY | O_NOCTTY | O_NOFOLLOW | O_NONBLOCK);
  if (fd < 0)
    return -1;

  /* If open follows symlinks, lstat DIR and fstat FD to ensure that
     they are the same file; if they are different files, set errno to
     ELOOP (the same value that open uses for symlinks with
     O_NOFOLLOW) so the caller can report a failure.
     Skip this check if HAVE_READLINK == 0, which should be the case
     on any system that lacks symlink support.  */
  if (HAVE_READLINK && ! HAVE_WORKING_O_NOFOLLOW)
    {
      struct stat sb1;
      result = lstat (dir, &sb1);
      if (result == 0)
        {
          struct stat sb2;
          result = fstat (fd, &sb2);
          if (result == 0 && ! SAME_INODE (sb1, sb2))
            {
              errno = ELOOP;
              result = -1;
            }
        }
    }

  if (result == 0)
    result = fchdir (fd);

  saved_errno = errno;
  close (fd);
  errno = saved_errno;
  return result;
}
