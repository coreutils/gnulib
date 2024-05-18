/* Test of isfinite() substitute.
   Copyright (C) 2007-2024 Free Software Foundation, Inc.

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

/* Written by Ben Pfaff, 2007, using Bruno Haible's code as a
   template. */

#include <config.h>

#include <math.h>

/* isfinite must be a macro.  */
#ifndef isfinite
# error missing declaration
#endif

#include <float.h>
#include <limits.h>

#include "infinity.h"
#include "snan.h"
#include "macros.h"

float zerof = 0.0f;
double zerod = 0.0;
long double zerol = 0.0L;

static void
test_isfinitef ()
{
  /* Zero. */
  ASSERT (isfinite (0.0f));
  /* Subnormal values. */
  ASSERT (isfinite (FLT_MIN / 2));
  ASSERT (isfinite (-FLT_MIN / 2));
  /* Finite values.  */
  ASSERT (isfinite (3.141f));
  ASSERT (isfinite (3.141e30f));
  ASSERT (isfinite (3.141e-30f));
  ASSERT (isfinite (-2.718f));
  ASSERT (isfinite (-2.718e30f));
  ASSERT (isfinite (-2.718e-30f));
  /* Infinite values.  */
  ASSERT (!isfinite (Infinityf ()));
  ASSERT (!isfinite (- Infinityf ()));
  /* Quiet NaN.  */
  ASSERT (!isfinite (zerof / zerof));
#if HAVE_SNANF
  /* Signalling NaN.  */
  ASSERT (!isfinite (SNaNf ()));
#endif
}

static void
test_isfinited ()
{
  /* Zero. */
  ASSERT (isfinite (0.0));
  /* Subnormal values. */
  ASSERT (isfinite (DBL_MIN / 2));
  ASSERT (isfinite (-DBL_MIN / 2));
  /* Finite values. */
  ASSERT (isfinite (3.141));
  ASSERT (isfinite (3.141e30));
  ASSERT (isfinite (3.141e-30));
  ASSERT (isfinite (-2.718));
  ASSERT (isfinite (-2.718e30));
  ASSERT (isfinite (-2.718e-30));
  /* Infinite values.  */
  ASSERT (!isfinite (Infinityd ()));
  ASSERT (!isfinite (- Infinityd ()));
  /* Quiet NaN.  */
  ASSERT (!isfinite (zerod / zerod));
#if HAVE_SNAND
  /* Signalling NaN.  */
  ASSERT (!isfinite (SNaNd ()));
#endif
}

static void
test_isfinitel ()
{
  /* Zero. */
  ASSERT (isfinite (0.0L));
  /* Subnormal values. */
  ASSERT (isfinite (LDBL_MIN / 2));
  ASSERT (isfinite (-LDBL_MIN / 2));
  /* Finite values. */
  ASSERT (isfinite (3.141L));
  ASSERT (isfinite (3.141e30L));
  ASSERT (isfinite (3.141e-30L));
  ASSERT (isfinite (-2.718L));
  ASSERT (isfinite (-2.718e30L));
  ASSERT (isfinite (-2.718e-30L));
  /* Infinite values.  */
  ASSERT (!isfinite (Infinityl ()));
  ASSERT (!isfinite (- Infinityl ()));
  /* Quiet NaN.  */
  ASSERT (!isfinite (zerol / zerol));
#if HAVE_SNANL
  /* Signalling NaN.  */
  ASSERT (!isfinite (SNaNl ()));
#endif

#if ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
  #define NWORDS \
    ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
  typedef union { unsigned int word[NWORDS]; long double value; }
          memory_long_double;
/* Representation of an 80-bit 'long double' as an initializer for a sequence
   of 'unsigned int' words.  */
# ifdef WORDS_BIGENDIAN
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { ((unsigned int) (exponent) << 16) | ((unsigned int) (manthi) >> 16), \
       ((unsigned int) (manthi) << 16) | ((unsigned int) (mantlo) >> 16),   \
       (unsigned int) (mantlo) << 16                                        \
     }
# else
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { mantlo, manthi, exponent }
# endif
  { /* Quiet NaN.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    ASSERT (!isfinite (x.value));
  }
  {
    /* Signalling NaN.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    ASSERT (!isfinite (x.value));
  }
  /* isfinite should return something for noncanonical values.  */
  { /* Pseudo-NaN.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    ASSERT (isfinite (x.value) || !isfinite (x.value));
  }
  { /* Pseudo-Infinity.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    ASSERT (isfinite (x.value) || !isfinite (x.value));
  }
  { /* Pseudo-Zero.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    ASSERT (isfinite (x.value) || !isfinite (x.value));
  }
  { /* Unnormalized number.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    ASSERT (isfinite (x.value) || !isfinite (x.value));
  }
  { /* Pseudo-Denormal.  */
    static memory_long_double x =
      { .word = LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    ASSERT (isfinite (x.value) || !isfinite (x.value));
  }
  #undef NWORDS
#endif
}

int
main ()
{
  test_isfinitef ();
  test_isfinited ();
  test_isfinitel ();
  return test_exit_status;
}
