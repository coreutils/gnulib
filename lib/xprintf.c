/* printf wrappers that fail immediately for non-file-related errors
   Copyright (C) 2007, 2009-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

#include <config.h>

#include "xprintf.h"

#include <errno.h>

#include <error.h>
#include "exitfail.h"
#include "gettext.h"

#define _(msgid) dgettext (GNULIB_TEXT_DOMAIN, msgid)

off64_t
xprintf (char const *restrict format, ...)
{
  va_list args;
  off64_t retval;
  va_start (args, format);
  retval = xvprintf (format, args);
  va_end (args);

  return retval;
}

off64_t
xvprintf (char const *restrict format, va_list args)
{
  off64_t retval = vzprintf (format, args);
  if (retval < 0 && ! ferror (stdout))
    error (exit_failure, errno, _("cannot perform formatted output"));

  return retval;
}

off64_t
xfprintf (FILE *restrict stream, char const *restrict format, ...)
{
  va_list args;
  off64_t retval;
  va_start (args, format);
  retval = xvfprintf (stream, format, args);
  va_end (args);

  return retval;
}

off64_t
xvfprintf (FILE *restrict stream, char const *restrict format, va_list args)
{
  off64_t retval = vfzprintf (stream, format, args);
  if (retval < 0 && ! ferror (stream))
    error (exit_failure, errno, _("cannot perform formatted output"));

  return retval;
}
