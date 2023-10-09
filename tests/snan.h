/* Macros for signalling not-a-number.
   Copyright (C) 2007-2023 Free Software Foundation, Inc.

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

#include <float.h>
#include <limits.h>
#include <math.h>

#include "nan.h"


#if defined FLT_EXPBIT0_WORD && defined FLT_EXPBIT0_BIT

# define HAVE_SNANF 1

/* Returns a signalling 'float' NaN.  */
_GL_UNUSED static float
SNaNf ()
{
  #define NWORDS \
    ((sizeof (float) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  typedef union { float value; unsigned int word[NWORDS]; } memory_float;
  memory_float m;
  m.value = NaNf ();
 #if FLT_EXPBIT0_BIT > 0
    m.word[FLT_EXPBIT0_WORD] ^= (unsigned int) 1 << (FLT_EXPBIT0_BIT - 1);
 #else
    m.word[FLT_EXPBIT0_WORD + (FLT_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
 #endif
  if (FLT_EXPBIT0_WORD < NWORDS / 2)
    m.word[FLT_EXPBIT0_WORD + 1] |= (unsigned int) 1 << FLT_EXPBIT0_BIT;
  else
    m.word[0] |= (unsigned int) 1;
  #undef NWORDS
  return m.value;
}

#endif


#if defined DBL_EXPBIT0_WORD && defined DBL_EXPBIT0_BIT

# define HAVE_SNAND 1

/* Returns a signalling 'double' NaN.  */
_GL_UNUSED static double
SNaNd ()
{
  #define NWORDS \
    ((sizeof (double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  typedef union { double value; unsigned int word[NWORDS]; } memory_double;
  memory_double m;
  m.value = NaNd ();
  #if DBL_EXPBIT0_BIT > 0
    m.word[DBL_EXPBIT0_WORD] ^= (unsigned int) 1 << (DBL_EXPBIT0_BIT - 1);
  #else
    m.word[DBL_EXPBIT0_WORD + (DBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
  #endif
  m.word[DBL_EXPBIT0_WORD + (DBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
    |= (unsigned int) 1 << DBL_EXPBIT0_BIT;
  #undef NWORDS
  return m.value;
}

#endif


#if defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT

# define HAVE_SNANL 1

/* Returns a signalling 'long double' NaN.  */
_GL_UNUSED static long double
SNaNl ()
{
  /* A bit pattern that is different from a Quiet NaN.  With a bit of luck,
     it's a Signalling NaN.  */
  #define NWORDS \
    ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  typedef union { unsigned int word[NWORDS]; long double value; }
          memory_long_double;
  memory_long_double m;
  m.value = NaNl ();
  #if defined __powerpc__ && LDBL_MANT_DIG == 106
    /* This is PowerPC "double double", a pair of two doubles.  Inf and NaN are
       represented as the corresponding 64-bit IEEE values in the first double;
       the second is ignored.  Manipulate only the first double.  */
    #define HNWORDS \
      ((sizeof (double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  #else
    #define HNWORDS NWORDS
  #endif
  #if LDBL_EXPBIT0_BIT > 0
    m.word[LDBL_EXPBIT0_WORD] ^= (unsigned int) 1 << (LDBL_EXPBIT0_BIT - 1);
  #else
    m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < HNWORDS / 2 ? 1 : - 1)]
      ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
  #endif
  m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < HNWORDS / 2 ? 1 : - 1)]
    |= (unsigned int) 1 << LDBL_EXPBIT0_BIT;
  #undef HNWORDS
  #undef NWORDS
  return m.value;
}

#endif
