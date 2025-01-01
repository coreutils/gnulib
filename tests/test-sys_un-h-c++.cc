/* Test of the <sys/un.h> substitute in C++ mode.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Collin Funk <collin.funk1@gmail.com>, 2024.  */

#define GNULIB_NAMESPACE gnulib
#include <config.h>

#if HAVE_UNIXSOCKET

# include <sys/un.h>

int
main ()
{
}

#else

/* UNIX domain sockets unsupported.  */

# include <iostream>

int
main ()
{
  std::cout << "Skipping test: UNIX domain sockets not supported" << std::endl;
  return 77;
}

#endif
