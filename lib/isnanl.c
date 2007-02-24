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

#define LDBL_EXP_MASK ((LDBL_MAX_EXP - LDBL_MIN_EXP) | 7)

#define NWORDS \
  ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { long double value; unsigned int word[NWORDS]; }
        memory_long_double;

int
rpl_isnanl (long double x)
{
#if defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
  /* Be careful to not do any floating-point operation on x, such as x == x,
     because x may be a signalling NaN.  */
  static memory_long_double nan = { 0.0L / 0.0L };
  static long double plus_inf = 1.0L / 0.0L;
  static long double minus_inf = -1.0L / 0.0L;
  memory_long_double m;

  /* A NaN can be recognized through its exponent.  But exclude +Infinity and
     -Infinity, which have the same exponent.  */
  m.value = x;
  if ((((m.word[LDBL_EXPBIT0_WORD] >> LDBL_EXPBIT0_BIT)
	^ (nan.word[LDBL_EXPBIT0_WORD] >> LDBL_EXPBIT0_BIT))
       & LDBL_EXP_MASK)
      == 0)
    return (memcmp (&m.value, &plus_inf, sizeof (long double)) != 0
	    && memcmp (&m.value, &minus_inf, sizeof (long double)) != 0);
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
