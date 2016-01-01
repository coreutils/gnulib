/* Test of glob/globfree functions.
   Copyright (C) 2009-2016 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson <simon@josefsson.org>, 2009.  */

#include <config.h>

#include <glob.h>

#include "signature.h"
SIGNATURE_CHECK (glob, int, (char const *, int, int (*) (char const *, int),
                             glob_t *));
SIGNATURE_CHECK (globfree, void, (glob_t *));

#include <string.h>

#include "macros.h"

#define GL_NO_SUCH_FILE "/gnulib-magic-does-not-exist"

int
main ()
{
  int res;
  glob_t g;

  /* Make sure glob_t struct members exists. */
  ASSERT (sizeof (g.gl_pathc));
  ASSERT (sizeof (g.gl_pathv));
  ASSERT (sizeof (g.gl_offs));

  res = glob (".", 0, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob (".", GLOB_NOSORT, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob (".", GLOB_MARK, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);

  res = glob (".", GLOB_APPEND, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 2);
  globfree (&g);

  res = glob (".", GLOB_NOESCAPE, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  globfree (&g);

  res = glob ("./*", 0, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc >= 1);
  globfree (&g);

  res = glob (GL_NO_SUCH_FILE, 0, NULL, &g);
  ASSERT (res == GLOB_NOMATCH);

  res = glob (GL_NO_SUCH_FILE, GLOB_NOCHECK, NULL, &g);
  ASSERT (res == 0 && g.gl_pathc == 1);
  ASSERT (strcmp (g.gl_pathv[0], GL_NO_SUCH_FILE) == 0);
  globfree (&g);

  return 0;
}
