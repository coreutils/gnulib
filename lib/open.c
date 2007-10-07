/* Open a descriptor to a file.
   Copyright (C) 2007 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

/* Specification.  */
#include <fcntl.h>

/* If the fchdir replacement is used, open() is defined in fchdir.c.  */
#ifndef FCHDIR_REPLACEMENT

# include <stdarg.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>

int
open (const char *filename, int flags, ...)
# undef open
{
  mode_t mode;

  mode = 0;
  if (flags & O_CREAT)
    {
      va_list arg;
      va_start (arg, flags);

      /* If mode_t is narrower than int, use the promoted type (int),
	 not mode_t.  Use sizeof to guess whether mode_t is narrower;
	 we don't know of any practical counterexamples.  */
      mode = (sizeof (mode_t) < sizeof (int)
	      ? va_arg (arg, int)
	      : va_arg (arg, mode_t));

      va_end (arg);
    }

# if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
  if (strcmp (filename, "/dev/null") == 0)
    filename = "NUL";
# endif

  return open (filename, flags, mode);
}
#endif
