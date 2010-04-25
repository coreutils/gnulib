/* Determine name of a terminal.

   Copyright (C) 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include <unistd.h>

#include <errno.h>
#include <limits.h>
#include <string.h>

int
ttyname_r (int fd, char *buf, size_t buflen)
#undef ttyname_r
{
  /* When ttyname_r exists and works, use it.
     But on Solaris 10, ttyname_r is broken: it returns NULL in situations
     when ttyname finds the result.  */
#if HAVE_TTYNAME_R && !defined __sun
  /* This code is multithread-safe.  */
  char *name = ttyname_r (fd, buf, buflen <= INT_MAX ? buflen : INT_MAX);
  if (name == NULL)
    return errno;
  if (name != buf)
    {
      size_t namelen = strlen (name) + 1;
      if (namelen > buflen)
        return ERANGE;
      memmove (buf, name, namelen);
    }
  return 0;
#elif HAVE_TTYNAME
  /* Note: This is not multithread-safe.  */
  char *name;
  size_t namelen;

  name = ttyname (fd);
  if (name == NULL)
    return errno;
  namelen = strlen (name) + 1;
  if (namelen > buflen)
    return ERANGE;
  memcpy (buf, name, namelen);
  return 0;
#else
  /* Platforms like mingw: no ttys exist at all.  */
  return ENOTTY;
#endif
}
