/* printf wrappers that fail immediately for non-file-related errors
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "xprintf.h"

#include <errno.h>
#include <stdarg.h>

#include "error.h"
#include "exitfail.h"
#include "gettext.h"

/* written by Jim Meyering */

/* Just like printf, but call error if it fails for any reason
   for which printf does not set the stream error indicator.  */
int
xprintf (char const *restrict format, ...)
{
  va_list args;
  va_start (args, format);
  int err = vprintf (format, args);
  if (err && (errno == EILSEQ || errno == EINVAL || errno == ENOMEM))
    error (exit_failure, errno, gettext ("write error"));

  return err;
}

/* Just like fprintf, but call error if it fails for any reason
   for which printf does not set the stream error indicator.  */
int
xfprintf (FILE *restrict stream, char const *restrict format, ...)
{
  va_list args;
  va_start (args, format);
  int err = vfprintf (stream, format, args);
  if (err && (errno == EILSEQ || errno == EINVAL || errno == ENOMEM))
    error (exit_failure, errno, gettext ("write error"));

  return err;
}
