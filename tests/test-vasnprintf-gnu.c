/* Test of POSIX and GNU compatible vasnprintf() and asnprintf() functions.
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

#include "vasnprintf.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

static void
test_function (char * (*my_asnprintf) (char *, size_t *, const char *, ...))
{
  /* Test the support of the 'B' conversion specifier for binary output of
     integers.  */

  { /* Zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%B %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* A large positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%B %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11111111111111111111111111111110 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%*B %d", 20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%*B %d", -20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Zero precision and a positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Zero precision and a zero number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0B %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Width and precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%25.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Padding and precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%025.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%-20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#B %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0B11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "    0B11000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%0#20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0B000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%0#.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0B00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#25.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "   0B00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%0#25.20B %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "   0B00000011000000111001 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#.0B %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }
}

static char *
my_asnprintf (char *resultbuf, size_t *lengthp, const char *format, ...)
{
  va_list args;
  char *ret;

  va_start (args, format);
  ret = vasnprintf (resultbuf, lengthp, format, args);
  va_end (args);
  return ret;
}

static void
test_vasnprintf ()
{
  test_function (my_asnprintf);
}

static void
test_asnprintf ()
{
  test_function (asnprintf);
}

int
main (int argc, char *argv[])
{
  test_vasnprintf ();
  test_asnprintf ();
  return test_exit_status;
}
