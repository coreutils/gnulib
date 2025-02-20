/* Internationalized description of system error code.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <string.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "at-init.h"
#include "errno-iter.h"
#include "glthread/tls.h"
#include "glthread/once.h"

/* Minimum and maximum valid errno value (other than 0).  */
static int errno_min;
static int errno_max;

/* Array of descriptions of system error codes in the "C" locale.
   Note: Such a linear array would not work
   - on GNU/Hurd, where errno_min = -309, errno_max = 1073741945,
   - on Haiku, where errno_min = -2147483648, errno_max = 2016.  */
static char **c_error_strings;

/* We need to initialize them before main() is entered, because main() often
   contains a call to setlocale().  */
AT_INIT (init_c_error_strings);
#ifdef __SUNPRO_C
# pragma init (init_c_error_strings)
#endif

static int
errno_minmax_callback (void *data, int err)
{
  if (err < errno_min)
    errno_min = err;
  if (err > errno_max)
    errno_max = err;
  return 0;
}

static int
errno_string_callback (void *data, int err)
{
  char *s = strerror (err);
  if (s != NULL && *s != '\0')
    c_error_strings[err - errno_min] = strdup (s);
  return 0;
}

/* Initializes errno_min, errno_max, c_error_strings.  */
static void
init_c_error_strings (void)
{
  /* Compute errno_min, errno_max.  */
  errno_min = errno_max = EDOM;
  errno_iterate (errno_minmax_callback, NULL);

#if 0 /* for debugging */
  {
    char buf[80];
    sprintf (buf, "min.errno = %d\nmax.errno = %d\n", errno_min, errno_max);
    write (STDOUT_FILENO, buf, strlen (buf));
  }
#endif

  /* Allocate memory for c_error_strings.  */
  c_error_strings =
    (char **) calloc (errno_max - errno_min + 1, sizeof (char *));
  /* Fetch the descriptions of system error codes.  */
  if (c_error_strings != NULL)
    errno_iterate (errno_string_callback, NULL);
}

#if GNULIB_defined_locale_t

/* Copy as much of MSG into BUF as possible, without corrupting errno.
   Return 0 if MSG fit in BUFLEN, otherwise return ERANGE.  */
static int
safe_copy (char *buf, size_t buflen, const char *msg)
{
  size_t len = strlen (msg);
  size_t moved = len < buflen ? len : buflen - 1;

  /* Although POSIX lets memmove corrupt errno, we don't
     know of any implementation where this is a real problem.  */
  memmove (buf, msg, moved);
  buf[moved] = '\0';
  return len < buflen ? 0 : ERANGE;
}

#endif

int
strerror_l_r (int errnum, char *buf, size_t buflen, locale_t locale)
{
  int ret;
  int saved_errno = errno;

#if GNULIB_defined_locale_t

  struct gl_locale_category_t *plc =
    &locale->category[gl_log2_lcmask_to_index (gl_log2_lc_mask (LC_MESSAGES))];
  if (plc->is_c_locale)
    {
      /* Implementation for the "C" locale.  */
      char stackbuf[80];
      char *s;
      if (errnum == 0)
        s = "No error";
      else
        {
          if (errnum >= errno_min && errnum <= errno_max
              && c_error_strings != NULL)
            s = c_error_strings[errnum - errno_min];
          else
            s = NULL;
          if (s == NULL)
            {
# if defined __HAIKU__
              /* For consistency with perror().  */
              sprintf (stackbuf, "Unknown Application Error (%d)", errnum);
# else
              sprintf (stackbuf, "Unknown error %d", errnum);
# endif
              s = stackbuf;
            }
        }
      ret = safe_copy (buf, buflen, s);
    }
  else
    {
      /* Implementation for the global locale.  */
# if HAVE_WORKING_USELOCALE
      locale_t saved_locale = uselocale (LC_GLOBAL_LOCALE);
# endif
      ret = strerror_r (errnum, buf, buflen);
# if HAVE_WORKING_USELOCALE
      uselocale (saved_locale);
# endif
    }

#else

# if HAVE_WORKING_USELOCALE
  locale_t saved_locale = uselocale (locale);
# endif
  ret = strerror_r (errnum, buf, buflen);
# if HAVE_WORKING_USELOCALE
  uselocale (saved_locale);
# endif

#endif

  if (ret == 0)
    errno = saved_errno;
  return ret;
}

/* Maximum supported size of an error code description.  */
#define RESULT_MAX_SIZE 200

static gl_tls_key_t result_key; /* TLS key for a 'char *' */

static void
key_init (void)
{
  gl_tls_key_init (result_key, free);
  /* The per-thread initial value is NULL.  */
}

/* Ensure that key_init is called once only.  */
gl_once_define(static, key_init_once)

char *
strerror_l (int errnum, locale_t locale)
{
  int saved_errno = errno;

  /* Call strerror_l_r, returning the string in thread-local storage.  */
  gl_once (key_init_once, key_init);
  char *result = gl_tls_get (result_key);
  if (result == NULL)
    {
      result = malloc (RESULT_MAX_SIZE);
      if (result == NULL)
        {
          errno = ENOMEM;
          return NULL;
        }
      gl_tls_set (result_key, result);
    }

  int ret = strerror_l_r (errnum, result, RESULT_MAX_SIZE, locale);
  if (ret == 0)
    {
      errno = saved_errno;
      return result;
    }
  else
    return NULL;
}
