/* Test of POSIX compatible vasprintf() and asprintf() functions.
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

#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT(expr) if (!(expr)) abort ();

/* The Compaq (ex-DEC) C 6.4 compiler chokes on the expression 0.0 / 0.0.  */
#ifdef __DECC
static double
NaN ()
{
  static double zero = 0.0;
  return zero / zero;
}
#else
# define NaN() (0.0 / 0.0)
#endif

static void
test_function (int (*my_asprintf) (char **, const char *, ...))
{
  int repeat;

  /* Test return value convention.  */

  for (repeat = 0; repeat <= 8; repeat++)
    {
      char *result;
      int retval = asprintf (&result, "%d", 12345);
      ASSERT (retval == 5);
      ASSERT (result != NULL);
      ASSERT (strcmp (result, "12345") == 0);
      free (result);
    }

  /* Test support of size specifiers as in C99.  */

  {
    char *result;
    int retval =
      my_asprintf (&result, "%ju %d", (uintmax_t) 12345671, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345671 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  {
    char *result;
    int retval =
      my_asprintf (&result, "%zu %d", (size_t) 12345672, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345672 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  {
    char *result;
    int retval =
      my_asprintf (&result, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "12345673 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  {
    char *result;
    int retval =
      my_asprintf (&result, "%Lg %d", (long double) 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "1.5 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* A positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* A negative number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Positive zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Negative zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", -0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Positive infinity.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Negative infinity.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* NaN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%a %d", NaN (), 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding near the decimal point.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0a %d", 1.5, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.1a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.2a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.3a %d", 1.51, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.3a %d", 1.49999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.1a %d", 1.999, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Width.  */
    char *result;
    int retval =
      my_asprintf (&result, "%10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Small precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Large precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.50a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%-10a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%+a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    char *result;
    int retval =
      my_asprintf (&result, "% a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#a %d", 1.0, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010a %d", 1.75, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010a %d", NaN (), 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* A positive number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", 3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* A negative number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%LA %d", -3.1416015625L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Positive zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Negative zero.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", -0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Positive infinity.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Negative infinity.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* NaN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding near the decimal point.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0La %d", 1.5L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 0.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.0La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 1.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.1La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 2.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.2La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding with precision 3.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.3La %d", 1.51L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.3La %d", 1.49999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug and a
       glibc 2.4 bug <http://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.1La %d", 1.999L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Width.  */
    char *result;
    int retval =
      my_asprintf (&result, "%10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Small precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* Large precision.  */
    char *result;
    int retval =
      my_asprintf (&result, "%.50La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_LEFT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%-10La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_SHOWSIGN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%+La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_SPACE.  */
    char *result;
    int retval =
      my_asprintf (&result, "% La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ALT.  */
    char *result;
    int retval =
      my_asprintf (&result, "%#La %d", 1.0L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with finite number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010La %d", 1.75L, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with infinite number.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  { /* FLAG_ZERO with NaN.  */
    char *result;
    int retval =
      my_asprintf (&result, "%010La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (result != NULL);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
    free (result);
  }

  /* Test the support of the %n format directive.  */

  {
    int count = -1;
    char *result;
    int retval =
      my_asprintf (&result, "%d %n", 123, &count, 33, 44, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "123 ") == 0);
    ASSERT (retval == strlen (result));
    ASSERT (count == 4);
    free (result);
  }

  /* Test the support of the POSIX/XSI format strings with positions.  */

  {
    char *result;
    int retval =
      my_asprintf (&result, "%2$d %1$d", 33, 55);
    ASSERT (result != NULL);
    ASSERT (strcmp (result, "55 33") == 0);
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
  return 0;
}
