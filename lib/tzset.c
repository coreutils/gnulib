/* Provide tzset for systems that don't have it or for which it's broken.

   Copyright (C) 2001-2003, 2005-2007, 2009-2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

#include <config.h>

/* Specification.  */
#include <time.h>

#include "tzsanitize.h"

void
rpl_tzset (void)
#undef tzset
{
#if defined _WIN32 && ! defined __CYGWIN__
  tzsanitize ();

  /* On native Windows, tzset() is deprecated.  Use _tzset() instead.  See
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/posix-tzset>
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/tzset>  */
  _tzset ();
#else
  tzset ();
#endif
}
