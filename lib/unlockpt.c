/* Unlock the slave side of a pseudo-terminal from its master side.
   Copyright (C) 1998, 2010-2022 Free Software Foundation, Inc.
   Contributed by Zack Weinberg <zack@rabi.phys.columbia.edu>, 1998.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

int
unlockpt (int fd)
{
  /* Platforms which have the TIOCSPTLCK ioctl (Linux) already have the
     unlockpt function.  */
#if HAVE_REVOKE
  /* Mac OS X 10.3, OpenBSD 3.8 do not have the unlockpt function, but they
     have revoke().  */
  char *name = ptsname (fd);
  if (name == NULL)
    return -1;
  return revoke (name);
#else
  /* Assume that the slave side of a pseudo-terminal is already unlocked
     by default.  */
  if (fcntl (fd, F_GETFD) < 0)
    return -1;
  return 0;
#endif
}
