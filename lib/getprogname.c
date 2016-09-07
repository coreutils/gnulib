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
#include <stdlib.h>
#include <string.h>


#ifndef HAVE_GETPROGNAME
const char *
getprogname (void)
{
#if HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME
  return program_invocation_short_name;
#elif HAVE_DECL_PROGRAM_INVOCATION_NAME || HAVE_GETEXECNAME

  const char *slash;
# if HAVE_DECL_PROGRAM_INVOCATION_NAME
  const char *base = program_invocation_name;
# else
  const char *base = getexecname ();
  if (!base)
    base = "?";
# endif

  slash = strrchr (base, '/');
  if (slash != NULL)
    base = slash + 1;

  return base;
#else
 #error "getprogname module not ported to this OS"
#endif
}
#endif
