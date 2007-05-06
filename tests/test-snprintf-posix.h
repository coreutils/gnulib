/* Test of POSIX compatible vsnprintf() and snprintf() functions.
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
test_function (int (*my_snprintf) (char *, size_t, const char *, ...))
{
  char buf[8];
  int size;

  /* Test return value convention.  */

  for (size = 0; size <= 8; size++)
    {
      int retval;

      memcpy (buf, "DEADBEEF", 8);
      retval = my_snprintf (buf, size, "%d", 12345);
      ASSERT (retval == 5);
      if (size < 6)
	{
	  if (size > 0)
	    {
	      ASSERT (memcmp (buf, "12345", size - 1) == 0);
	      ASSERT (buf[size - 1] == '\0');
	    }
	  ASSERT (memcmp (buf + size, "DEADBEEF" + size, 8 - size) == 0);
	}
      else
	{
	  ASSERT (memcmp (buf, "12345\0EF", 8) == 0);
	}
    }

  /* Test support of size specifiers as in C99.  */

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%ju %d", (uintmax_t) 12345671, 33, 44, 55);
    ASSERT (strcmp (result, "12345671 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%zu %d", (size_t) 12345672, 33, 44, 55);
    ASSERT (strcmp (result, "12345672 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55);
    ASSERT (strcmp (result, "12345673 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lg %d", (long double) 1.5, 33, 44, 55);
    ASSERT (strcmp (result, "1.5 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%a %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding near the decimal point.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.0a %d", 1.5, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 0.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.0a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 1.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.1a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 2.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.2a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 3.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.3a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.3a %d", 1.49999, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.1a %d", 1.999, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Large precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.50a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%-10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%+a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "% a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%#a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%#a %d", 1.0, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010a %d", 1.0 / 0.0, 33, 44, 55);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010a %d", NaN (), 33, 44, 55);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", 3.1416015625L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LA %d", -3.1416015625L, 33, 44, 55);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding near the decimal point.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.0La %d", 1.5L, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 0.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.0La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 1.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.1La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 2.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.2La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 3.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.3La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.3La %d", 1.49999L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug and a
       glibc 2.4 bug <http://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.1La %d", 1.999L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Large precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.50La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%-10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%+La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "% La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%#La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%#La %d", 1.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010La %d", 1.0L / 0.0L, 33, 44, 55);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%010La %d", 0.0L / 0.0L, 33, 44, 55);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %f format directive.  */

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%f %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015f %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015f %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.f %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%Lf %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015Lf %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %F format directive.  */

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "INF 33") == 0
	    || strcmp (result, "INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-INF 33") == 0
	    || strcmp (result, "-INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%F %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "NAN 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015F %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015F %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -INF 33") == 0
	    || strcmp (result, "      -INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.F %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "INF 33") == 0
	    || strcmp (result, "INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-INF 33") == 0
	    || strcmp (result, "-INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%LF %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "NAN 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015LF %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%015LF %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -INF 33") == 0
	    || strcmp (result, "      -INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%.LF %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %n format directive.  */

  {
    int count = -1;
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%d %n", 123, &count, 33, 44, 55);
    ASSERT (strcmp (result, "123 ") == 0);
    ASSERT (retval == strlen (result));
    ASSERT (count == 4);
  }

  /* Test the support of the POSIX/XSI format strings with positions.  */

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%2$d %1$d", 33, 55);
    ASSERT (strcmp (result, "55 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the grouping flag.  */

  {
    char result[100];
    int retval =
      my_snprintf (result, sizeof (result), "%'d %d", 1234567, 99);
    ASSERT (result[strlen (result) - 1] == '9');
    ASSERT (retval == strlen (result));
  }
}
