/* truncate emulations for native Windows.
   Copyright (C) 2017-2020 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>

int
truncate (const char *filename, off_t length)
{
  int fd;

  if (length == 0)
    {
      fd = open (filename, O_WRONLY | O_TRUNC);
      if (fd < 0)
        return -1;
    }
  else
    {
      fd = open (filename, O_WRONLY);
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
}
