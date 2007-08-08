/* A more useful interface to strtol.

   Copyright (C) 1995, 1996, 1998, 1999, 2001, 2002, 2003, 2004, 2006, 2007
   Free Software Foundation, Inc.

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

#include <config.h>
#include "xstrtol.h"

#include <stdlib.h>

#include "error.h"
#include "gettext.h"

#define _(str) gettext (str)

/* Report an error for an out-of-range integer argument.
   EXIT_CODE is the exit code (0 for a non-fatal error).
   OPTION is the option that takes the argument
    (usually starting with one or two minus signs).
   ARG is the option's argument.
   ERR is the error code returned by one of the xstrto* functions.  */
void
xstrtol_error (int exit_code, char const *option, char const *arg,
	       strtol_error err)
{
  switch (err)
    {
    default:
      abort ();

    case LONGINT_INVALID:
      error (exit_code, 0, _("invalid %s argument `%s'"),
	     option, arg);
      break;

    case LONGINT_INVALID_SUFFIX_CHAR:
    case LONGINT_INVALID_SUFFIX_CHAR | LONGINT_OVERFLOW:
      error (exit_code, 0, _("invalid suffix in %s argument `%s'"),
	     option, arg);
      break;

    case LONGINT_OVERFLOW:
      error (exit_code, 0, _("%s argument `%s' too large"),
	     option, arg);
      break;
    }
}
