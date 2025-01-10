/* Test of the <sys/un.h> substitute.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Collin Funk <collin.funk1@gmail.com>, 2024.  */

#include <config.h>

#if HAVE_UNIXSOCKET

/* Specification.  */
# include <sys/un.h>

# include "intprops.h"

/* POSIX requires that sa_family_t is an unsigned integer type.  */
static_assert (! TYPE_SIGNED (sa_family_t));

int
main (void)
{
  struct sockaddr_un sa;

  /* POSIX requires sun_path is 92 bytes or greater.  */
  static_assert (92 <= sizeof sa.sun_path);

  return 0;
}

#else

/* UNIX domain sockets unsupported.  */

# include <stdio.h>

int
main (void)
{
  fputs ("Skipping test: UNIX domain sockets not supported\n", stderr);
  return 77;
}

#endif
