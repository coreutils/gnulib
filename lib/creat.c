/* Create a file.
   Copyright (C) 2007-2019 Free Software Foundation, Inc.

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

/* If the user's config.h happens to include <fcntl.h>, let it include only
   the system's <fcntl.h> here, so that orig_creat doesn't recurse to
   rpl_creat.  */
#define __need_system_fcntl_h
#include <config.h>

/* Get the original definition of creat.  It might be defined as a macro.  */
#include <fcntl.h>
#include <sys/types.h>
#undef __need_system_fcntl_h

static int
orig_creat (const char *filename, mode_t mode)
{
  return creat (filename, mode);
}

/* Specification.  */
/* Write "fcntl.h" here, not <fcntl.h>, otherwise OSF/1 5.1 DTK cc eliminates
   this include because of the preliminary #include <fcntl.h> above.  */
#include "fcntl.h"

#include <errno.h>
#include <string.h>
#include <sys/types.h>

int
creat (const char *filename, mode_t mode)
{
#if OPEN_TRAILING_SLASH_BUG
  /* If the filename ends in a slash, then fail.
     Rationale: POSIX <http://www.opengroup.org/susv3/basedefs/xbd_chap04.html>
     says that
       "A pathname that contains at least one non-slash character and that
        ends with one or more trailing slashes shall be resolved as if a
        single dot character ( '.' ) were appended to the pathname."
     and
       "The special filename dot shall refer to the directory specified by
        its predecessor."
     creat() is defined as being equivalent to open() with flags
     O_CREAT | O_TRUNC | O_WRONLY.  Therefore:
     If the named file already exists as a directory, then creat() must fail
     with errno = EISDIR.
     If the named file does not exist or does not name a directory, then
     creat() must fail since creat() cannot create directories.  */
  {
    size_t len = strlen (filename);
    if (len > 0 && filename[len - 1] == '/')
      {
        errno = EISDIR;
        return -1;
      }
  }
#endif

#if defined _WIN32 && !defined __CYGWIN__
  /* Remap the 'x' bits to the 'r' bits.  */
  mode = (mode & ~0111) | ((mode & 0111) << 2);
#endif

  return orig_creat (filename, mode);
}
