/* sethostname emulation for glibc compliance.

   Copyright (C) 2011 Free Software Foundation, Inc.

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

/* Ben Walton <bwalton@artsci.utoronto.ca> */

#include <config.h>

/* Unix API.  */

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/* Set up to LEN chars of NAME as system hostname.
   Return 0 if ok, set errno and return -1 on error. */

int
sethostname (const char *name, size_t len)
{
  /* Ensure the string isn't too long.  glibc does allow setting an
     empty hostname so no point in enforcing a lower bound. */
  if (len > HOST_NAME_MAX)
    {
      errno = EINVAL;
      return -1;
    }

  /* NAME does not need to be null terminated so leave room to terminate
     regardless of input. */
  char hostname[HOST_NAME_MAX + 1];
  memcpy ((void *) hostname, (const void *) name, len);
  hostname[len] = '\0';

#ifdef __minix /* Minix */
  {
    FILE *hostf;
    int r = 0;

    /* glibc returns EFAULT, EINVAL, and EPERM on error.  None of
       these are appropriate for us to set, even if they may match the
       situation, during failed open/write/close operations, so we
       leave errno alone and rely on what the system sets up. */
    hostf = fopen ("/etc/hostname.file", "w");
    if (hostf == NULL)
      r = -1;
    else
      {
        fprintf (hostf, "%s\n", hostname);
        if (ferror (hostf))
          {
            clearerr (hostf);
            r = -1;
          }

        /* use return value of fclose for function return value as it
           matches our needs.  fclose will also set errno on
           failure */
        r = fclose (hostf);
      }

    return r;
  }
#else
  /* For platforms that we don't have a better option for, simply bail
     out */
  errno = ENOSYS;
  return -1;
#endif
}
