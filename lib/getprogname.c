/* Program name management.
   Copyright (C) 2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "getprogname.h"

#include <errno.h> /* get program_invocation_name declaration */
#include <stdlib.h> /* get __argv declaration */

#ifdef _AIX
# include <unistd.h>
# include <procinfo.h>
# include <string.h>
#endif

#include "dirname.h"

#ifndef HAVE_GETPROGNAME

char const *
getprogname (void)
{
# if HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME
  return program_invocation_short_name;
# elif HAVE_DECL_PROGRAM_INVOCATION_NAME
  return last_component (program_invocation_name);
# elif HAVE_GETEXECNAME
  const char *p = getexecname ();
  if (!p)
    p = "?";
  return last_component (p);
# elif HAVE_DECL___ARGV
  const char *p = __argv && __argv[0] ? __argv[0] : "?";
  return last_component (p);
# elif _AIX
  /* Idea by Bastien ROUCARIÈS <address@hidden>,
     http://lists.gnu.org/archive/html/bug-gnulib/2010-12/msg00095.html
     Reference: http://
   ibm.biz/knowctr#ssw_aix_53/com.ibm.aix.basetechref/doc/basetrf1/getprocs.htm
  */
  static char *p;
  static int first = 1;
  if (first)
    {
      first = 0;
      pid_t pid = getpid ();
      struct procentry64 procs;
      p = (0 < getprocs64 (&procs, sizeof procs, NULL, 0, &pid, 1)
           ? strdup (procs.pi_comm)
           : NULL);
      if (!p)
        p = "?";
    }
  return p;
# else
#  error "getprogname module not ported to this OS"
# endif
}

#endif
