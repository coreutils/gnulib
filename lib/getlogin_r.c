/* Provide a working getlogin_r for systems which lack it.

   Copyright (C) 2005 Free Software Foundation, Inc.

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

/* written by Paul Eggert and Derek Price */

#include <config.h>

#include "getlogin_r.h"

#include <errno.h>
#include <string.h>

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if !HAVE_DECL_GETLOGIN
char *getlogin (void);
#endif

/* See getlogin_r.h for documentation.  */
int
getlogin_r (char *name, size_t size)
{
  char *n;
  int save_errno = errno;

  errno = 0;
  n = getlogin ();
  if (n)
    {
      size_t nlen = strlen (n);
      if (nlen < size)
        {
          memcpy (name, n, nlen + 1);
          return 0;
        }
      errno = ERANGE;
    }

  if (errno) return errno;
  errno = save_errno;
  return -1;
}
