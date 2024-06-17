/* Time zone internal functions

   Copyright 2015-2024 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include <time.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "flexmember.h"
#include "time-internal.h"

/* Get and set the TZ environment variable.  These functions can be
   overridden by programs like Emacs that manage their own environment.  */

#ifndef getenv_TZ
static char *
getenv_TZ (void)
{
  return getenv ("TZ");
}
#endif

#ifndef setenv_TZ
static int
setenv_TZ (char const *tz)
{
  return tz ? setenv ("TZ", tz, 1) : unsetenv ("TZ");
}
#endif

#if !HAVE_TIMEZONE_T
static const char *
tzgetname (timezone_t tz, int isdst)
{
  return tz->tz_is_set ? tz->abbrs : NULL;
}
#endif

/* Change the environment to match the specified timezone_t value.
   Return true if successful, false (setting errno) otherwise.  */
static bool
change_env (timezone_t tz)
{
  if (setenv_TZ (tzgetname (tz, 0)) != 0)
    return false;
  tzset ();
  return true;
}

/* Temporarily set the time zone to TZ, which must not be null.
   Return LOCAL_TZ if the time zone setting is already correct.
   Otherwise return a newly allocated time zone representing the old
   setting, or NULL (setting errno) on failure.  */
timezone_t
set_tz (timezone_t tz)
{
  char *env_tz = getenv_TZ ();
  const char *tz_name = tzgetname (tz, 0);
  if (env_tz
      ? tz_name != NULL && strcmp (tz_name, env_tz) == 0
      : tz_name == NULL)
    return local_tz;
  else
    {
      timezone_t old_tz = tzalloc (env_tz);
      if (!old_tz)
        return old_tz;
      if (! change_env (tz))
        {
          int saved_errno = errno;
          tzfree (old_tz);
          errno = saved_errno;
          return NULL;
        }
      return old_tz;
    }
}

/* Restore an old setting returned by set_tz.  It must not be null.
   Return true (preserving errno) if successful, false (setting errno)
   otherwise.  */
bool
revert_tz (timezone_t tz)
{
  if (tz == local_tz)
    return true;
  else
    {
      int saved_errno = errno;
      bool ok = change_env (tz);
      if (!ok)
        saved_errno = errno;
      tzfree (tz);
      errno = saved_errno;
      return ok;
    }
}
