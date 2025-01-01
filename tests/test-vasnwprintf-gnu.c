/* Test of POSIX and GNU compatible vasnwprintf() and asnwprintf() functions.
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

#include "vasnwprintf.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "macros.h"

static void
test_function (wchar_t * (*my_asnwprintf) (wchar_t *, size_t *, const wchar_t *, ...))
{
  /* Test the support of the 'b' conversion specifier for binary output of
     integers.  */

  { /* Zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* A large positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%b %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11111111111111111111111111111110 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width given as argument.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*b %d", 20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"      11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%*b %d", -20, 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"     00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* Padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%025.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (wcscmp (result, L"     00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%-20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"11000000111001       33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with zero.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"    0b11000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (wcscmp (result, L"   0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%0#25.20b %d", 12345, 33, 44, 55);
    ASSERT (result != NULL);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (wcscmp (result, L"   0b00000011000000111001 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    size_t length;
    wchar_t *result =
      my_asnwprintf (NULL, &length, L"%#.0b %d", 0, 33, 44, 55);
    ASSERT (result != NULL);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (wcscmp (result, L" 33") == 0);
    ASSERT (length == wcslen (result));
    free (result);
  }
}

static wchar_t *
my_asnwprintf (wchar_t *resultbuf, size_t *lengthp, const wchar_t *format, ...)
{
  va_list args;
  wchar_t *ret;

  va_start (args, format);
  ret = vasnwprintf (resultbuf, lengthp, format, args);
  va_end (args);
  return ret;
}

static void
test_vasnwprintf ()
{
  test_function (my_asnwprintf);
}

static void
test_asnwprintf ()
{
  test_function (asnwprintf);
}

int
main (int argc, char *argv[])
{
  test_vasnwprintf ();
  test_asnwprintf ();
  return test_exit_status;
}
