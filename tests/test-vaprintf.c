/* Test of vaprintf() and aprintf() functions.
   Copyright (C) 2007-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

/* Tell GCC not to warn about the specific edge cases tested here.  */
#if _GL_GNUC_PREREQ (4, 3)
# pragma GCC diagnostic ignored "-Wformat-zero-length"
# pragma GCC diagnostic ignored "-Wformat-nonliteral"
# pragma GCC diagnostic ignored "-Wformat-security"
#endif

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

static char *
my_aprintf (const char *format, ...)
{
  va_list args;
  char *ret;

  va_start (args, format);
  ret = vaprintf (format, args);
  va_end (args);
  return ret;
}

static void
test_vaprintf (void)
{
  char *result;

  for (int repeat = 0; repeat <= 8; repeat++)
    {
      result = my_aprintf ("%d", 12345);
      ASSERT (result != NULL);
      ASSERT (streq (result, "12345"));
      free (result);
    }

  {
    /* Silence gcc warning about zero-length format string.  */
    const char *empty = "";
    result = my_aprintf (empty);
    ASSERT (result != NULL);
    ASSERT (streq (result, ""));
    free (result);
  }

  result = my_aprintf ("%s", "foo");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foo"));
  free (result);

  result = my_aprintf ("%s%s", "foo", "bar");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foobar"));
  free (result);

  result = my_aprintf ("%s%sbaz", "foo", "bar");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foobarbaz"));
  free (result);
}

static void
test_aprintf (void)
{
  char *result;

  for (int repeat = 0; repeat <= 8; repeat++)
    {
      result = aprintf ("%d", 12345);
      ASSERT (result != NULL);
      ASSERT (streq (result, "12345"));
      free (result);
    }

  {
    /* Silence gcc warning about zero-length format string,
       and about "format not a string literal and no format"
       (whatever that means) .  */
    const char *empty = "";
    result = aprintf (empty, empty);
    ASSERT (result != NULL);
    ASSERT (streq (result, ""));
    free (result);
  }

  result = aprintf ("%s", "foo");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foo"));
  free (result);

  result = aprintf ("%s%s", "foo", "bar");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foobar"));
  free (result);

  result = aprintf ("%s%sbaz", "foo", "bar");
  ASSERT (result != NULL);
  ASSERT (streq (result, "foobarbaz"));
  free (result);
}

int
main (_GL_UNUSED int argc, char *argv[])
{
  test_vaprintf ();
  test_aprintf ();

  return test_exit_status;
}
