/* Work around an fstatat bug on Solaris 9.

   Copyright (C) 2006, 2009 Free Software Foundation, Inc.

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

/* Written by Paul Eggert and Jim Meyering.  */

#include <config.h>

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#undef fstatat

/* fstatat should always follow symbolic links that end in /, but on
   Solaris 9 it doesn't if AT_SYMLINK_NOFOLLOW is specified.
   Likewise, trailing slash on a non-directory should be an error.
   These are the same problems that lstat.c and stat.c address, so
   solve it in a similar way.  */

int
rpl_fstatat (int fd, char const *file, struct stat *st, int flag)
{
  int result = fstatat (fd, file, st, flag);
  size_t len;

  if (result != 0)
    return result;
  len = strlen (file);
  if (flag & AT_SYMLINK_NOFOLLOW)
    {
      /* Fix lstat behavior.  */
      if (file[len - 1] != '/' || S_ISDIR (st->st_mode))
	return 0;
      if (!S_ISLNK (st->st_mode))
	{
	  errno = ENOTDIR;
	  return -1;
	}
      result = fstatat (fd, file, st, flag & ~AT_SYMLINK_NOFOLLOW);
    }
  /* Fix stat behavior.  */
  if (result == 0 && !S_ISDIR (st->st_mode) && file[len - 1] == '/')
    {
      errno = ENOTDIR;
      return -1;
    }
  return result;
}
