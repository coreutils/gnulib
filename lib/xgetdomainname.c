/* xgetdomainname.c -- Return the NIS domain name, without size limitations.
   Copyright (C) 1992, 1996, 2000-2001, 2003-2004, 2006, 2008-2015 Free
   Software Foundation, Inc.

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

/* Based on xgethostname.c, written by Jim Meyering.  */

#include <config.h>

/* Specification.  */
#include "xgetdomainname.h"

/* Get getdomainname.  */
#include <unistd.h>

/* Get errno.  */
#include <errno.h>

/* Get free.  */
#include <stdlib.h>

#include "xalloc.h"

#ifndef INITIAL_DOMAINNAME_LENGTH
# define INITIAL_DOMAINNAME_LENGTH 34
#endif

/* Return the NIS domain name of the machine, in malloc'd storage.
   WARNING! The NIS domain name is unrelated to the fully qualified host name
            of the machine.  It is also unrelated to email addresses.
   WARNING! The NIS domain name is usually the empty string or "(none)" when
            not using NIS.
   If malloc fails, exit.
   Upon any other failure, set errno and return NULL.  */
char *
xgetdomainname (void)
{
  char *domainname;
  size_t size;

  size = INITIAL_DOMAINNAME_LENGTH;
  domainname = xmalloc (size);
  while (1)
    {
      int k = size - 1;
      int err;

      errno = 0;
      domainname[k] = '\0';
      err = getdomainname (domainname, size);
      if (err >= 0 && domainname[k] == '\0')
        break;
      else if (err < 0 && errno != EINVAL)
        {
          int saved_errno = errno;
          free (domainname);
          errno = saved_errno;
          return NULL;
        }
      size *= 2;
      domainname = xrealloc (domainname, size);
    }

  return domainname;
}
