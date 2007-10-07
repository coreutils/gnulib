/* Test of gl_locale_name function.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include "localename.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setenv.h"

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          abort ();							     \
        }								     \
    }									     \
  while (0)

int
main ()
{
  /* Check that gl_locale_name returns non-NULL.  */
  ASSERT (gl_locale_name (LC_MESSAGES, "LC_MESSAGES") != NULL);

  /* Check that when all environment variables are unset,
     gl_locale_name_posix returns NULL.  */
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
  ASSERT (strcmp (gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  unsetenv ("LC_ALL");
  setenv ("LC_CTYPE", "C", 1);
  setenv ("LC_MESSAGES", "C", 1);
  unsetenv ("LANG");
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  unsetenv ("LC_ALL");
  unsetenv ("LC_CTYPE");
  unsetenv ("LC_MESSAGES");
  setenv ("LANG", "C", 1);
  setlocale (LC_ALL, "");
  ASSERT (strcmp (gl_locale_name_posix (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);
  ASSERT (strcmp (gl_locale_name (LC_MESSAGES, "LC_MESSAGES"), "C") == 0);

  return 0;
}
