/* Test of splitting a float into fraction and mantissa.
   Copyright (C) 2007-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <config.h>

#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (frexpf, float, (float, int *));

#include <float.h>

#include "isnanf-nolibm.h"
#include "minus-zero.h"
#include "infinity.h"
#include "nan.h"
#include "macros.h"

/* Avoid some warnings from "gcc -Wshadow".
   This file doesn't use the exp() function.  */
#undef exp
#define exp exponent

static float
my_ldexp (float x, int d)
{
  for (; d > 0; d--)
    x *= 2.0f;
  for (; d < 0; d++)
    x *= 0.5f;
  return x;
}

int
main ()
{
  int i;
  volatile float x;

  { /* NaN.  */
    int exp = -9999;
    float mantissa;
    x = NaNf ();
    mantissa = frexpf (x, &exp);
    ASSERT (isnanf (mantissa));
  }

  { /* Positive infinity.  */
    int exp = -9999;
    float mantissa;
    x = Infinityf ();
    mantissa = frexpf (x, &exp);
    ASSERT (mantissa == x);
  }

  { /* Negative infinity.  */
    int exp = -9999;
    float mantissa;
    x = - Infinityf ();
    mantissa = frexpf (x, &exp);
    ASSERT (mantissa == x);
  }

  { /* Positive zero.  */
    int exp = -9999;
    float mantissa;
    x = 0.0f;
    mantissa = frexpf (x, &exp);
    ASSERT (exp == 0);
    ASSERT (mantissa == x);
    ASSERT (!signbit (mantissa));
  }

  { /* Negative zero.  */
    int exp = -9999;
    float mantissa;
    x = minus_zerof;
    mantissa = frexpf (x, &exp);
    ASSERT (exp == 0);
    ASSERT (mantissa == x);
    ASSERT (signbit (mantissa));
  }

  for (i = 1, x = 1.0f; i <= FLT_MAX_EXP; i++, x *= 2.0f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.5f);
    }
  for (i = 1, x = 1.0f; i >= FLT_MIN_EXP; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.5f);
    }
  for (; i >= FLT_MIN_EXP - 100 && x > 0.0f; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.5f);
    }

  for (i = 1, x = -1.0f; i <= FLT_MAX_EXP; i++, x *= 2.0f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == -0.5f);
    }
  for (i = 1, x = -1.0f; i >= FLT_MIN_EXP; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == -0.5f);
    }
  for (; i >= FLT_MIN_EXP - 100 && x < 0.0f; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == -0.5f);
    }

  for (i = 1, x = 1.01f; i <= FLT_MAX_EXP; i++, x *= 2.0f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.505f);
    }
  for (i = 1, x = 1.01f; i >= FLT_MIN_EXP; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.505f);
    }
  for (; i >= FLT_MIN_EXP - 100 && x > 0.0f; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa >= 0.5f);
      ASSERT (mantissa < 1.0f);
      ASSERT (mantissa == my_ldexp (x, - exp));
    }

  for (i = 1, x = 1.73205f; i <= FLT_MAX_EXP; i++, x *= 2.0f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.866025f);
    }
  for (i = 1, x = 1.73205f; i >= FLT_MIN_EXP; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i);
      ASSERT (mantissa == 0.866025f);
    }
  for (; i >= FLT_MIN_EXP - 100 && x > 0.0f; i--, x *= 0.5f)
    {
      int exp = -9999;
      float mantissa = frexpf (x, &exp);
      ASSERT (exp == i || exp == i + 1);
      ASSERT (mantissa >= 0.5f);
      ASSERT (mantissa < 1.0f);
      ASSERT (mantissa == my_ldexp (x, - exp));
    }

  return 0;
}
