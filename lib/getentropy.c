/* Fill a buffer with random bytes.
   Copyright 2020-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

#include <config.h>

#include <unistd.h>

#include <errno.h>
#include <sys/random.h>

/* Overwrite BUFFER with random data.  BUFFER contains LENGTH bytes.
   LENGTH must be at most 256.  Return 0 on success, -1 (setting
   errno) on failure.  */
int
getentropy (void *buffer, size_t length)
{
  char *buf = buffer;

  if (length <= 256)
    for (;;)
      {
        ssize_t bytes;
        if (length == 0)
          return 0;
        while ((bytes = getrandom (buf, length, 0)) < 0)
          if (errno != EINTR)
            return -1;
        if (bytes == 0)
          break;
        buf += bytes;
        length -= bytes;
      }

  errno = EIO;
  return -1;
}
