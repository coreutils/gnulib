/* Open the master side of a pseudo-terminal.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <fcntl.h>
#include <errno.h>

int
posix_openpt (int flags)
{
  int master;

#ifdef _AIX /* AIX */

  master = open ("/dev/ptc", flags);

#elif (defined _WIN32 || defined __WIN32__) && !defined __CYGWIN__ /* mingw */

  /* Mingw lacks pseudo-terminals altogether.  */
  master = -1;
  errno = ENOSYS;

#else /* MacOS, OpenBSD, HP-UX, IRIX, Solaris 9, Cygwin 1.5 */

  /* Most systems that lack posix_openpt() have /dev/ptmx.  */
  master = open ("/dev/ptmx", flags);

#endif

  return master;
}

#endif
