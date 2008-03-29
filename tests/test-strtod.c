/*
 * Copyright (C) 2008 Free Software Foundation
 * Written by Eric Blake
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdlib.h>

#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
	{								     \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
	  /* FIXME abort ();*/status = 1;                               \
	}								     \
    }									     \
  while (0)

int
main ()
{
  int status = 0;
  /* Subject sequence empty or invalid.  */
  {
    errno = 0;
    const char input[] = "";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " ";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " +";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " .";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " .e0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " +.e-0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " in";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }
  {
    errno = 0;
    const char input[] = " na";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
  }

  /* Simple floating point values.  */
  {
    errno = 0;
    const char input[] = "1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1.";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = ".1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.1);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = " 1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "+1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "-1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == -1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e+0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e-0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e-1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.1);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }

  /* Zero.  */
  {
    errno = 0;
    const char input[] = "0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = ".0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0e0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0e+9999999";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 10);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0e-9999999";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 10);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "-0";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (signbit (result) == signbit (-0.0));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }

  /* Suffixes.  */
  {
    errno = 0;
    const char input[] = "1f";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1.f";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e+";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1e-";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "00x1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "-0x";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (signbit (result) == signbit (-0.0));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0xg";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0xp";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x.";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0xp+";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0xp+1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x.p+1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "1p+1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }

  /* Overflow/underflow.  */
  {
    errno = 0;
    const char input[] = "1E100000";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 8);
    ASSERT (errno == ERANGE);
  }
  {
    errno = 0;
    const char input[] = "-1E100000";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == -HUGE_VAL);
    ASSERT (ptr == input + 9);
    ASSERT (errno == ERANGE);
  }
  {
    errno = 0;
    const char input[] = "1E-100000";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (0.0 <= result && result <= FLT_MIN);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 9);
    ASSERT (errno == ERANGE);
  }
  {
    errno = 0;
    const char input[] = "-1E-100000";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (-FLT_MIN <= result && result <= 0.0);
    ASSERT (signbit (result) == signbit (-0.0));
    ASSERT (ptr == input + 10);
    ASSERT (errno == ERANGE);
  }

  /* Infinity.  */
  {
    errno = 0;
    const char input[] = "iNf";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "-InF";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == -HUGE_VAL);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "infinite";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "infinitY";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 8);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "infinitY.";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 8);
    ASSERT (errno == 0);
  }

  /* NaN.  Some processors set the sign bit of the default NaN, so all
     we check is that using a sign changes the result.  */
  {
    errno = 0;
    const char input[] = "-nan";
    char *ptr1;
    char *ptr2;
    double result1 = strtod (input, &ptr1);
    double result2 = strtod (input + 1, &ptr2);
#ifdef NAN
    ASSERT (isnan (result1));
    ASSERT (isnan (result2));
    ASSERT (signbit (result1) != signbit (result2));
    ASSERT (ptr1 == input + 4);
    ASSERT (ptr2 == input + 4);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    errno = 0;
    const char input[] = "+nan(";
    char *ptr1;
    char *ptr2;
    double result1 = strtod (input, &ptr1);
    double result2 = strtod (input + 1, &ptr2);
#ifdef NAN
    ASSERT (isnan (result1));
    ASSERT (isnan (result2));
    ASSERT (signbit (result1) == signbit (result2));
    ASSERT (ptr1 == input + 4);
    ASSERT (ptr2 == input + 4);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    errno = 0;
    const char input[] = "-nan()";
    char *ptr1;
    char *ptr2;
    double result1 = strtod (input, &ptr1);
    double result2 = strtod (input + 1, &ptr2);
#ifdef NAN
    ASSERT (isnan (result1));
    ASSERT (isnan (result2));
    ASSERT (signbit (result1) != signbit (result2));
    ASSERT (ptr1 == input + 6);
    ASSERT (ptr2 == input + 6);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    errno = 0;
    const char input[] = " nan().";
    char *ptr;
    double result = strtod (input, &ptr);
#ifdef NAN
    ASSERT (isnan (result));
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
#else
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    errno = 0;
    /* The behavior of nan(0) is implementation-defined, but all
       implementations we know of which handle optional
       n-char-sequences handle nan(0) the same as nan().  */
    const char input[] = "-nan(0).";
    char *ptr1;
    char *ptr2;
    double result1 = strtod (input, &ptr1);
    double result2 = strtod (input + 1, &ptr2);
#ifdef NAN
    ASSERT (isnan (result1));
    ASSERT (isnan (result2));
    ASSERT (signbit (result1) != signbit (result2));
    ASSERT (ptr1 == input + 7);
    ASSERT (ptr2 == input + 7);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }

  /* Hex.  */
#if 0
  /* TODO - gnulib doesn't implement this yet.  */
  {
    errno = 0;
    const char input[] = "0xa";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0XA";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x1p";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x1p+";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x1p+1";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 2.0);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
  {
    errno = 0;
    const char input[] = "0x1p+1a";
    char *ptr;
    double result = strtod (input, &ptr);
    ASSERT (result == 2.0);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
#endif

  /* Large buffers.  */
  {
    errno = 0;
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
	char *ptr;
	double result;
	memset (input, '\t', m - 1);
	input[m - 1] = '1';
	input[m] = '\0';
	result = strtod (input, &ptr);
	ASSERT (result == 1.0);
	ASSERT (ptr == input + m);
	ASSERT (errno == 0);
      }
    free (input);
  }
  {
    errno = 0;
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
	char *ptr;
	double result;
	memset (input, '0', m - 1);
	input[m - 1] = '1';
	input[m] = '\0';
	result = strtod (input, &ptr);
	ASSERT (result == 1.0);
	ASSERT (ptr == input + m);
	ASSERT (errno == 0);
      }
    free (input);
  }
#if 0
  /* Newlib has an artificial limit of 20000 for the exponent.  TODO -
     gnulib should fix this.  */
  {
    errno = 0;
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
	char *ptr;
	double result;
	input[0] = '.';
	memset (input + 1, '0', m - 10);
	input[m - 9] = '1';
	input[m - 8] = 'e';
	input[m - 7] = '+';
	input[m - 6] = '9';
	input[m - 5] = '9';
	input[m - 4] = '9';
	input[m - 3] = '9';
	input[m - 2] = '9';
	input[m - 1] = '1';
	input[m] = '\0';
	result = strtod (input, &ptr);
	ASSERT (result == 1.0);
	ASSERT (ptr == input + m);
	ASSERT (errno == 0);
      }
    free (input);
  }
  {
    errno = 0;
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
	char *ptr;
	double result;
	input[0] = '1';
	memset (input + 1, '0', m - 9);
	input[m - 8] = 'e';
	input[m - 7] = '-';
	input[m - 6] = '9';
	input[m - 5] = '9';
	input[m - 4] = '9';
	input[m - 3] = '9';
	input[m - 2] = '9';
	input[m - 1] = '1';
	input[m] = '\0';
	result = strtod (input, &ptr);
	ASSERT (result == 1.0);
	ASSERT (ptr == input + m);
	ASSERT (errno == 0);
      }
    free (input);
  }
#endif
  {
    errno = 0;
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
	char *ptr;
	double result;
	input[0] = '-';
	input[1] = '0';
	input[2] = 'e';
	input[3] = '1';
	memset (input + 4, '0', m - 3);
	input[m] = '\0';
	result = strtod (input, &ptr);
	ASSERT (result == 0.0);
	ASSERT (signbit (result) == signbit (-0.0));
	ASSERT (ptr == input + m);
	ASSERT (errno == 0);
      }
    free (input);
  }

  /* Rounding.  */
  /* TODO - is it worth some tests of rounding for typical IEEE corner
     cases, such as .5 ULP rounding up to the smallest denormal and
     not causing underflow, or FLT_MIN - .5 ULP not causing an
     infinite loop?  */

  return status;
}
