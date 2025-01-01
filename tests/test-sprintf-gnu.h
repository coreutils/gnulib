/* Test of POSIX and GNU compatible vsprintf() and sprintf() functions.
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

static void
test_function (int (*my_sprintf) (char *, const char *, ...))
{
  char result[5000];

  /* Test the support of the 'B' conversion specifier for binary output of
     integers.  */

  { /* Zero.  */
    int retval =
      my_sprintf (result, "%B %d", 0, 33, 44, 55);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    int retval =
      my_sprintf (result, "%B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A large positive number.  */
    int retval =
      my_sprintf (result, "%B %d", 0xFFFFFFFEU, 33, 44, 55);
    ASSERT (strcmp (result, "11111111111111111111111111111110 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    int retval =
      my_sprintf (result, "%20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width given as argument.  */
    int retval =
      my_sprintf (result, "%*B %d", 20, 12345, 33, 44, 55);
    ASSERT (strcmp (result, "      11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative width given as argument (cf. FLAG_LEFT below).  */
    int retval =
      my_sprintf (result, "%*B %d", -20, 12345, 33, 44, 55);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    int retval =
      my_sprintf (result, "%.20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Zero precision and a positive number.  */
    int retval =
      my_sprintf (result, "%.0B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Zero precision and a zero number.  */
    int retval =
      my_sprintf (result, "%.0B %d", 0, 33, 44, 55);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters."  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width and precision.  */
    int retval =
      my_sprintf (result, "%25.20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Padding and precision.  */
    int retval =
      my_sprintf (result, "%025.20B %d", 12345, 33, 44, 55);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "     00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    int retval =
      my_sprintf (result, "%-20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "11000000111001       33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with zero.  */
    int retval =
      my_sprintf (result, "%#B %d", 0, 33, 44, 55);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number.  */
    int retval =
      my_sprintf (result, "%#B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "0B11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number and width.  */
    int retval =
      my_sprintf (result, "%#20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "    0B11000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number and padding.  */
    int retval =
      my_sprintf (result, "%0#20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "0B000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number and precision.  */
    int retval =
      my_sprintf (result, "%0#.20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "0B00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number and width and precision.  */
    int retval =
      my_sprintf (result, "%#25.20B %d", 12345, 33, 44, 55);
    ASSERT (strcmp (result, "   0B00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a positive number and padding and precision.  */
    int retval =
      my_sprintf (result, "%0#25.20B %d", 12345, 33, 44, 55);
    /* Neither ISO C nor POSIX specify that the '0' flag is ignored when
       a width and a precision are both present.  But implementations do so.  */
    ASSERT (strcmp (result, "   0B00000011000000111001 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT with a zero precision and a zero number.  */
    int retval =
      my_sprintf (result, "%#.0B %d", 0, 33, 44, 55);
    /* ISO C and POSIX specify that "The result of converting a zero value
       with a precision of zero is no characters.", and the prefix is added
       only for non-zero values.  */
    ASSERT (strcmp (result, " 33") == 0);
    ASSERT (retval == strlen (result));
  }
}
