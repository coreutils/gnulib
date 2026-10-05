/* Work around platform bugs in strftime.
   Copyright (C) 2017-2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <time.h>

#include "tzsanitize.h"

#undef strftime

size_t
rpl_strftime (char *buf, size_t bufsize, const char *format, const struct tm *tp)
{
#if defined _WIN32 && ! defined __CYGWIN__
  tzsanitize ();
#endif

  return strftime (buf, bufsize, format, tp);
}
