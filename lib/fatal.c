/* Fatal exits for noninteractive utilities

   Copyright (C) 2001, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "fatal.h"

#include "exit.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _LIBC
# define program_name program_invocation_name
#else /* not _LIBC */
/* The calling program should define program_name and set it to the
   name of the executing program.  */
extern char *program_name;
#endif

#include "unlocked-io.h"

/* Like error, but always exit with EXIT_FAILURE.  */

void
fatal (int errnum, const char *message, ...)
{
  va_list args;

  if (error_print_progname)
    (*error_print_progname) ();
  else
    {
      fflush (stdout);
      fprintf (stderr, "%s: ", program_name);
    }

  va_start (args, message);
  error (EXIT_FAILURE, errnum, message, args);

  /* The following code isn't reachable, but pacifies some compilers.  */
  va_end (args);
  abort ();
}
