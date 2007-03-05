/* Test of POSIX compatible vasnprintf() and asnprintf() functions.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "vasnprintf.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) if (!(expr)) abort ();

static void
test_function (char * (*my_asnprintf) (char *, size_t *, const char *, ...))
{
  char buf[8];
  int size;

  /* Test return value convention.  */

  for (size = 0; size <= 8; size++)
    {
      size_t length = size;
      char *result = my_asnprintf (NULL, &length, "%d", 12345);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "12345") == 0);
      ASSERT (length == 5);
      free (result);
    }

  for (size = 0; size <= 8; size++)
    {
      size_t length;
      char *result;

      memcpy (buf, "DEADBEEF", 8);
      length = size;
      result = my_asnprintf (buf, &length, "%d", 12345);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "12345") == 0);
      ASSERT (length == 5);
      if (size < 6)
	ASSERT (result != buf);
      ASSERT (memcmp (buf + size, "DEADBEEF" + size, 8 - size) == 0);
      if (result != buf)
	free (result);
    }

  /* Test support of size specifiers as in C99.  */

  {
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%ju %d", (uintmax_t) 12345671, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345671 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  {
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%zu %d", (size_t) 12345672, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345672 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  {
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345673 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

#if HAVE_LONG_DOUBLE
  {
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%Lg %d", (long double) 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "1.5 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }
#endif

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* A positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", -0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%a %d", 0.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding near the decimal point.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0a %d", 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.1a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.2a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.3a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.3a %d", 1.49999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.1a %d", 1.999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Small precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Large precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.50a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%-10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%+a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "% a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#a %d", 1.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010a %d", 0.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

#if HAVE_LONG_DOUBLE

  { /* A positive number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", 3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* A negative number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%LA %d", -3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Positive zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Negative zero.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", -0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Positive infinity.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Negative infinity.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* NaN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding near the decimal point.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0La %d", 1.5L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.0La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.1La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.2La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.3La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.3La %d", 1.49999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug and a
       glibc 2.4 bug <http://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.1La %d", 1.999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Width.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Small precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* Large precision.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%.50La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%-10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%+La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "% La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%#La %d", 1.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%010La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (length == strlen (result));
    free (result);
  }

#endif

  /* Test the support of the %n format directive.  */

  {
    int count = -1;
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%d %n", 123, &count, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "123 ") == 0);
    ASSERT (length == strlen (result));
    ASSERT (count == 4);
    free (result);
  }

  /* Test the support of the POSIX/XSI format strings with positions.  */

  {
    size_t length;
    char *result =
      my_asnprintf (NULL, &length, "%2$d %1$d", 33, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "55 33") == 0);
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
  return 0;
}
