/* Obtain a series of random bytes.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include <sys/random.h>

#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

#include "minmax.h"

/* Set BUFFER (of size LENGTH) to random bytes under the control of FLAGS.
   Return the number of bytes written, or -1 on error.  */
ssize_t
getrandom (void *buffer, size_t length, unsigned int flags)
#undef getrandom
{
#if HAVE_GETRANDOM
  return getrandom (buffer, length, flags);
#else
  static int randfd[2] = { -1, -1 };
  bool devrandom = (flags & GRND_RANDOM) != 0;
  int fd = randfd[devrandom];

  if (fd < 0)
    {
      static char const randdevice[][MAX (sizeof NAME_OF_NONCE_DEVICE,
                                          sizeof NAME_OF_RANDOM_DEVICE)]
        = { NAME_OF_NONCE_DEVICE, NAME_OF_RANDOM_DEVICE };
      int oflags = (O_RDONLY + O_CLOEXEC
                    + (flags & GRND_NONBLOCK ? O_NONBLOCK : 0));
      fd = open (randdevice[devrandom], oflags);
      if (fd < 0)
        return fd;
      randfd[devrandom] = fd;
    }

  return read (fd, buffer, length);
#endif
}
