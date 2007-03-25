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

#include "float+.h"

#ifdef USE_LONG_DOUBLE
# define FUNC rpl_isnanl
# define DOUBLE long double
# define MAX_EXP LDBL_MAX_EXP
# define MIN_EXP LDBL_MIN_EXP
# if defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
#  define KNOWN_EXPBIT0_LOCATION
#  define EXPBIT0_WORD LDBL_EXPBIT0_WORD
#  define EXPBIT0_BIT LDBL_EXPBIT0_BIT
# endif
# define SIZE SIZEOF_LDBL
# define L_(literal) literal##L
#else
# define FUNC rpl_isnan
# define DOUBLE double
# define MAX_EXP DBL_MAX_EXP
# define MIN_EXP DBL_MIN_EXP
# if defined DBL_EXPBIT0_WORD && defined DBL_EXPBIT0_BIT
#  define KNOWN_EXPBIT0_LOCATION
#  define EXPBIT0_WORD DBL_EXPBIT0_WORD
#  define EXPBIT0_BIT DBL_EXPBIT0_BIT
# endif
# define SIZE SIZEOF_DBL
# define L_(literal) literal
#endif

#define EXP_MASK ((MAX_EXP - MIN_EXP) | 7)

#define NWORDS \
  ((sizeof (DOUBLE) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { DOUBLE value; unsigned int word[NWORDS]; } memory_double;

int
FUNC (DOUBLE x)
{
#ifdef KNOWN_EXPBIT0_LOCATION
  /* Be careful to not do any floating-point operation on x, such as x == x,
     because x may be a signaling NaN.  */
# if defined __SUNPRO_C || defined __DECC
  /* The Sun C 5.0 compilers and the Compaq (ex-DEC) 6.4 compilers don't
     recognize the initializers as constant expressions.  The latter compiler
     also fails when constant-folding 0.0 / 0.0 even when constant-folding is
     not required.  */
  static DOUBLE zero = L_(0.0);
  memory_double nan;
  DOUBLE plus_inf = L_(1.0) / L_(0.0);
  DOUBLE minus_inf = -L_(1.0) / L_(0.0);
  nan.value = zero / zero;
# else
  static memory_double nan = { L_(0.0) / L_(0.0) };
  static DOUBLE plus_inf = L_(1.0) / L_(0.0);
  static DOUBLE minus_inf = -L_(1.0) / L_(0.0);
# endif
  {
    memory_double m;

    /* A NaN can be recognized through its exponent.  But exclude +Infinity and
       -Infinity, which have the same exponent.  */
    m.value = x;
    if (((m.word[EXPBIT0_WORD] ^ nan.word[EXPBIT0_WORD])
	 & (EXP_MASK << EXPBIT0_BIT))
	== 0)
      return (memcmp (&m.value, &plus_inf, SIZE) != 0
	      && memcmp (&m.value, &minus_inf, SIZE) != 0);
    else
      return 0;
  }
#else
  /* The configuration did not find sufficient information.  Give up about
     the signaling NaNs, handle only the quiet NaNs.  */
  if (x == x)
    return 0;
  else
    return 1;
#endif
}
