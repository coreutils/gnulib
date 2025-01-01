/* Test of POSIX and GNU compatible vasprintf() and asprintf() functions.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

static void
test_function (int (*my_asprintf) (char **, const char *, ...))
{
  /* Test the support of the 'b' conversion specifier for binary output of
     integers.  */

  { /* Zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* A positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* A large positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%b %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11111111111111111111111111111110 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Width.  */
    char *result;
    int retval =
      my_asprintf (&result, "%20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Width given as argument.  */
    char *result;
    int retval =
      my_asprintf (&result, "%*b %d", 20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    char *result;
    int retval =
      my_asprintf (&result, "%*b %d", -20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Zero precision and a positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Zero precision and a zero number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Width and precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Padding and precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%025.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%-20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0b11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "    0b11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding.  */
    char *result;
    int retval =
      my_asprintf (&result, "%0#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0b000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%0#.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0b00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "   0b00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%0#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "   0b00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }
}

static int
my_asprintf (char **result, const char *format, ...)
{
  va_list args;
  int ret;

  va_start (args, format);
  ret = vasprintf (result, format, args);
  va_end (args);
  return ret;
}

static void
test_vasprintf ()
{
  test_function (my_asprintf);
}

static void
test_asprintf ()
{
  test_function (asprintf);
}

int
main (int argc, char *argv[])
{
  test_vasprintf ();
  test_asprintf ();
  return test_exit_status;
}
