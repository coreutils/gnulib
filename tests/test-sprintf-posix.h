/* Test of POSIX compatible vsprintf() and sprintf() functions.
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

static int
strmatch (const char *pattern, const char *string)
{
  if (strlen (pattern) != strlen (string))
    return 0;
  for (; *pattern != '\0'; pattern++, string++)
    if (*pattern != '*' && *string != *pattern)
      return 0;
  return 1;
}

static void
test_function (int (*my_sprintf) (char *, const char *, ...))
{
  char buf[8];

  /* Test return value convention.  */

  {
    int retval;

    memcpy (buf, "DEADBEEF", 8);
    retval = my_sprintf (buf, "%d", 12345);
    ASSERT (retval == 5);
    ASSERT (memcmp (buf, "12345\0EF", 8) == 0);
  }

  /* Test support of size specifiers as in C99.  */

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%ju %d", (uintmax_t) 12345671, 33, 44, 55);
    ASSERT (strcmp (result, "12345671 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%zu %d", (size_t) 12345672, 33, 44, 55);
    ASSERT (strcmp (result, "12345672 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55);
    ASSERT (strcmp (result, "12345673 33") == 0);
    ASSERT (retval == strlen (result));
  }

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", (long double) 1.5, 33, 44, 55);
    ASSERT (strcmp (result, "1.5 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the 'a' and 'A' conversion specifier for hexadecimal
     output of floating-point numbers.  */

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", 3.1416015625, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%A %d", -3.1416015625, 33, 44, 55);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%a %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding near the decimal point.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.0a %d", 1.5, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 0.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.0a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 1.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.1a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 2.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.2a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 3.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.3a %d", 1.51, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.3a %d", 1.49999, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.1a %d", 1.999, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Large precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.50a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#a %d", 1.0, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010a %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010a %d", 1.0 / 0.0, 33, 44, 55);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010a %d", NaN (), 33, 44, 55);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", 3.1416015625L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.922p+1 33") == 0
	    || strcmp (result, "0x3.244p+0 33") == 0
	    || strcmp (result, "0x6.488p-1 33") == 0
	    || strcmp (result, "0xc.91p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LA %d", -3.1416015625L, 33, 44, 55);
    ASSERT (strcmp (result, "-0X1.922P+1 33") == 0
	    || strcmp (result, "-0X3.244P+0 33") == 0
	    || strcmp (result, "-0X6.488P-1 33") == 0
	    || strcmp (result, "-0XC.91P-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0x0p+0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%La %d", 0.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding near the decimal point.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.0La %d", 1.5L, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 0.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.0La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x2p+0 33") == 0
	    || strcmp (result, "0x3p-1 33") == 0
	    || strcmp (result, "0x6p-2 33") == 0
	    || strcmp (result, "0xcp-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 1.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.1La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.8p+0 33") == 0
	    || strcmp (result, "0x3.0p-1 33") == 0
	    || strcmp (result, "0x6.1p-2 33") == 0
	    || strcmp (result, "0xc.1p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 2.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.2La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.83p+0 33") == 0
	    || strcmp (result, "0x3.05p-1 33") == 0
	    || strcmp (result, "0x6.0ap-2 33") == 0
	    || strcmp (result, "0xc.14p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding with precision 3.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.3La %d", 1.51L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.829p+0 33") == 0
	    || strcmp (result, "0x3.052p-1 33") == 0
	    || strcmp (result, "0x6.0a4p-2 33") == 0
	    || strcmp (result, "0xc.148p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.3La %d", 1.49999L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.800p+0 33") == 0
	    || strcmp (result, "0x3.000p-1 33") == 0
	    || strcmp (result, "0x6.000p-2 33") == 0
	    || strcmp (result, "0xc.000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Rounding can turn a ...FFF into a ...000.
       This shows a MacOS X 10.3.9 (Darwin 7.9) bug and a
       glibc 2.4 bug <http://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.1La %d", 1.999L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.0p+1 33") == 0
	    || strcmp (result, "0x2.0p+0 33") == 0
	    || strcmp (result, "0x4.0p-1 33") == 0
	    || strcmp (result, "0x8.0p-2 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "  0x1.cp+0 33") == 0
	    || strcmp (result, "  0x3.8p-1 33") == 0
	    || strcmp (result, "    0x7p-2 33") == 0
	    || strcmp (result, "    0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c000000000p+0 33") == 0
	    || strcmp (result, "0x3.8000000000p-1 33") == 0
	    || strcmp (result, "0x7.0000000000p-2 33") == 0
	    || strcmp (result, "0xe.0000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Large precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.50La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.c0000000000000000000000000000000000000000000000000p+0 33") == 0
	    || strcmp (result, "0x3.80000000000000000000000000000000000000000000000000p-1 33") == 0
	    || strcmp (result, "0x7.00000000000000000000000000000000000000000000000000p-2 33") == 0
	    || strcmp (result, "0xe.00000000000000000000000000000000000000000000000000p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0   33") == 0
	    || strcmp (result, "0x3.8p-1   33") == 0
	    || strcmp (result, "0x7p-2     33") == 0
	    || strcmp (result, "0xep-3     33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "+0x1.cp+0 33") == 0
	    || strcmp (result, "+0x3.8p-1 33") == 0
	    || strcmp (result, "+0x7p-2 33") == 0
	    || strcmp (result, "+0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, " 0x1.cp+0 33") == 0
	    || strcmp (result, " 0x3.8p-1 33") == 0
	    || strcmp (result, " 0x7p-2 33") == 0
	    || strcmp (result, " 0xep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.cp+0 33") == 0
	    || strcmp (result, "0x3.8p-1 33") == 0
	    || strcmp (result, "0x7.p-2 33") == 0
	    || strcmp (result, "0xe.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#La %d", 1.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0x1.p+0 33") == 0
	    || strcmp (result, "0x2.p-1 33") == 0
	    || strcmp (result, "0x4.p-2 33") == 0
	    || strcmp (result, "0x8.p-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010La %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "0x001.cp+0 33") == 0
	    || strcmp (result, "0x003.8p-1 33") == 0
	    || strcmp (result, "0x00007p-2 33") == 0
	    || strcmp (result, "0x0000ep-3 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010La %d", 1.0L / 0.0L, 33, 44, 55);
    /* "0000000inf 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       inf 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010La %d", 0.0L / 0.0L, 33, 44, 55);
    /* "0000000nan 33" is not a valid result; see
       <http://lists.gnu.org/archive/html/bug-gnulib/2007-04/msg00107.html> */
    ASSERT (strcmp (result, "       nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %f format directive.  */

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37, "0.000000" },
	{ 1.234321234321234e-36, "0.000000" },
	{ 1.234321234321234e-35, "0.000000" },
	{ 1.234321234321234e-34, "0.000000" },
	{ 1.234321234321234e-33, "0.000000" },
	{ 1.234321234321234e-32, "0.000000" },
	{ 1.234321234321234e-31, "0.000000" },
	{ 1.234321234321234e-30, "0.000000" },
	{ 1.234321234321234e-29, "0.000000" },
	{ 1.234321234321234e-28, "0.000000" },
	{ 1.234321234321234e-27, "0.000000" },
	{ 1.234321234321234e-26, "0.000000" },
	{ 1.234321234321234e-25, "0.000000" },
	{ 1.234321234321234e-24, "0.000000" },
	{ 1.234321234321234e-23, "0.000000" },
	{ 1.234321234321234e-22, "0.000000" },
	{ 1.234321234321234e-21, "0.000000" },
	{ 1.234321234321234e-20, "0.000000" },
	{ 1.234321234321234e-19, "0.000000" },
	{ 1.234321234321234e-18, "0.000000" },
	{ 1.234321234321234e-17, "0.000000" },
	{ 1.234321234321234e-16, "0.000000" },
	{ 1.234321234321234e-15, "0.000000" },
	{ 1.234321234321234e-14, "0.000000" },
	{ 1.234321234321234e-13, "0.000000" },
	{ 1.234321234321234e-12, "0.000000" },
	{ 1.234321234321234e-11, "0.000000" },
	{ 1.234321234321234e-10, "0.000000" },
	{ 1.234321234321234e-9, "0.000000" },
	{ 1.234321234321234e-8, "0.000000" },
	{ 1.234321234321234e-7, "0.000000" },
	{ 1.234321234321234e-6, "0.000001" },
	{ 1.234321234321234e-5, "0.000012" },
	{ 1.234321234321234e-4, "0.000123" },
	{ 1.234321234321234e-3, "0.001234" },
	{ 1.234321234321234e-2, "0.012343" },
	{ 1.234321234321234e-1, "0.123432" },
	{ 1.234321234321234, "1.234321" },
	{ 1.234321234321234e1, "12.343212" },
	{ 1.234321234321234e2, "123.432123" },
	{ 1.234321234321234e3, "1234.321234" },
	{ 1.234321234321234e4, "12343.212343" },
	{ 1.234321234321234e5, "123432.123432" },
	{ 1.234321234321234e6, "1234321.234321" },
	{ 1.234321234321234e7, "12343212.343212" },
	{ 1.234321234321234e8, "123432123.432123" },
	{ 1.234321234321234e9, "1234321234.321234" },
	{ 1.234321234321234e10, "12343212343.2123**" },
	{ 1.234321234321234e11, "123432123432.123***" },
	{ 1.234321234321234e12, "1234321234321.23****" },
	{ 1.234321234321234e13, "12343212343212.3*****" },
	{ 1.234321234321234e14, "123432123432123.******" },
	{ 1.234321234321234e15, "1234321234321234.000000" },
	{ 1.234321234321234e16, "123432123432123**.000000" },
	{ 1.234321234321234e17, "123432123432123***.000000" },
	{ 1.234321234321234e18, "123432123432123****.000000" },
	{ 1.234321234321234e19, "123432123432123*****.000000" },
	{ 1.234321234321234e20, "123432123432123******.000000" },
	{ 1.234321234321234e21, "123432123432123*******.000000" },
	{ 1.234321234321234e22, "123432123432123********.000000" },
	{ 1.234321234321234e23, "123432123432123*********.000000" },
	{ 1.234321234321234e24, "123432123432123**********.000000" },
	{ 1.234321234321234e25, "123432123432123***********.000000" },
	{ 1.234321234321234e26, "123432123432123************.000000" },
	{ 1.234321234321234e27, "123432123432123*************.000000" },
	{ 1.234321234321234e28, "123432123432123**************.000000" },
	{ 1.234321234321234e29, "123432123432123***************.000000" },
	{ 1.234321234321234e30, "123432123432123****************.000000" },
	{ 1.234321234321234e31, "123432123432123*****************.000000" },
	{ 1.234321234321234e32, "123432123432123******************.000000" },
	{ 1.234321234321234e33, "123432123432123*******************.000000" },
	{ 1.234321234321234e34, "123432123432123********************.000000" },
	{ 1.234321234321234e35, "123432123432123*********************.000000" },
	{ 1.234321234321234e36, "123432123432123**********************.000000" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%f", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%f %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "  1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000   33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "+1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, " 1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.f %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "2. 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015f %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015f %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015f %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.f %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37L, "0.000000" },
	{ 1.234321234321234e-36L, "0.000000" },
	{ 1.234321234321234e-35L, "0.000000" },
	{ 1.234321234321234e-34L, "0.000000" },
	{ 1.234321234321234e-33L, "0.000000" },
	{ 1.234321234321234e-32L, "0.000000" },
	{ 1.234321234321234e-31L, "0.000000" },
	{ 1.234321234321234e-30L, "0.000000" },
	{ 1.234321234321234e-29L, "0.000000" },
	{ 1.234321234321234e-28L, "0.000000" },
	{ 1.234321234321234e-27L, "0.000000" },
	{ 1.234321234321234e-26L, "0.000000" },
	{ 1.234321234321234e-25L, "0.000000" },
	{ 1.234321234321234e-24L, "0.000000" },
	{ 1.234321234321234e-23L, "0.000000" },
	{ 1.234321234321234e-22L, "0.000000" },
	{ 1.234321234321234e-21L, "0.000000" },
	{ 1.234321234321234e-20L, "0.000000" },
	{ 1.234321234321234e-19L, "0.000000" },
	{ 1.234321234321234e-18L, "0.000000" },
	{ 1.234321234321234e-17L, "0.000000" },
	{ 1.234321234321234e-16L, "0.000000" },
	{ 1.234321234321234e-15L, "0.000000" },
	{ 1.234321234321234e-14L, "0.000000" },
	{ 1.234321234321234e-13L, "0.000000" },
	{ 1.234321234321234e-12L, "0.000000" },
	{ 1.234321234321234e-11L, "0.000000" },
	{ 1.234321234321234e-10L, "0.000000" },
	{ 1.234321234321234e-9L, "0.000000" },
	{ 1.234321234321234e-8L, "0.000000" },
	{ 1.234321234321234e-7L, "0.000000" },
	{ 1.234321234321234e-6L, "0.000001" },
	{ 1.234321234321234e-5L, "0.000012" },
	{ 1.234321234321234e-4L, "0.000123" },
	{ 1.234321234321234e-3L, "0.001234" },
	{ 1.234321234321234e-2L, "0.012343" },
	{ 1.234321234321234e-1L, "0.123432" },
	{ 1.234321234321234L, "1.234321" },
	{ 1.234321234321234e1L, "12.343212" },
	{ 1.234321234321234e2L, "123.432123" },
	{ 1.234321234321234e3L, "1234.321234" },
	{ 1.234321234321234e4L, "12343.212343" },
	{ 1.234321234321234e5L, "123432.123432" },
	{ 1.234321234321234e6L, "1234321.234321" },
	{ 1.234321234321234e7L, "12343212.343212" },
	{ 1.234321234321234e8L, "123432123.432123" },
	{ 1.234321234321234e9L, "1234321234.321234" },
	{ 1.234321234321234e10L, "12343212343.2123**" },
	{ 1.234321234321234e11L, "123432123432.123***" },
	{ 1.234321234321234e12L, "1234321234321.23****" },
	{ 1.234321234321234e13L, "12343212343212.3*****" },
	{ 1.234321234321234e14L, "123432123432123.******" },
	{ 1.234321234321234e15L, "1234321234321234.000000" },
	{ 1.234321234321234e16L, "123432123432123**.000000" },
	{ 1.234321234321234e17L, "123432123432123***.000000" },
	{ 1.234321234321234e18L, "123432123432123****.000000" },
	{ 1.234321234321234e19L, "123432123432123*****.000000" },
	{ 1.234321234321234e20L, "123432123432123******.000000" },
	{ 1.234321234321234e21L, "123432123432123*******.000000" },
	{ 1.234321234321234e22L, "123432123432123********.000000" },
	{ 1.234321234321234e23L, "123432123432123*********.000000" },
	{ 1.234321234321234e24L, "123432123432123**********.000000" },
	{ 1.234321234321234e25L, "123432123432123***********.000000" },
	{ 1.234321234321234e26L, "123432123432123************.000000" },
	{ 1.234321234321234e27L, "123432123432123*************.000000" },
	{ 1.234321234321234e28L, "123432123432123**************.000000" },
	{ 1.234321234321234e29L, "123432123432123***************.000000" },
	{ 1.234321234321234e30L, "123432123432123****************.000000" },
	{ 1.234321234321234e31L, "123432123432123*****************.000000" },
	{ 1.234321234321234e32L, "123432123432123******************.000000" },
	{ 1.234321234321234e33L, "123432123432123*******************.000000" },
	{ 1.234321234321234e34L, "123432123432123********************.000000" },
	{ 1.234321234321234e35L, "123432123432123*********************.000000" },
	{ 1.234321234321234e36L, "123432123432123**********************.000000" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%Lf", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%Lf %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "  1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000   33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "+1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, " 1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.Lf %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "2. 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015Lf %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%015Lf %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.Lf %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %F format directive.  */

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "INF 33") == 0
	    || strcmp (result, "INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-INF 33") == 0
	    || strcmp (result, "-INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%F %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "NAN 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015F %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015F %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -INF 33") == 0
	    || strcmp (result, "      -INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.F %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "12.750000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234567.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.031250 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "INF 33") == 0
	    || strcmp (result, "INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-INF 33") == 0
	    || strcmp (result, "-INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%LF %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "NAN 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015LF %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "00001234.000000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015LF %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -INF 33") == 0
	    || strcmp (result, "      -INFINITY 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.LF %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %e format directive.  */

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.275000e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1.234567e+06 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37, "1.234321e-37" },
	{ 1.234321234321234e-36, "1.234321e-36" },
	{ 1.234321234321234e-35, "1.234321e-35" },
	{ 1.234321234321234e-34, "1.234321e-34" },
	{ 1.234321234321234e-33, "1.234321e-33" },
	{ 1.234321234321234e-32, "1.234321e-32" },
	{ 1.234321234321234e-31, "1.234321e-31" },
	{ 1.234321234321234e-30, "1.234321e-30" },
	{ 1.234321234321234e-29, "1.234321e-29" },
	{ 1.234321234321234e-28, "1.234321e-28" },
	{ 1.234321234321234e-27, "1.234321e-27" },
	{ 1.234321234321234e-26, "1.234321e-26" },
	{ 1.234321234321234e-25, "1.234321e-25" },
	{ 1.234321234321234e-24, "1.234321e-24" },
	{ 1.234321234321234e-23, "1.234321e-23" },
	{ 1.234321234321234e-22, "1.234321e-22" },
	{ 1.234321234321234e-21, "1.234321e-21" },
	{ 1.234321234321234e-20, "1.234321e-20" },
	{ 1.234321234321234e-19, "1.234321e-19" },
	{ 1.234321234321234e-18, "1.234321e-18" },
	{ 1.234321234321234e-17, "1.234321e-17" },
	{ 1.234321234321234e-16, "1.234321e-16" },
	{ 1.234321234321234e-15, "1.234321e-15" },
	{ 1.234321234321234e-14, "1.234321e-14" },
	{ 1.234321234321234e-13, "1.234321e-13" },
	{ 1.234321234321234e-12, "1.234321e-12" },
	{ 1.234321234321234e-11, "1.234321e-11" },
	{ 1.234321234321234e-10, "1.234321e-10" },
	{ 1.234321234321234e-9, "1.234321e-09" },
	{ 1.234321234321234e-8, "1.234321e-08" },
	{ 1.234321234321234e-7, "1.234321e-07" },
	{ 1.234321234321234e-6, "1.234321e-06" },
	{ 1.234321234321234e-5, "1.234321e-05" },
	{ 1.234321234321234e-4, "1.234321e-04" },
	{ 1.234321234321234e-3, "1.234321e-03" },
	{ 1.234321234321234e-2, "1.234321e-02" },
	{ 1.234321234321234e-1, "1.234321e-01" },
	{ 1.234321234321234, "1.234321e+00" },
	{ 1.234321234321234e1, "1.234321e+01" },
	{ 1.234321234321234e2, "1.234321e+02" },
	{ 1.234321234321234e3, "1.234321e+03" },
	{ 1.234321234321234e4, "1.234321e+04" },
	{ 1.234321234321234e5, "1.234321e+05" },
	{ 1.234321234321234e6, "1.234321e+06" },
	{ 1.234321234321234e7, "1.234321e+07" },
	{ 1.234321234321234e8, "1.234321e+08" },
	{ 1.234321234321234e9, "1.234321e+09" },
	{ 1.234321234321234e10, "1.234321e+10" },
	{ 1.234321234321234e11, "1.234321e+11" },
	{ 1.234321234321234e12, "1.234321e+12" },
	{ 1.234321234321234e13, "1.234321e+13" },
	{ 1.234321234321234e14, "1.234321e+14" },
	{ 1.234321234321234e15, "1.234321e+15" },
	{ 1.234321234321234e16, "1.234321e+16" },
	{ 1.234321234321234e17, "1.234321e+17" },
	{ 1.234321234321234e18, "1.234321e+18" },
	{ 1.234321234321234e19, "1.234321e+19" },
	{ 1.234321234321234e20, "1.234321e+20" },
	{ 1.234321234321234e21, "1.234321e+21" },
	{ 1.234321234321234e22, "1.234321e+22" },
	{ 1.234321234321234e23, "1.234321e+23" },
	{ 1.234321234321234e24, "1.234321e+24" },
	{ 1.234321234321234e25, "1.234321e+25" },
	{ 1.234321234321234e26, "1.234321e+26" },
	{ 1.234321234321234e27, "1.234321e+27" },
	{ 1.234321234321234e28, "1.234321e+28" },
	{ 1.234321234321234e29, "1.234321e+29" },
	{ 1.234321234321234e30, "1.234321e+30" },
	{ 1.234321234321234e31, "1.234321e+31" },
	{ 1.234321234321234e32, "1.234321e+32" },
	{ 1.234321234321234e33, "1.234321e+33" },
	{ 1.234321234321234e34, "1.234321e+34" },
	{ 1.234321234321234e35, "1.234321e+35" },
	{ 1.234321234321234e36, "1.234321e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%e", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-3.125000e-02 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%e %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%15e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "   1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-15e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000e+00    33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "+1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, " 1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.e %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "2.e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.e %d", 9.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015e %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "0001.234000e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015e %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015e %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.e %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.275000e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1.234567e+06 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37L, "1.234321e-37" },
	{ 1.234321234321234e-36L, "1.234321e-36" },
	{ 1.234321234321234e-35L, "1.234321e-35" },
	{ 1.234321234321234e-34L, "1.234321e-34" },
	{ 1.234321234321234e-33L, "1.234321e-33" },
	{ 1.234321234321234e-32L, "1.234321e-32" },
	{ 1.234321234321234e-31L, "1.234321e-31" },
	{ 1.234321234321234e-30L, "1.234321e-30" },
	{ 1.234321234321234e-29L, "1.234321e-29" },
	{ 1.234321234321234e-28L, "1.234321e-28" },
	{ 1.234321234321234e-27L, "1.234321e-27" },
	{ 1.234321234321234e-26L, "1.234321e-26" },
	{ 1.234321234321234e-25L, "1.234321e-25" },
	{ 1.234321234321234e-24L, "1.234321e-24" },
	{ 1.234321234321234e-23L, "1.234321e-23" },
	{ 1.234321234321234e-22L, "1.234321e-22" },
	{ 1.234321234321234e-21L, "1.234321e-21" },
	{ 1.234321234321234e-20L, "1.234321e-20" },
	{ 1.234321234321234e-19L, "1.234321e-19" },
	{ 1.234321234321234e-18L, "1.234321e-18" },
	{ 1.234321234321234e-17L, "1.234321e-17" },
	{ 1.234321234321234e-16L, "1.234321e-16" },
	{ 1.234321234321234e-15L, "1.234321e-15" },
	{ 1.234321234321234e-14L, "1.234321e-14" },
	{ 1.234321234321234e-13L, "1.234321e-13" },
	{ 1.234321234321234e-12L, "1.234321e-12" },
	{ 1.234321234321234e-11L, "1.234321e-11" },
	{ 1.234321234321234e-10L, "1.234321e-10" },
	{ 1.234321234321234e-9L, "1.234321e-09" },
	{ 1.234321234321234e-8L, "1.234321e-08" },
	{ 1.234321234321234e-7L, "1.234321e-07" },
	{ 1.234321234321234e-6L, "1.234321e-06" },
	{ 1.234321234321234e-5L, "1.234321e-05" },
	{ 1.234321234321234e-4L, "1.234321e-04" },
	{ 1.234321234321234e-3L, "1.234321e-03" },
	{ 1.234321234321234e-2L, "1.234321e-02" },
	{ 1.234321234321234e-1L, "1.234321e-01" },
	{ 1.234321234321234L, "1.234321e+00" },
	{ 1.234321234321234e1L, "1.234321e+01" },
	{ 1.234321234321234e2L, "1.234321e+02" },
	{ 1.234321234321234e3L, "1.234321e+03" },
	{ 1.234321234321234e4L, "1.234321e+04" },
	{ 1.234321234321234e5L, "1.234321e+05" },
	{ 1.234321234321234e6L, "1.234321e+06" },
	{ 1.234321234321234e7L, "1.234321e+07" },
	{ 1.234321234321234e8L, "1.234321e+08" },
	{ 1.234321234321234e9L, "1.234321e+09" },
	{ 1.234321234321234e10L, "1.234321e+10" },
	{ 1.234321234321234e11L, "1.234321e+11" },
	{ 1.234321234321234e12L, "1.234321e+12" },
	{ 1.234321234321234e13L, "1.234321e+13" },
	{ 1.234321234321234e14L, "1.234321e+14" },
	{ 1.234321234321234e15L, "1.234321e+15" },
	{ 1.234321234321234e16L, "1.234321e+16" },
	{ 1.234321234321234e17L, "1.234321e+17" },
	{ 1.234321234321234e18L, "1.234321e+18" },
	{ 1.234321234321234e19L, "1.234321e+19" },
	{ 1.234321234321234e20L, "1.234321e+20" },
	{ 1.234321234321234e21L, "1.234321e+21" },
	{ 1.234321234321234e22L, "1.234321e+22" },
	{ 1.234321234321234e23L, "1.234321e+23" },
	{ 1.234321234321234e24L, "1.234321e+24" },
	{ 1.234321234321234e25L, "1.234321e+25" },
	{ 1.234321234321234e26L, "1.234321e+26" },
	{ 1.234321234321234e27L, "1.234321e+27" },
	{ 1.234321234321234e28L, "1.234321e+28" },
	{ 1.234321234321234e29L, "1.234321e+29" },
	{ 1.234321234321234e30L, "1.234321e+30" },
	{ 1.234321234321234e31L, "1.234321e+31" },
	{ 1.234321234321234e32L, "1.234321e+32" },
	{ 1.234321234321234e33L, "1.234321e+33" },
	{ 1.234321234321234e34L, "1.234321e+34" },
	{ 1.234321234321234e35L, "1.234321e+35" },
	{ 1.234321234321234e36L, "1.234321e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%Le", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-3.125000e-02 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0.000000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.000000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%Le %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%15Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "   1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-15Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000e+00    33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "+1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, " 1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.750000e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.Le %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "2.e+00 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.Le %d", 9.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015Le %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0001.234000e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015Le %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%015Le %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.Le %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %g format directive.  */

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", 12.75, 33, 44, 55);
    ASSERT (strcmp (result, "12.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", 1234567.0, 33, 44, 55);
    ASSERT (strcmp (result, "1.23457e+06 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37, "1.23432e-37" },
	{ 1.234321234321234e-36, "1.23432e-36" },
	{ 1.234321234321234e-35, "1.23432e-35" },
	{ 1.234321234321234e-34, "1.23432e-34" },
	{ 1.234321234321234e-33, "1.23432e-33" },
	{ 1.234321234321234e-32, "1.23432e-32" },
	{ 1.234321234321234e-31, "1.23432e-31" },
	{ 1.234321234321234e-30, "1.23432e-30" },
	{ 1.234321234321234e-29, "1.23432e-29" },
	{ 1.234321234321234e-28, "1.23432e-28" },
	{ 1.234321234321234e-27, "1.23432e-27" },
	{ 1.234321234321234e-26, "1.23432e-26" },
	{ 1.234321234321234e-25, "1.23432e-25" },
	{ 1.234321234321234e-24, "1.23432e-24" },
	{ 1.234321234321234e-23, "1.23432e-23" },
	{ 1.234321234321234e-22, "1.23432e-22" },
	{ 1.234321234321234e-21, "1.23432e-21" },
	{ 1.234321234321234e-20, "1.23432e-20" },
	{ 1.234321234321234e-19, "1.23432e-19" },
	{ 1.234321234321234e-18, "1.23432e-18" },
	{ 1.234321234321234e-17, "1.23432e-17" },
	{ 1.234321234321234e-16, "1.23432e-16" },
	{ 1.234321234321234e-15, "1.23432e-15" },
	{ 1.234321234321234e-14, "1.23432e-14" },
	{ 1.234321234321234e-13, "1.23432e-13" },
	{ 1.234321234321234e-12, "1.23432e-12" },
	{ 1.234321234321234e-11, "1.23432e-11" },
	{ 1.234321234321234e-10, "1.23432e-10" },
	{ 1.234321234321234e-9, "1.23432e-09" },
	{ 1.234321234321234e-8, "1.23432e-08" },
	{ 1.234321234321234e-7, "1.23432e-07" },
	{ 1.234321234321234e-6, "1.23432e-06" },
	{ 1.234321234321234e-5, "1.23432e-05" },
	{ 1.234321234321234e-4, "0.000123432" },
	{ 1.234321234321234e-3, "0.00123432" },
	{ 1.234321234321234e-2, "0.0123432" },
	{ 1.234321234321234e-1, "0.123432" },
	{ 1.234321234321234, "1.23432" },
	{ 1.234321234321234e1, "12.3432" },
	{ 1.234321234321234e2, "123.432" },
	{ 1.234321234321234e3, "1234.32" },
	{ 1.234321234321234e4, "12343.2" },
	{ 1.234321234321234e5, "123432" },
	{ 1.234321234321234e6, "1.23432e+06" },
	{ 1.234321234321234e7, "1.23432e+07" },
	{ 1.234321234321234e8, "1.23432e+08" },
	{ 1.234321234321234e9, "1.23432e+09" },
	{ 1.234321234321234e10, "1.23432e+10" },
	{ 1.234321234321234e11, "1.23432e+11" },
	{ 1.234321234321234e12, "1.23432e+12" },
	{ 1.234321234321234e13, "1.23432e+13" },
	{ 1.234321234321234e14, "1.23432e+14" },
	{ 1.234321234321234e15, "1.23432e+15" },
	{ 1.234321234321234e16, "1.23432e+16" },
	{ 1.234321234321234e17, "1.23432e+17" },
	{ 1.234321234321234e18, "1.23432e+18" },
	{ 1.234321234321234e19, "1.23432e+19" },
	{ 1.234321234321234e20, "1.23432e+20" },
	{ 1.234321234321234e21, "1.23432e+21" },
	{ 1.234321234321234e22, "1.23432e+22" },
	{ 1.234321234321234e23, "1.23432e+23" },
	{ 1.234321234321234e24, "1.23432e+24" },
	{ 1.234321234321234e25, "1.23432e+25" },
	{ 1.234321234321234e26, "1.23432e+26" },
	{ 1.234321234321234e27, "1.23432e+27" },
	{ 1.234321234321234e28, "1.23432e+28" },
	{ 1.234321234321234e29, "1.23432e+29" },
	{ 1.234321234321234e30, "1.23432e+30" },
	{ 1.234321234321234e31, "1.23432e+31" },
	{ 1.234321234321234e32, "1.23432e+32" },
	{ 1.234321234321234e33, "1.23432e+33" },
	{ 1.234321234321234e34, "1.23432e+34" },
	{ 1.234321234321234e35, "1.23432e+35" },
	{ 1.234321234321234e36, "1.23432e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%g", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", -0.03125, 33, 44, 55);
    ASSERT (strcmp (result, "-0.03125 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", -0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", 1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%g %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "      1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.75       33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "+1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, " 1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.75000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.g %d", 1.75, 33, 44, 55);
    ASSERT (strcmp (result, "2. 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.g %d", 9.75, 33, 44, 55);
    ASSERT (strcmp (result, "1.e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010g %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "0000001234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015g %d", -1.0 / 0.0, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015g %d", NaN (), 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.g %d", 1234.0, 33, 44, 55);
    ASSERT (strcmp (result, "1e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", 12.75L, 33, 44, 55);
    ASSERT (strcmp (result, "12.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* A larger positive number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", 1234567.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1.23457e+06 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Small and large positive numbers.  */
    static struct { long double value; const char *string; } data[] =
      {
	{ 1.234321234321234e-37L, "1.23432e-37" },
	{ 1.234321234321234e-36L, "1.23432e-36" },
	{ 1.234321234321234e-35L, "1.23432e-35" },
	{ 1.234321234321234e-34L, "1.23432e-34" },
	{ 1.234321234321234e-33L, "1.23432e-33" },
	{ 1.234321234321234e-32L, "1.23432e-32" },
	{ 1.234321234321234e-31L, "1.23432e-31" },
	{ 1.234321234321234e-30L, "1.23432e-30" },
	{ 1.234321234321234e-29L, "1.23432e-29" },
	{ 1.234321234321234e-28L, "1.23432e-28" },
	{ 1.234321234321234e-27L, "1.23432e-27" },
	{ 1.234321234321234e-26L, "1.23432e-26" },
	{ 1.234321234321234e-25L, "1.23432e-25" },
	{ 1.234321234321234e-24L, "1.23432e-24" },
	{ 1.234321234321234e-23L, "1.23432e-23" },
	{ 1.234321234321234e-22L, "1.23432e-22" },
	{ 1.234321234321234e-21L, "1.23432e-21" },
	{ 1.234321234321234e-20L, "1.23432e-20" },
	{ 1.234321234321234e-19L, "1.23432e-19" },
	{ 1.234321234321234e-18L, "1.23432e-18" },
	{ 1.234321234321234e-17L, "1.23432e-17" },
	{ 1.234321234321234e-16L, "1.23432e-16" },
	{ 1.234321234321234e-15L, "1.23432e-15" },
	{ 1.234321234321234e-14L, "1.23432e-14" },
	{ 1.234321234321234e-13L, "1.23432e-13" },
	{ 1.234321234321234e-12L, "1.23432e-12" },
	{ 1.234321234321234e-11L, "1.23432e-11" },
	{ 1.234321234321234e-10L, "1.23432e-10" },
	{ 1.234321234321234e-9L, "1.23432e-09" },
	{ 1.234321234321234e-8L, "1.23432e-08" },
	{ 1.234321234321234e-7L, "1.23432e-07" },
	{ 1.234321234321234e-6L, "1.23432e-06" },
	{ 1.234321234321234e-5L, "1.23432e-05" },
	{ 1.234321234321234e-4L, "0.000123432" },
	{ 1.234321234321234e-3L, "0.00123432" },
	{ 1.234321234321234e-2L, "0.0123432" },
	{ 1.234321234321234e-1L, "0.123432" },
	{ 1.234321234321234L, "1.23432" },
	{ 1.234321234321234e1L, "12.3432" },
	{ 1.234321234321234e2L, "123.432" },
	{ 1.234321234321234e3L, "1234.32" },
	{ 1.234321234321234e4L, "12343.2" },
	{ 1.234321234321234e5L, "123432" },
	{ 1.234321234321234e6L, "1.23432e+06" },
	{ 1.234321234321234e7L, "1.23432e+07" },
	{ 1.234321234321234e8L, "1.23432e+08" },
	{ 1.234321234321234e9L, "1.23432e+09" },
	{ 1.234321234321234e10L, "1.23432e+10" },
	{ 1.234321234321234e11L, "1.23432e+11" },
	{ 1.234321234321234e12L, "1.23432e+12" },
	{ 1.234321234321234e13L, "1.23432e+13" },
	{ 1.234321234321234e14L, "1.23432e+14" },
	{ 1.234321234321234e15L, "1.23432e+15" },
	{ 1.234321234321234e16L, "1.23432e+16" },
	{ 1.234321234321234e17L, "1.23432e+17" },
	{ 1.234321234321234e18L, "1.23432e+18" },
	{ 1.234321234321234e19L, "1.23432e+19" },
	{ 1.234321234321234e20L, "1.23432e+20" },
	{ 1.234321234321234e21L, "1.23432e+21" },
	{ 1.234321234321234e22L, "1.23432e+22" },
	{ 1.234321234321234e23L, "1.23432e+23" },
	{ 1.234321234321234e24L, "1.23432e+24" },
	{ 1.234321234321234e25L, "1.23432e+25" },
	{ 1.234321234321234e26L, "1.23432e+26" },
	{ 1.234321234321234e27L, "1.23432e+27" },
	{ 1.234321234321234e28L, "1.23432e+28" },
	{ 1.234321234321234e29L, "1.23432e+29" },
	{ 1.234321234321234e30L, "1.23432e+30" },
	{ 1.234321234321234e31L, "1.23432e+31" },
	{ 1.234321234321234e32L, "1.23432e+32" },
	{ 1.234321234321234e33L, "1.23432e+33" },
	{ 1.234321234321234e34L, "1.23432e+34" },
	{ 1.234321234321234e35L, "1.23432e+35" },
	{ 1.234321234321234e36L, "1.23432e+36" }
      };
    size_t k;
    for (k = 0; k < SIZEOF (data); k++)
      {
	char result[1000];
	int retval =
	  my_sprintf (result, "%Lg", data[k].value);
	ASSERT (strmatch (data[k].string, result));
	ASSERT (retval == strlen (result));
      }
  }

  { /* A negative number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", -0.03125L, 33, 44, 55);
    ASSERT (strcmp (result, "-0.03125 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative zero.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", -0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-0 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Positive infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", 1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "inf 33") == 0
	    || strcmp (result, "infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Negative infinity.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "-inf 33") == 0
	    || strcmp (result, "-infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%Lg %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Width.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%10Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "      1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_LEFT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%-10Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.75       33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SHOWSIGN.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%+Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "+1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_SPACE.  */
    char result[1000];
    int retval =
      my_sprintf (result, "% Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, " 1.75 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.75000 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.Lg %d", 1.75L, 33, 44, 55);
    ASSERT (strcmp (result, "2. 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ALT.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%#.Lg %d", 9.75L, 33, 44, 55);
    ASSERT (strcmp (result, "1.e+01 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with finite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%010Lg %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "0000001234 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with infinite number.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%015Lg %d", -1.0L / 0.0L, 33, 44, 55);
    ASSERT (strcmp (result, "           -inf 33") == 0
	    || strcmp (result, "      -infinity 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* FLAG_ZERO with NaN.  */
    static long double zero = 0.0L;
    char result[1000];
    int retval =
      my_sprintf (result, "%015Lg %d", zero / zero, 33, 44, 55);
    ASSERT (strcmp (result, "            nan 33") == 0);
    ASSERT (retval == strlen (result));
  }

  { /* Precision.  */
    char result[1000];
    int retval =
      my_sprintf (result, "%.Lg %d", 1234.0L, 33, 44, 55);
    ASSERT (strcmp (result, "1e+03 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the %n format directive.  */

  {
    int count = -1;
    char result[1000];
    int retval =
      my_sprintf (result, "%d %n", 123, &count, 33, 44, 55);
    ASSERT (strcmp (result, "123 ") == 0);
    ASSERT (retval == strlen (result));
    ASSERT (count == 4);
  }

  /* Test the support of the POSIX/XSI format strings with positions.  */

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%2$d %1$d", 33, 55);
    ASSERT (strcmp (result, "55 33") == 0);
    ASSERT (retval == strlen (result));
  }

  /* Test the support of the grouping flag.  */

  {
    char result[1000];
    int retval =
      my_sprintf (result, "%'d %d", 1234567, 99);
    ASSERT (result[strlen (result) - 1] == '9');
    ASSERT (retval == strlen (result));
  }
}
