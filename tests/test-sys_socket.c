/* Test of <sys/socket.h> substitute.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <sys/socket.h>

#include <errno.h>

/* Check some integer constant expressions.  */
int a[] = { SHUT_RD, SHUT_WR, SHUT_RDWR };

int
main ()
{
  /* Check some errno values.  */
  switch (a[0])
    {
    case ENOTSOCK:
    case EADDRINUSE:
    case ENETRESET:
    case ECONNABORTED:
    case ECONNRESET:
    case ENOTCONN:
    case ESHUTDOWN:
      break;
    }

  return 0;
}
