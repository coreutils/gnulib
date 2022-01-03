/* Fused multiply-add.
   Copyright (C) 2007, 2009, 2011-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2011.  */

#if ! defined USE_LONG_DOUBLE
# include <config.h>
#endif

/* Specification.  */
#include <math.h>

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#if HAVE_FEGETROUND
# include <fenv.h>
#endif

#include "float+.h"
#include "integer_length.h"
#include "verify.h"

#ifdef USE_LONG_DOUBLE
# define FUNC fmal
# define DOUBLE long double
# define FREXP frexpl
# define LDEXP ldexpl
# define MIN_EXP LDBL_MIN_EXP
# define MANT_BIT LDBL_MANT_BIT
# define L_(literal) literal##L
#elif ! defined USE_FLOAT
# define FUNC fma
# define DOUBLE double
# define FREXP frexp
# define LDEXP ldexp
# define MIN_EXP DBL_MIN_EXP
# define MANT_BIT DBL_MANT_BIT
# define L_(literal) literal
#else /* defined USE_FLOAT */
# define FUNC fmaf
# define DOUBLE float
# define FREXP frexpf
# define LDEXP ldexpf
# define MIN_EXP FLT_MIN_EXP
# define MANT_BIT FLT_MANT_BIT
# define L_(literal) literal##f
#endif

#undef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#undef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* MSVC with option -fp:strict refuses to compile constant initializers that
   contain floating-point operations.  Pacify this compiler.  */
#if defined _MSC_VER && !defined __clang__
# pragma fenv_access (off)
#endif

/* Work around GCC 4.2.1 bug on OpenBSD 5.1/SPARC64.  */
#if defined __GNUC__ && defined __sparc__
# define VOLATILE volatile
#else
# define VOLATILE
#endif

/* It is possible to write an implementation of fused multiply-add with
   floating-point operations alone.  See
     Sylvie Boldo, Guillaume Melquiond:
     Emulation of FMA and correctly-rounded sums: proved algorithms using
     rounding to odd.
     <https://www.lri.fr/~melquion/doc/08-tc.pdf>
   But is it complicated.
   Here we take the simpler (and probably slower) approach of doing
   multi-precision arithmetic.  */

/* We use the naming conventions of GNU gmp, but vastly simpler (and slower)
   algorithms.  */

typedef unsigned int mp_limb_t;
#define GMP_LIMB_BITS 32
verify (sizeof (mp_limb_t) * CHAR_BIT == GMP_LIMB_BITS);

typedef unsigned long long mp_twolimb_t;
#define GMP_TWOLIMB_BITS 64
verify (sizeof (mp_twolimb_t) * CHAR_BIT == GMP_TWOLIMB_BITS);

/* Number of limbs needed for a single DOUBLE.  */
#define NLIMBS1 ((MANT_BIT + GMP_LIMB_BITS - 1) / GMP_LIMB_BITS)

/* Number of limbs needed for the accumulator.  */
#define NLIMBS3 (3 * NLIMBS1 + 1)

/* Assuming 0.5 <= x < 1.0:
   Convert the mantissa (x * 2^DBL_MANT_BIT) to a sequence of limbs.  */
static void
decode (DOUBLE x, mp_limb_t limbs[NLIMBS1])
{
  /* I'm not sure whether it's safe to cast a 'double' value between
     2^31 and 2^32 to 'unsigned int', therefore play safe and cast only
     'double' values between 0 and 2^31 (to 'unsigned int' or 'int',
     doesn't matter).
     So, we split the MANT_BIT bits of x into a number of chunks of
     at most 31 bits.  */
  enum { chunk_count = (MANT_BIT - 1) / 31 + 1 };
  /* Variables used for storing the bits limb after limb.  */
  mp_limb_t *p = limbs + NLIMBS1 - 1;
  mp_limb_t accu = 0;
  unsigned int bits_needed = MANT_BIT - (NLIMBS1 - 1) * GMP_LIMB_BITS;
  /* The bits bits_needed-1...0 need to be ORed into the accu.
     1 <= bits_needed <= GMP_LIMB_BITS.  */
  /* Unroll the first 4 loop rounds.  */
  if (chunk_count > 0)
    {
      /* Here we still have MANT_BIT-0*31 bits to extract from x.  */
      enum { chunk_bits = MIN (31, MANT_BIT - 0 * 31) }; /* > 0, <= 31 */
      mp_limb_t d;
      x *= (mp_limb_t) 1 << chunk_bits;
      d = (int) x; /* 0 <= d < 2^chunk_bits.  */
      x -= d;
      if (!(x >= L_(0.0) && x < L_(1.0)))
        abort ();
      if (bits_needed < chunk_bits)
        {
          /* store bits_needed bits */
          accu |= d >> (chunk_bits - bits_needed);
          *p = accu;
          if (p == limbs)
            goto done;
          p--;
          /* hold (chunk_bits - bits_needed) bits */
          accu = d << (GMP_LIMB_BITS - (chunk_bits - bits_needed));
          bits_needed = GMP_LIMB_BITS - (chunk_bits - bits_needed);
        }
      else
        {
          /* store chunk_bits bits */
          accu |= d << (bits_needed - chunk_bits);
          bits_needed -= chunk_bits;
          if (bits_needed == 0)
            {
              *p = accu;
              if (p == limbs)
                goto done;
              p--;
              accu = 0;
              bits_needed = GMP_LIMB_BITS;
            }
        }
    }
  if (chunk_count > 1)
    {
      /* Here we still have MANT_BIT-1*31 bits to extract from x.  */
      enum { chunk_bits = MIN (31, MAX (MANT_BIT - 1 * 31, 0)) }; /* > 0, <= 31 */
      mp_limb_t d;
      x *= (mp_limb_t) 1 << chunk_bits;
      d = (int) x; /* 0 <= d < 2^chunk_bits.  */
      x -= d;
      if (!(x >= L_(0.0) && x < L_(1.0)))
        abort ();
      if (bits_needed < chunk_bits)
        {
          /* store bits_needed bits */
          accu |= d >> (chunk_bits - bits_needed);
          *p = accu;
          if (p == limbs)
            goto done;
          p--;
          /* hold (chunk_bits - bits_needed) bits */
          accu = d << (GMP_LIMB_BITS - (chunk_bits - bits_needed));
          bits_needed = GMP_LIMB_BITS - (chunk_bits - bits_needed);
        }
      else
        {
          /* store chunk_bits bits */
          accu |= d << (bits_needed - chunk_bits);
          bits_needed -= chunk_bits;
          if (bits_needed == 0)
            {
              *p = accu;
              if (p == limbs)
                goto done;
              p--;
              accu = 0;
              bits_needed = GMP_LIMB_BITS;
            }
        }
    }
  if (chunk_count > 2)
    {
      /* Here we still have MANT_BIT-2*31 bits to extract from x.  */
      enum { chunk_bits = MIN (31, MAX (MANT_BIT - 2 * 31, 0)) }; /* > 0, <= 31 */
      mp_limb_t d;
      x *= (mp_limb_t) 1 << chunk_bits;
      d = (int) x; /* 0 <= d < 2^chunk_bits.  */
      x -= d;
      if (!(x >= L_(0.0) && x < L_(1.0)))
        abort ();
      if (bits_needed < chunk_bits)
        {
          /* store bits_needed bits */
          accu |= d >> (chunk_bits - bits_needed);
          *p = accu;
          if (p == limbs)
            goto done;
          p--;
          /* hold (chunk_bits - bits_needed) bits */
          accu = d << (GMP_LIMB_BITS - (chunk_bits - bits_needed));
          bits_needed = GMP_LIMB_BITS - (chunk_bits - bits_needed);
        }
      else
        {
          /* store chunk_bits bits */
          accu |= d << (bits_needed - chunk_bits);
          bits_needed -= chunk_bits;
          if (bits_needed == 0)
            {
              *p = accu;
              if (p == limbs)
                goto done;
              p--;
              accu = 0;
              bits_needed = GMP_LIMB_BITS;
            }
        }
    }
  if (chunk_count > 3)
    {
      /* Here we still have MANT_BIT-3*31 bits to extract from x.  */
      enum { chunk_bits = MIN (31, MAX (MANT_BIT - 3 * 31, 0)) }; /* > 0, <= 31 */
      mp_limb_t d;
      x *= (mp_limb_t) 1 << chunk_bits;
      d = (int) x; /* 0 <= d < 2^chunk_bits.  */
      x -= d;
      if (!(x >= L_(0.0) && x < L_(1.0)))
        abort ();
      if (bits_needed < chunk_bits)
        {
          /* store bits_needed bits */
          accu |= d >> (chunk_bits - bits_needed);
          *p = accu;
          if (p == limbs)
            goto done;
          p--;
          /* hold (chunk_bits - bits_needed) bits */
          accu = d << (GMP_LIMB_BITS - (chunk_bits - bits_needed));
          bits_needed = GMP_LIMB_BITS - (chunk_bits - bits_needed);
        }
      else
        {
          /* store chunk_bits bits */
          accu |= d << (bits_needed - chunk_bits);
          bits_needed -= chunk_bits;
          if (bits_needed == 0)
            {
              *p = accu;
              if (p == limbs)
                goto done;
              p--;
              accu = 0;
              bits_needed = GMP_LIMB_BITS;
            }
        }
    }
  if (chunk_count > 4)
    {
      /* Here we still have MANT_BIT-4*31 bits to extract from x.  */
      /* Generic loop.  */
      size_t k;
      for (k = 4; k < chunk_count; k++)
        {
          size_t chunk_bits = MIN (31, MANT_BIT - k * 31); /* > 0, <= 31 */
          mp_limb_t d;
          x *= (mp_limb_t) 1 << chunk_bits;
          d = (int) x; /* 0 <= d < 2^chunk_bits.  */
          x -= d;
          if (!(x >= L_(0.0) && x < L_(1.0)))
            abort ();
          if (bits_needed < chunk_bits)
            {
              /* store bits_needed bits */
              accu |= d >> (chunk_bits - bits_needed);
              *p = accu;
              if (p == limbs)
                goto done;
              p--;
              /* hold (chunk_bits - bits_needed) bits */
              accu = d << (GMP_LIMB_BITS - (chunk_bits - bits_needed));
              bits_needed = GMP_LIMB_BITS - (chunk_bits - bits_needed);
            }
          else
            {
              /* store chunk_bits bits */
              accu |= d << (bits_needed - chunk_bits);
              bits_needed -= chunk_bits;
              if (bits_needed == 0)
                {
                  *p = accu;
                  if (p == limbs)
                    goto done;
                  p--;
                  accu = 0;
                  bits_needed = GMP_LIMB_BITS;
                }
            }
        }
    }
  /* We shouldn't get here.  */
  abort ();

 done: ;
#ifndef USE_LONG_DOUBLE /* On FreeBSD 6.1/x86, 'long double' numbers sometimes
                           have excess precision.  */
  if (!(x == L_(0.0)))
    abort ();
#endif
}

/* Multiply two sequences of limbs.  */
static void
multiply (mp_limb_t xlimbs[NLIMBS1], mp_limb_t ylimbs[NLIMBS1],
          mp_limb_t prod_limbs[2 * NLIMBS1])
{
  size_t k, i, j;
  enum { len1 = NLIMBS1 };
  enum { len2 = NLIMBS1 };

  for (k = len2; k > 0; )
    prod_limbs[--k] = 0;
  for (i = 0; i < len1; i++)
    {
      mp_limb_t digit1 = xlimbs[i];
      mp_twolimb_t carry = 0;
      for (j = 0; j < len2; j++)
        {
          mp_limb_t digit2 = ylimbs[j];
          carry += (mp_twolimb_t) digit1 * (mp_twolimb_t) digit2;
          carry += prod_limbs[i + j];
          prod_limbs[i + j] = (mp_limb_t) carry;
          carry = carry >> GMP_LIMB_BITS;
        }
      prod_limbs[i + len2] = (mp_limb_t) carry;
    }
}

DOUBLE
FUNC (DOUBLE x, DOUBLE y, DOUBLE z)
{
  if (isfinite (x) && isfinite (y))
    {
      if (isfinite (z))
        {
          /* x, y, z are all finite.  */
          if (x == L_(0.0) || y == L_(0.0))
            return z;
          if (z == L_(0.0))
            return x * y;
          /* x, y, z are all non-zero.
             The result is x * y + z.  */
          {
            int e;                  /* exponent of x * y + z */
            int sign;
            mp_limb_t sum[NLIMBS3];
            size_t sum_len;

            {
              int xys;                /* sign of x * y */
              int zs;                 /* sign of z */
              int xye;                /* sum of exponents of x and y */
              int ze;                 /* exponent of z */
              mp_limb_t summand1[NLIMBS3];
              size_t summand1_len;
              mp_limb_t summand2[NLIMBS3];
              size_t summand2_len;

              {
                mp_limb_t zlimbs[NLIMBS1];
                mp_limb_t xylimbs[2 * NLIMBS1];

                {
                  DOUBLE xn;              /* normalized part of x */
                  DOUBLE yn;              /* normalized part of y */
                  DOUBLE zn;              /* normalized part of z */
                  int xe;                 /* exponent of x */
                  int ye;                 /* exponent of y */
                  mp_limb_t xlimbs[NLIMBS1];
                  mp_limb_t ylimbs[NLIMBS1];

                  xys = 0;
                  xn = x;
                  if (x < 0)
                    {
                      xys = 1;
                      xn = - x;
                    }
                  yn = y;
                  if (y < 0)
                    {
                      xys = 1 - xys;
                      yn = - y;
                    }

                  zs = 0;
                  zn = z;
                  if (z < 0)
                    {
                      zs = 1;
                      zn = - z;
                    }

                  /* xn, yn, zn are all positive.
                     The result is (-1)^xys * xn * yn + (-1)^zs * zn.  */
                  xn = FREXP (xn, &xe);
                  yn = FREXP (yn, &ye);
                  zn = FREXP (zn, &ze);
                  xye = xe + ye;
                  /* xn, yn, zn are all < 1.0 and >= 0.5.
                     The result is
                       (-1)^xys * 2^xye * xn * yn + (-1)^zs * 2^ze * zn.  */
                  if (xye < ze - MANT_BIT)
                    {
                      /* 2^xye * xn * yn < 2^xye <= 2^(ze-MANT_BIT-1)  */
                      return z;
                    }
                  if (xye - 2 * MANT_BIT > ze)
                    {
                      /* 2^ze * zn < 2^ze <= 2^(xye-2*MANT_BIT-1).
                         We cannot simply do
                           return x * y;
                         because it would round differently: A round-to-even
                         in the multiplication can be a round-up or round-down
                         here, due to z.  So replace z with a value that doesn't
                         require the use of long bignums but that rounds the
                         same way.  */
                      zn = L_(0.5);
                      ze = xye - 2 * MANT_BIT - 1;
                    }
                  /* Convert mantissas of xn, yn, zn to limb sequences:
                     xlimbs = 2^MANT_BIT * xn
                     ylimbs = 2^MANT_BIT * yn
                     zlimbs = 2^MANT_BIT * zn  */
                  decode (xn, xlimbs);
                  decode (yn, ylimbs);
                  decode (zn, zlimbs);
                  /* Multiply the mantissas of xn and yn:
                     xylimbs = xlimbs * ylimbs  */
                  multiply (xlimbs, ylimbs, xylimbs);
                }
                /* The result is
                     (-1)^xys * 2^(xye-2*MANT_BIT) * xylimbs
                     + (-1)^zs * 2^(ze-MANT_BIT) * zlimbs.
                   Compute
                     e = min (xye-2*MANT_BIT, ze-MANT_BIT)
                   then
                     summand1 = 2^(xye-2*MANT_BIT-e) * xylimbs
                     summand2 = 2^(ze-MANT_BIT-e) * zlimbs  */
                e = MIN (xye - 2 * MANT_BIT, ze - MANT_BIT);
                if (e == xye - 2 * MANT_BIT)
                  {
                    /* Simply copy the limbs of xylimbs.  */
                    size_t i;
                    for (i = 0; i < 2 * NLIMBS1; i++)
                      summand1[i] = xylimbs[i];
                    summand1_len = 2 * NLIMBS1;
                  }
                else
                  {
                    size_t ediff = xye - 2 * MANT_BIT - e;
                    /* Left shift the limbs of xylimbs by ediff bits.  */
                    size_t ldiff = ediff / GMP_LIMB_BITS;
                    size_t shift = ediff % GMP_LIMB_BITS;
                    size_t i;
                    for (i = 0; i < ldiff; i++)
                      summand1[i] = 0;
                    if (shift > 0)
                      {
                        mp_limb_t carry = 0;
                        for (i = 0; i < 2 * NLIMBS1; i++)
                          {
                            summand1[ldiff + i] = (xylimbs[i] << shift) | carry;
                            carry = xylimbs[i] >> (GMP_LIMB_BITS - shift);
                          }
                        summand1[ldiff + 2 * NLIMBS1] = carry;
                        summand1_len = ldiff + 2 * NLIMBS1 + 1;
                      }
                    else
                      {
                        for (i = 0; i < 2 * NLIMBS1; i++)
                          summand1[ldiff + i] = xylimbs[i];
                        summand1_len = ldiff + 2 * NLIMBS1;
                      }
                    /* Estimation of needed array size:
                       ediff = (xye - 2 * MANT_BIT) - (ze - MANT_BIT) <= MANT_BIT + 1
                       therefore
                       summand1_len
                         = (ediff + GMP_LIMB_BITS - 1) / GMP_LIMB_BITS + 2 * NLIMBS1
                         <= (MANT_BIT + GMP_LIMB_BITS) / GMP_LIMB_BITS + 2 * NLIMBS1
                         <= 3 * NLIMBS1 + 1
                         = NLIMBS3  */
                    if (!(summand1_len <= NLIMBS3))
                      abort ();
                  }
                if (e == ze - MANT_BIT)
                  {
                    /* Simply copy the limbs of zlimbs.  */
                    size_t i;
                    for (i = 0; i < NLIMBS1; i++)
                      summand2[i] = zlimbs[i];
                    summand2_len = NLIMBS1;
                  }
                else
                  {
                    size_t ediff = ze - MANT_BIT - e;
                    /* Left shift the limbs of zlimbs by ediff bits.  */
                    size_t ldiff = ediff / GMP_LIMB_BITS;
                    size_t shift = ediff % GMP_LIMB_BITS;
                    size_t i;
                    for (i = 0; i < ldiff; i++)
                      summand2[i] = 0;
                    if (shift > 0)
                      {
                        mp_limb_t carry = 0;
                        for (i = 0; i < NLIMBS1; i++)
                          {
                            summand2[ldiff + i] = (zlimbs[i] << shift) | carry;
                            carry = zlimbs[i] >> (GMP_LIMB_BITS - shift);
                          }
                        summand2[ldiff + NLIMBS1] = carry;
                        summand2_len = ldiff + NLIMBS1 + 1;
                      }
                    else
                      {
                        for (i = 0; i < NLIMBS1; i++)
                          summand2[ldiff + i] = zlimbs[i];
                        summand2_len = ldiff + NLIMBS1;
                      }
                    /* Estimation of needed array size:
                       ediff = (ze - MANT_BIT) - (xye - 2 * MANT_BIT) <= 2 * MANT_BIT
                       therefore
                       summand2_len
                         = (ediff + GMP_LIMB_BITS - 1) / GMP_LIMB_BITS + NLIMBS1
                         <= (2 * MANT_BIT + GMP_LIMB_BITS - 1) / GMP_LIMB_BITS + NLIMBS1
                         <= 3 * NLIMBS1 + 1
                         = NLIMBS3  */
                    if (!(summand2_len <= NLIMBS3))
                      abort ();
                  }
              }
              /* The result is
                   (-1)^xys * 2^e * summand1 + (-1)^zs * 2^e * summand2.  */
              if (xys == zs)
                {
                  /* Perform an addition.  */
                  size_t i;
                  mp_limb_t carry;

                  sign = xys;
                  carry = 0;
                  for (i = 0; i < MIN (summand1_len, summand2_len); i++)
                    {
                      mp_limb_t digit1 = summand1[i];
                      mp_limb_t digit2 = summand2[i];
                      sum[i] = digit1 + digit2 + carry;
                      carry = (carry
                               ? digit1 >= (mp_limb_t)-1 - digit2
                               : digit1 > (mp_limb_t)-1 - digit2);
                    }
                  if (summand1_len > summand2_len)
                    for (; i < summand1_len; i++)
                      {
                        mp_limb_t digit1 = summand1[i];
                        sum[i] = carry + digit1;
                        carry = carry && digit1 == (mp_limb_t)-1;
                      }
                  else
                    for (; i < summand2_len; i++)
                      {
                        mp_limb_t digit2 = summand2[i];
                        sum[i] = carry + digit2;
                        carry = carry && digit2 == (mp_limb_t)-1;
                      }
                  if (carry)
                    sum[i++] = carry;
                  sum_len = i;
                }
              else
                {
                  /* Perform a subtraction.  */
                  /* Compare summand1 and summand2 by magnitude.  */
                  while (summand1[summand1_len - 1] == 0)
                    summand1_len--;
                  while (summand2[summand2_len - 1] == 0)
                    summand2_len--;
                  if (summand1_len > summand2_len)
                    sign = xys;
                  else if (summand1_len < summand2_len)
                    sign = zs;
                  else
                    {
                      size_t i = summand1_len;
                      for (;;)
                        {
                          --i;
                          if (summand1[i] > summand2[i])
                            {
                              sign = xys;
                              break;
                            }
                          if (summand1[i] < summand2[i])
                            {
                              sign = zs;
                              break;
                            }
                          if (i == 0)
                            /* summand1 and summand2 are equal.  */
                            return L_(0.0);
                        }
                    }
                  if (sign == xys)
                    {
                      /* Compute summand1 - summand2.  */
                      size_t i;
                      mp_limb_t carry;

                      carry = 0;
                      for (i = 0; i < summand2_len; i++)
                        {
                          mp_limb_t digit1 = summand1[i];
                          mp_limb_t digit2 = summand2[i];
                          sum[i] = digit1 - digit2 - carry;
                          carry = (carry ? digit1 <= digit2 : digit1 < digit2);
                        }
                      for (; i < summand1_len; i++)
                        {
                          mp_limb_t digit1 = summand1[i];
                          sum[i] = digit1 - carry;
                          carry = carry && digit1 == 0;
                        }
                      if (carry)
                        abort ();
                      sum_len = summand1_len;
                    }
                  else
                    {
                      /* Compute summand2 - summand1.  */
                      size_t i;
                      mp_limb_t carry;

                      carry = 0;
                      for (i = 0; i < summand1_len; i++)
                        {
                          mp_limb_t digit1 = summand1[i];
                          mp_limb_t digit2 = summand2[i];
                          sum[i] = digit2 - digit1 - carry;
                          carry = (carry ? digit2 <= digit1 : digit2 < digit1);
                        }
                      for (; i < summand2_len; i++)
                        {
                          mp_limb_t digit2 = summand2[i];
                          sum[i] = digit2 - carry;
                          carry = carry && digit2 == 0;
                        }
                      if (carry)
                        abort ();
                      sum_len = summand2_len;
                    }
                }
            }
            /* The result is
                 (-1)^sign * 2^e * sum.  */
            /* Now perform the rounding to MANT_BIT mantissa bits.  */
            while (sum[sum_len - 1] == 0)
              sum_len--;
            /* Here we know that the most significant limb, sum[sum_len - 1], is
               non-zero.  */
            {
              /* How many bits the sum has.  */
              unsigned int sum_bits =
                integer_length (sum[sum_len - 1]) + (sum_len - 1) * GMP_LIMB_BITS;
              /* How many bits to keep when rounding.  */
              unsigned int keep_bits;
              /* How many bits to round off.  */
              unsigned int roundoff_bits;
              if (e + (int) sum_bits >= MIN_EXP)
                /* 2^e * sum >= 2^(MIN_EXP-1).
                   result will be a normalized number.  */
                keep_bits = MANT_BIT;
              else if (e + (int) sum_bits >= MIN_EXP - MANT_BIT)
                /* 2^e * sum >= 2^(MIN_EXP-MANT_BIT-1).
                   result will be a denormalized number or rounded to zero.  */
                keep_bits = e + (int) sum_bits - (MIN_EXP + MANT_BIT);
              else
                /* 2^e * sum < 2^(MIN_EXP-MANT_BIT-1).  Round to zero.  */
                return L_(0.0);
              /* Note: 0 <= keep_bits <= MANT_BIT.  */
              if (sum_bits <= keep_bits)
                {
                  /* Nothing to do.  */
                  roundoff_bits = 0;
                  keep_bits = sum_bits;
                }
              else
                {
                  int round_up;
                  roundoff_bits = sum_bits - keep_bits; /* > 0, <= sum_bits */
                  {
#if HAVE_FEGETROUND && defined FE_TOWARDZERO
                    /* Cf. <https://pubs.opengroup.org/onlinepubs/9699919799/functions/fegetround.html> */
                    int rounding_mode = fegetround ();
                    if (rounding_mode == FE_TOWARDZERO)
                      round_up = 0;
# if defined FE_DOWNWARD /* not defined on sh4 */
                    else if (rounding_mode == FE_DOWNWARD)
                      round_up = sign;
# endif
# if defined FE_UPWARD /* not defined on sh4 */
                    else if (rounding_mode == FE_UPWARD)
                      round_up = !sign;
# endif
#else
                    /* Cf. <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/float.h.html> */
                    int rounding_mode = FLT_ROUNDS;
                    if (rounding_mode == 0) /* toward zero */
                      round_up = 0;
                    else if (rounding_mode == 3) /* toward negative infinity */
                      round_up = sign;
                    else if (rounding_mode == 2) /* toward positive infinity */
                      round_up = !sign;
#endif
                    else
                      {
                        /* Round to nearest.  */
                        round_up = 0;
                        /* Test bit (roundoff_bits-1).  */
                        if ((sum[(roundoff_bits - 1) / GMP_LIMB_BITS]
                             >> ((roundoff_bits - 1) % GMP_LIMB_BITS)) & 1)
                          {
                            /* Test bits roundoff_bits-1 .. 0.  */
                            bool halfway =
                              ((sum[(roundoff_bits - 1) / GMP_LIMB_BITS]
                                & (((mp_limb_t) 1 << ((roundoff_bits - 1) % GMP_LIMB_BITS)) - 1))
                               == 0);
                            if (halfway)
                              {
                                int i;
                                for (i = (roundoff_bits - 1) / GMP_LIMB_BITS - 1; i >= 0; i--)
                                  if (sum[i] != 0)
                                    {
                                      halfway = false;
                                      break;
                                    }
                              }
                            if (halfway)
                              /* Round to even.  Test bit roundoff_bits.  */
                              round_up = ((sum[roundoff_bits / GMP_LIMB_BITS]
                                          >> (roundoff_bits % GMP_LIMB_BITS)) & 1);
                            else
                              /* Round up.  */
                              round_up = 1;
                          }
                      }
                  }
                  /* Perform the rounding.  */
                  {
                    size_t i = roundoff_bits / GMP_LIMB_BITS;
                    {
                      size_t j = i;
                      while (j > 0)
                        sum[--j] = 0;
                    }
                    if (round_up)
                      {
                        /* Round up.  */
                        sum[i] =
                          (sum[i]
                           | (((mp_limb_t) 1 << (roundoff_bits % GMP_LIMB_BITS)) - 1))
                          + 1;
                        if (sum[i] == 0)
                          {
                            /* Propagate carry.  */
                            while (i < sum_len - 1)
                              {
                                ++i;
                                sum[i] += 1;
                                if (sum[i] != 0)
                                  break;
                              }
                          }
                        /* sum[i] is the most significant limb that was
                           incremented.  */
                        if (i == sum_len - 1 && (sum[i] & (sum[i] - 1)) == 0)
                          {
                            /* Through the carry, one more bit is needed.  */
                            if (sum[i] != 0)
                              sum_bits += 1;
                            else
                              {
                                /* Instead of requiring one more limb of memory,
                                   perform a shift by one bit, and adjust the
                                   exponent.  */
                                sum[i] = (mp_limb_t) 1 << (GMP_LIMB_BITS - 1);
                                e += 1;
                              }
                            /* The bit sequence has the form 1000...000.  */
                            keep_bits = 1;
                          }
                      }
                    else
                      {
                        /* Round down.  */
                        sum[i] &= ((mp_limb_t) -1 << (roundoff_bits % GMP_LIMB_BITS));
                        if (i == sum_len - 1 && sum[i] == 0)
                          /* The entire sum has become zero.  */
                          return L_(0.0);
                      }
                  }
                }
              /* The result is
                   (-1)^sign * 2^e * sum
                 and here we know that
                   2^(sum_bits-1) <= sum < 2^sum_bits,
                 and sum is a multiple of 2^(sum_bits-keep_bits), where
                   0 < keep_bits <= MANT_BIT  and  keep_bits <= sum_bits.
                 (If keep_bits was initially 0, the rounding either returned zero
                 or produced a bit sequence of the form 1000...000, setting
                 keep_bits to 1.)  */
              {
                /* Split the keep_bits bits into chunks of at most 32 bits.  */
                unsigned int chunk_count = (keep_bits - 1) / GMP_LIMB_BITS + 1;
                /* 1 <= chunk_count <= ceil (sum_bits / GMP_LIMB_BITS) = sum_len. */
                static const DOUBLE chunk_multiplier = /* 2^-GMP_LIMB_BITS */
                  L_(1.0) / ((DOUBLE) (1 << (GMP_LIMB_BITS / 2))
                             * (DOUBLE) (1 << ((GMP_LIMB_BITS + 1) / 2)));
                unsigned int shift = sum_bits % GMP_LIMB_BITS;
                DOUBLE fsum;
                if (MANT_BIT <= GMP_LIMB_BITS)
                  {
                    /* Since keep_bits <= MANT_BIT <= GMP_LIMB_BITS,
                       chunk_count is 1.  No need for a loop.  */
                    if (shift == 0)
                      fsum = (DOUBLE) sum[sum_len - 1];
                    else
                      fsum = (DOUBLE)
                             ((sum[sum_len - 1] << (GMP_LIMB_BITS - shift))
                              | (sum_len >= 2 ? sum[sum_len - 2] >> shift : 0));
                  }
                else
                  {
                    int k;
                    k = chunk_count - 1;
                    if (shift == 0)
                      {
                        /* First loop round.  */
                        fsum = (DOUBLE) sum[sum_len - k - 1];
                        /* General loop.  */
                        while (--k >= 0)
                          {
                            fsum *= chunk_multiplier;
                            fsum += (DOUBLE) sum[sum_len - k - 1];
                          }
                      }
                    else
                      {
                        /* First loop round.  */
                        {
                          VOLATILE mp_limb_t chunk =
                            (sum[sum_len - k - 1] << (GMP_LIMB_BITS - shift))
                            | (sum_len >= k + 2 ? sum[sum_len - k - 2] >> shift : 0);
                          fsum = (DOUBLE) chunk;
                        }
                        /* General loop.  */
                        while (--k >= 0)
                          {
                            fsum *= chunk_multiplier;
                            {
                              VOLATILE mp_limb_t chunk =
                                (sum[sum_len - k - 1] << (GMP_LIMB_BITS - shift))
                                | (sum[sum_len - k - 2] >> shift);
                              fsum += (DOUBLE) chunk;
                            }
                          }
                      }
                  }
                fsum = LDEXP (fsum, e + (int) sum_bits - GMP_LIMB_BITS);
                return (sign ? - fsum : fsum);
              }
            }
          }
        }
      else
        return z;
    }
  else
    return (x * y) + z;
}
