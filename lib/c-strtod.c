/* Convert string to double, using the C locale.

   Copyright (C) 2003-2004, 2006, 2009-2022 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include "c-strtod.h"

#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#if LONG
# define C_STRTOD c_strtold
# define DOUBLE long double
# define STRTOD_L strtold_l
# define HAVE_GOOD_STRTOD_L (HAVE_STRTOLD_L && !GNULIB_defined_strtold_function)
# define STRTOD strtold
#else
# define C_STRTOD c_strtod
# define DOUBLE double
# define STRTOD_L strtod_l
# define HAVE_GOOD_STRTOD_L (HAVE_STRTOD_L && !GNULIB_defined_strtod_function)
# define STRTOD strtod
#endif

#if defined LC_ALL_MASK \
    && ((LONG ? HAVE_GOOD_STRTOLD_L : HAVE_GOOD_STRTOD_L) \
        || HAVE_WORKING_USELOCALE)

/* Cache for the C locale object.
   Marked volatile so that different threads see the same value
   (avoids locking).  */
static volatile locale_t c_locale_cache;

/* Return the C locale object, or (locale_t) 0 with errno set
   if it cannot be created.  */
static locale_t
c_locale (void)
{
  if (!c_locale_cache)
    c_locale_cache = newlocale (LC_ALL_MASK, "C", (locale_t) 0);
  return c_locale_cache;
}

#endif

DOUBLE
C_STRTOD (char const *nptr, char **endptr)
{
  DOUBLE r;

#if defined LC_ALL_MASK \
    && ((LONG ? HAVE_GOOD_STRTOLD_L : HAVE_GOOD_STRTOD_L) \
        || HAVE_WORKING_USELOCALE)

  locale_t locale = c_locale ();
  if (!locale)
    {
      if (endptr)
        *endptr = (char *) nptr;
      return 0; /* errno is set here */
    }

# if (LONG ? HAVE_GOOD_STRTOLD_L : HAVE_GOOD_STRTOD_L)

  r = STRTOD_L (nptr, endptr, locale);

# else /* HAVE_WORKING_USELOCALE */

  locale_t old_locale = uselocale (locale);
  if (old_locale == (locale_t)0)
    {
      if (endptr)
        *endptr = (char *) nptr;
      return 0; /* errno is set here */
    }

  r = STRTOD (nptr, endptr);

  int saved_errno = errno;
  if (uselocale (old_locale) == (locale_t)0)
    /* We can't switch back to the old locale.  The thread is hosed.  */
    abort ();
  errno = saved_errno;

# endif

#else

  char *saved_locale = setlocale (LC_NUMERIC, NULL);

  if (saved_locale)
    {
      saved_locale = strdup (saved_locale);
      if (saved_locale == NULL)
        {
          if (endptr)
            *endptr = (char *) nptr;
          return 0; /* errno is set here */
        }
      setlocale (LC_NUMERIC, "C");
    }

  r = STRTOD (nptr, endptr);

  if (saved_locale)
    {
      int saved_errno = errno;

      setlocale (LC_NUMERIC, saved_locale);
      free (saved_locale);
      errno = saved_errno;
    }

#endif

  return r;
}
