/* Provide a non-threads replacement for the POSIX raise function.

   Copyright (C) 2002-2003, 2005-2006, 2009-2011 Free Software Foundation, Inc.

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

/* written by Jim Meyering and Bruno Haible */

#include <config.h>

/* Specification.  */
#include <signal.h>

#if HAVE_RAISE
/* Native Windows platform.  */

# include <errno.h>

# include "msvc-inval.h"

# undef raise

# if HAVE_MSVC_INVALID_PARAMETER_HANDLER
static inline int
raise_nothrow (int sig)
{
  int result;

  TRY_MSVC_INVAL
    {
      result = raise (sig);
    }
  CATCH_MSVC_INVAL
    {
      result = -1;
      errno = EINVAL;
    }
  DONE_MSVC_INVAL;

  return result;
}
#  define raise raise_nothrow
# endif

int
rpl_raise (int sig)
{
  return raise_nothrow (sig);
}

#else
/* An old Unix platform.  */

# include <unistd.h>

int
raise (int sig)
{
  return kill (getpid (), sig);
}

#endif
