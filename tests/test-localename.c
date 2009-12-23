/* Test of gl_locale_name function and its variants.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include "localename.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) \
  do                                                                         \
    {                                                                        \
      if (!(expr))                                                           \
        {                                                                    \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          fflush (stderr);                                                   \
          abort ();                                                          \
        }                                                                    \
    }                                                                        \
  while (0)

/* Test the gl_locale_name() function.  */
static void
test_locale_name (void)
{
  const char *name;

  /* Check that gl_locale_name returns non-NULL.  */
  ASSERT (gl_locale_name (LC_MESSAGES, "LC_MESSAGES") != NULL);

  /* Get into a defined state,  */
  setlocale (LC_ALL, "en_US.UTF-8");
#if HAVE_NEWLOCALE
  uselocale (LC_GLOBAL_LOCALE);
#endif

  /* Check that when all environment variables are unset,
     gl_locale_name returns the default locale.  */
  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LC_NUMERIC");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"),
                  gl_locale_name_default ()) == 0);
  ASSERT (strcmp (gl_locale_name (LC_NUMERIC, "LC_NUMERIC"),
                  gl_locale_name_default ()) == 0);

  /* Check that an empty environment variable is treated like an unset
     environment variable.  */

  setenv ("LC_ALL", "", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"),
                  gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "", 1);
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"),
                  gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "", 1);
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"),
                  gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "", 1);
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"),
                  gl_locale_name_default ()) == 0);

  /* Check that LC_ALL overrides the others, and LANG is overridden by the
     others.  */

  setenv ("LC_ALL", "C", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "C", 1);
  setenv ("LC_MESSAGES", "C", 1);
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "C", 1);
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  /* Check mixed situations.  */

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  setenv ("LANG", "de_DE.UTF-8", 1);
  if (setlocale (LC_ALL, "") != NULL)
    {
      name = gl_locale_name (LC_CTYPE, "LC_CTYPE");
      ASSERT (strcmp (name, "de_DE.UTF-8") == 0);
      name = gl_locale_name (LC_MESSAGES, "LC_MESSAGES");
      ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);
    }

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  unsetenv ("LANG");
  if (setlocale (LC_ALL, "") != NULL)
    {
      name = gl_locale_name (LC_CTYPE, "LC_CTYPE");
      ASSERT (strcmp (name, gl_locale_name_default ()) == 0);
      name = gl_locale_name (LC_MESSAGES, "LC_MESSAGES");
      ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);
    }
}

/* Test the gl_locale_name_posix() function.  */
static void
test_locale_name_posix (void)
{
  const char *name;

  /* Get into a defined state,  */
  setlocale (LC_ALL, "en_US.UTF-8");
#if HAVE_NEWLOCALE
  uselocale (LC_GLOBAL_LOCALE);
#endif

  /* Check that when all environment variables are unset,
     gl_locale_name_posix returns either NULL or the default locale.  */
  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LC_NUMERIC");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);
  name = gl_locale_name_posix (LC_NUMERIC, "LC_NUMERIC");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);

  /* Check that an empty environment variable is treated like an unset
     environment variable.  */

  setenv ("LC_ALL", "", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "", 1);
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "", 1);
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "", 1);
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);

  /* Check that LC_ALL overrides the others, and LANG is overridden by the
     others.  */

  setenv ("LC_ALL", "C", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "C", 1);
  setenv ("LC_MESSAGES", "C", 1);
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "C", 1);
  setlocale (LC_ALL, "");
  name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  /* Check mixed situations.  */

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  setenv ("LANG", "de_DE.UTF-8", 1);
  if (setlocale (LC_ALL, "") != NULL)
    {
      name = gl_locale_name_posix (LC_CTYPE, "LC_CTYPE");
      ASSERT (strcmp (name, "de_DE.UTF-8") == 0);
      name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
      ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);
    }

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  unsetenv ("LANG");
  if (setlocale (LC_ALL, "") != NULL)
    {
      name = gl_locale_name_posix (LC_CTYPE, "LC_CTYPE");
      ASSERT (name == NULL || strcmp (name, gl_locale_name_default ()) == 0);
      name = gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES");
      ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);
    }
}

/* Test the gl_locale_name_environ() function.  */
static void
test_locale_name_environ (void)
{
  const char *name;

  /* Get into a defined state,  */
  setlocale (LC_ALL, "en_US.UTF-8");
#if HAVE_NEWLOCALE
  uselocale (LC_GLOBAL_LOCALE);
#endif

  /* Check that when all environment variables are unset,
     gl_locale_name_environ returns NULL.  */
  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LC_NUMERIC");
  unsetenv ("LANG");
  ASSERT (gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES") == NULL);
  ASSERT (gl_locale_name_environ (LC_NUMERIC, "LC_NUMERIC") == NULL);

  /* Check that an empty environment variable is treated like an unset
     environment variable.  */

  setenv ("LC_ALL", "", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  ASSERT (gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES") == NULL);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "", 1);
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  ASSERT (gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES") == NULL);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "", 1);
  unsetenv ("LANG");
  ASSERT (gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES") == NULL);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "", 1);
  ASSERT (gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES") == NULL);

  /* Check that LC_ALL overrides the others, and LANG is overridden by the
     others.  */

  setenv ("LC_ALL", "C", 1);
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  unsetenv ("LANG");
  name = gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "C", 1);
  setenv ("LC_MESSAGES", "C", 1);
  unsetenv ("LANG");
  name = gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "C", 1);
  name = gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "C") == 0);

  /* Check mixed situations.  */

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  setenv ("LANG", "de_DE.UTF-8", 1);
  name = gl_locale_name_environ (LC_CTYPE, "LC_CTYPE");
  ASSERT (strcmp (name, "de_DE.UTF-8") == 0);
  name = gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  setenv ("LC_MESSAGES", "fr_FR.UTF-8", 1);
  unsetenv ("LANG");
  name = gl_locale_name_environ (LC_CTYPE, "LC_CTYPE");
  ASSERT (name == NULL);
  name = gl_locale_name_environ (LC_MESSAGES, "LC_MESSAGES");
  ASSERT (strcmp (name, "fr_FR.UTF-8") == 0);
}

/* Test the gl_locale_name_default() function.  */
static void
test_locale_name_default (void)
{
  const char *name = gl_locale_name_default ();

  ASSERT (name != NULL);

  /* Only MacOS X and Windows have a facility for the user to set the default
     locale.  */
#if !((defined __APPLE__ && defined __MACH__) || (defined _WIN32 || defined __WIN32__ || defined __CYGWIN__))
  ASSERT (strcmp (name, "C") == 0);
#endif
}

int
main ()
{
  test_locale_name ();
  test_locale_name_posix ();
  test_locale_name_environ ();
  test_locale_name_default ();

  return 0;
}
