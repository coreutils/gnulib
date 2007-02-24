/* Test for NaN that does not need libm.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <float.h>
#include <string.h>

#define DBL_EXP_MASK ((DBL_MAX_EXP - DBL_MIN_EXP) | 7)

#define NWORDS \
  ((sizeof (double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { double value; unsigned int word[NWORDS]; } memory_double;

int
rpl_isnan (double x)
{
#if defined DBL_EXPBIT0_WORD && defined DBL_EXPBIT0_BIT
  /* Be careful to not do any floating-point operation on x, such as x == x,
     because x may be a signaling NaN.  */
  static memory_double nan = { 0.0 / 0.0 };
  static double plus_inf = 1.0 / 0.0;
  static double minus_inf = -1.0 / 0.0;
  memory_double m;

  /* A NaN can be recognized through its exponent.  But exclude +Infinity and
     -Infinity, which have the same exponent.  */
  m.value = x;
  if ((((m.word[DBL_EXPBIT0_WORD] >> DBL_EXPBIT0_BIT)
	^ (nan.word[DBL_EXPBIT0_WORD] >> DBL_EXPBIT0_BIT))
       & DBL_EXP_MASK)
      == 0)
    return (memcmp (&m.value, &plus_inf, sizeof (double)) != 0
	    && memcmp (&m.value, &minus_inf, sizeof (double)) != 0);
  else
    return 0;
#else
  /* The configuration did not find sufficient information.  Give up about
     the signaling NaNs, handle only the quiet NaNs.  */
  if (x == x)
    return 0;
  else
    return 1;
#endif
}
