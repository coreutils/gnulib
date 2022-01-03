/* truncate emulations for native Windows.
   Copyright (C) 2017-2022 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* If the user's config.h happens to include <unistd.h>, let it include only
   the system's <unistd.h> here, so that orig_faccessat doesn't recurse to
   rpl_faccessat.  */
#define _GL_INCLUDING_UNISTD_H
#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#undef _GL_INCLUDING_UNISTD_H

#if TRUNCATE_TRAILING_SLASH_BUG
static int
orig_truncate (const char *filename, off_t length)
{
  return truncate (filename, length);
}
#endif

/* Write "unistd.h" here, not <unistd.h>, otherwise OSF/1 5.1 DTK cc
   eliminates this include because of the preliminary #include <unistd.h>
   above.  */
#include "unistd.h"

int
truncate (const char *filename, off_t length)
{
#if TRUNCATE_TRAILING_SLASH_BUG
  /* Use the original truncate(), but correct the trailing slash handling.  */
  size_t len = strlen (filename);
  if (len && filename[len - 1] == '/')
    {
      struct stat st;
      if (stat (filename, &st) == 0)
        errno = (S_ISDIR (st.st_mode) ? EISDIR : ENOTDIR);
      return -1;
    }
  return orig_truncate (filename, length);
#else
  int fd;

  if (length == 0)
    {
      fd = open (filename, O_WRONLY | O_TRUNC | O_CLOEXEC);
      if (fd < 0)
        return -1;
    }
  else
    {
      fd = open (filename, O_WRONLY | O_CLOEXEC);
      if (fd < 0)
        return -1;
      if (ftruncate (fd, length) < 0)
        {
          int saved_errno = errno;
          close (fd);
          errno = saved_errno;
          return -1;
        }
    }
  close (fd);
  return 0;
#endif
}
