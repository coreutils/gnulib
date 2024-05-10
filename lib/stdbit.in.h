/* stdbit.h - C23 bit and byte utilities for non-C23 platforms

   Copyright 2024 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

#ifndef STDBIT_H
#define STDBIT_H 1

/* This file uses _GL_INLINE, WORDS_BIGENDIAN.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <count-leading-zeros.h>
#include <count-trailing-zeros.h>
#include <count-one-bits.h>

/* An expression, preferably with the type of A, that has the value of B.  */
#if ((defined __GNUC__ && 2 <= __GNUC__) \
     || (defined __clang_major__ && 4 <= __clang_major__) \
     || (defined __IBMC__ && 1210 <= __IBMC__ && defined __IBM__TYPEOF__) \
     || (defined __SUNPRO_C && 0x5110 <= __SUNPRO_C && !__STDC__))
# define _GL_STDBIT_TYPEOF_CAST(a, b) ((__typeof__ (a)) (b))
#elif 202311 <= __STDC_VERSION__
# define _GL_STDBIT_TYPEOF_CAST(a, b) ((typeof (a)) (b))
#else
/* This platform is so old that it lacks typeof, so _Generic is likely
   missing or unreliable.  The C23 standard seems to allow yielding B
   (which is always unsigned long long int), so do that.  */
# define _GL_STDBIT_TYPEOF_CAST(a, b) (b)
#endif

#define __STDC_VERSION_STDBIT_H__ 202311L

#define __STDC_ENDIAN_BIG__ 4321
#define __STDC_ENDIAN_LITTLE__ 1234
#ifdef WORDS_BIGENDIAN
# define __STDC_ENDIAN_NATIVE__ __STDC_ENDIAN_BIG__
#else
# define __STDC_ENDIAN_NATIVE__ __STDC_ENDIAN_LITTLE__
#endif

_GL_INLINE_HEADER_BEGIN
#ifndef _GL_STDBIT_INLINE
# define _GL_STDBIT_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif


_GL_STDBIT_INLINE unsigned int
stdc_leading_zeros_ui (unsigned int n)
{
  return count_leading_zeros (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_zeros_uc (unsigned char n)
{
  return stdc_leading_zeros_ui (n) - 8 * (sizeof 0u - sizeof n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_zeros_us (unsigned short int n)
{
  return stdc_leading_zeros_ui (n) - 8 * (sizeof 0u - sizeof n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_zeros_ul (unsigned long int n)
{
  return count_leading_zeros_l (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_zeros_ull (unsigned long long int n)
{
  return count_leading_zeros_ll (n);
}

#define stdc_leading_zeros(n) \
  (sizeof (n) == 1 ? stdc_leading_zeros_uc (n)	\
   : sizeof (n) == sizeof (unsigned short int) ? stdc_leading_zeros_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_leading_zeros_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_leading_zeros_ul (n) \
   : stdc_leading_zeros_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_leading_ones_uc (unsigned char n)
{
  return stdc_leading_zeros_uc (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_ones_us (unsigned short int n)
{
  return stdc_leading_zeros_us (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_ones_ui (unsigned int n)
{
  return stdc_leading_zeros_ui (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_ones_ul (unsigned long int n)
{
  return stdc_leading_zeros_ul (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_leading_ones_ull (unsigned long long int n)
{
  return stdc_leading_zeros_ull (~n);
}

#define stdc_leading_ones(n) \
  (sizeof (n) == 1 ? stdc_leading_ones_uc (n)	\
   : sizeof (n) == sizeof (unsigned short int) ? stdc_leading_ones_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_leading_ones_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_leading_ones_ul (n) \
   : stdc_leading_ones_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_trailing_zeros_ui (unsigned int n)
{
  return count_trailing_zeros (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_zeros_uc (unsigned char n)
{
  return stdc_trailing_zeros_ui (n | (1 + (unsigned char) -1));
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_zeros_us (unsigned short int n)
{
  return stdc_trailing_zeros_ui (n | (1 + (unsigned short int) -1));
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_zeros_ul (unsigned long int n)
{
  return count_trailing_zeros_l (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_zeros_ull (unsigned long long int n)
{
  return count_trailing_zeros_ll (n);
}

#define stdc_trailing_zeros(n) \
  (sizeof (n) == 1 ? stdc_trailing_zeros_uc (n)	\
   : sizeof (n) == sizeof (unsigned short int) ? stdc_trailing_zeros_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_trailing_zeros_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_trailing_zeros_ul (n) \
   : stdc_trailing_zeros_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_trailing_ones_uc (unsigned char n)
{
  return stdc_trailing_zeros_uc (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_ones_us (unsigned short int n)
{
  return stdc_trailing_zeros_us (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_ones_ui (unsigned int n)
{
  return stdc_trailing_zeros_ui (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_ones_ul (unsigned long int n)
{
  return stdc_trailing_zeros_ul (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_trailing_ones_ull (unsigned long long int n)
{
  return stdc_trailing_zeros_ull (~n);
}

#define stdc_trailing_ones(n) \
  (sizeof (n) == 1 ? stdc_trailing_ones_uc (n)	\
   : sizeof (n) == sizeof (unsigned short int) ? stdc_trailing_ones_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_trailing_ones_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_trailing_ones_ul (n) \
   : stdc_trailing_ones_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_first_leading_zero_uc (unsigned char n)
{
  unsigned int count = stdc_leading_ones_uc (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_zero_us (unsigned short int n)
{
  unsigned int count = stdc_leading_ones_us (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_zero_ui (unsigned int n)
{
  unsigned int count = stdc_leading_ones_ui (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_zero_ul (unsigned long int n)
{
  unsigned int count = stdc_leading_ones_ul (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_zero_ull (unsigned long long int n)
{
  unsigned int count = stdc_leading_ones_ull (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

#define stdc_first_leading_zero(n) \
  (sizeof (n) == 1 ? stdc_first_leading_zero_uc (n) \
   : sizeof (n) == sizeof (unsigned short) ? stdc_first_leading_zero_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_first_leading_zero_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_first_leading_zero_ul (n) \
   : stdc_first_leading_zero_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_first_leading_one_uc (unsigned char n)
{
  unsigned int count = stdc_leading_zeros_uc (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_one_us (unsigned short int n)
{
  unsigned int count = stdc_leading_zeros_us (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_one_ui (unsigned int n)
{
  unsigned int count = stdc_leading_zeros_ui (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_one_ul (unsigned long int n)
{
  unsigned int count = stdc_leading_zeros_ul (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_leading_one_ull (unsigned long long int n)
{
  unsigned int count = stdc_leading_zeros_ull (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

#define stdc_first_leading_one(n) \
  (sizeof (n) == 1 ? stdc_first_leading_one_uc (n) \
   : sizeof (n) == sizeof (unsigned short) ? stdc_first_leading_one_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_first_leading_one_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_first_leading_one_ul (n) \
   : stdc_first_leading_one_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_zero_uc (unsigned char n)
{
  unsigned int count = stdc_trailing_ones_uc (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_zero_us (unsigned short int n)
{
  unsigned int count = stdc_trailing_ones_us (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_zero_ui (unsigned int n)
{
  unsigned int count = stdc_trailing_ones_ui (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_zero_ul (unsigned long int n)
{
  unsigned int count = stdc_trailing_ones_ul (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_zero_ull (unsigned long long int n)
{
  unsigned int count = stdc_trailing_ones_ull (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

#define stdc_first_trailing_zero(n) \
  (sizeof (n) == 1 ? stdc_first_trailing_zero_uc (n) \
   : sizeof (n) == sizeof (unsigned short) ? stdc_first_trailing_zero_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_first_trailing_zero_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_first_trailing_zero_ul (n) \
   : stdc_first_trailing_zero_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_one_uc (unsigned char n)
{
  unsigned int count = stdc_trailing_zeros_uc (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_one_us (unsigned short int n)
{
  unsigned int count = stdc_trailing_zeros_us (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_one_ui (unsigned int n)
{
  unsigned int count = stdc_trailing_zeros_ui (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_one_ul (unsigned long int n)
{
  unsigned int count = stdc_trailing_zeros_ul (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

_GL_STDBIT_INLINE unsigned int
stdc_first_trailing_one_ull (unsigned long long int n)
{
  unsigned int count = stdc_trailing_zeros_ull (n);
  return count == 8 * sizeof n ? 0 : count + 1;
}

#define stdc_first_trailing_one(n) \
  (sizeof (n) == 1 ? stdc_first_trailing_one_uc (n) \
   : sizeof (n) == sizeof (unsigned short) ? stdc_first_trailing_one_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_first_trailing_one_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_first_trailing_one_ul (n) \
   : stdc_first_trailing_one_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_count_ones_ui (unsigned int n)
{
  return count_one_bits (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_ones_uc (unsigned char n)
{
  return stdc_count_ones_ui (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_ones_us (unsigned short int n)
{
  return stdc_count_ones_ui (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_ones_ul (unsigned long int n)
{
  return count_one_bits_l (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_ones_ull (unsigned long long int n)
{
  return count_one_bits_ll (n);
}

#define stdc_count_ones(n) \
  (sizeof (n) == 1 ? stdc_count_ones_uc (n) \
   : sizeof (n) == sizeof (unsigned short int) ? stdc_count_ones_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_count_ones_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_count_ones_ul (n) \
   : stdc_count_ones_ull (n))


_GL_STDBIT_INLINE unsigned int
stdc_count_zeros_uc (unsigned char n)
{
  return stdc_count_ones_uc (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_zeros_us (unsigned short int n)
{
  return stdc_count_ones_us (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_zeros_ui (unsigned int n)
{
  return stdc_count_ones_ui (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_zeros_ul (unsigned long int n)
{
  return stdc_count_ones_ul (~n);
}

_GL_STDBIT_INLINE unsigned int
stdc_count_zeros_ull (unsigned long long int n)
{
  return stdc_count_ones_ull (~n);
}

#define stdc_count_zeros(n) \
  (sizeof (n) == 1 ? stdc_count_zeros_uc (n) \
   : sizeof (n) == sizeof (unsigned short int) ? stdc_count_zeros_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_count_zeros_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_count_zeros_ul (n) \
   : stdc_count_zeros_ull (n))


_GL_STDBIT_INLINE bool
stdc_has_single_bit_uc (unsigned char n)
{
  unsigned char n_1 = n - 1, nx = n_1 ^ n;
  return n_1 < nx;
}

_GL_STDBIT_INLINE bool
stdc_has_single_bit_us (unsigned short int n)
{
  unsigned short int n_1 = n - 1, nx = n_1 ^ n;
  return n_1 < nx;
}

_GL_STDBIT_INLINE bool
stdc_has_single_bit_ui (unsigned int n)
{
  unsigned int n_1 = n - 1, nx = n_1 ^ n;
  return n_1 < nx;
}

_GL_STDBIT_INLINE bool
stdc_has_single_bit_ul (unsigned long int n)
{
  unsigned long int n_1 = n - 1, nx = n_1 ^ n;
  return n_1 < nx;
}

_GL_STDBIT_INLINE bool
stdc_has_single_bit_ull (unsigned long long int n)
{
  unsigned long long int n_1 = n - 1, nx = n_1 ^ n;
  return n_1 < nx;
}

#define stdc_has_single_bit(n) \
  ((bool) \
   (sizeof (n) == 1 ? stdc_has_single_bit_uc (n) \
    : sizeof (n) == sizeof (unsigned short int) ? stdc_has_single_bit_us (n) \
    : sizeof (n) == sizeof 0u ? stdc_has_single_bit_ui (n) \
    : sizeof (n) == sizeof 0ul ? stdc_has_single_bit_ul (n) \
    : stdc_has_single_bit_ull (n)))


_GL_STDBIT_INLINE unsigned int
stdc_bit_width_uc (unsigned char n)
{
  return 8 * sizeof n - stdc_leading_zeros_uc (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_width_us (unsigned short int n)
{
  return 8 * sizeof n - stdc_leading_zeros_us (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_width_ui (unsigned int n)
{
  return 8 * sizeof n - stdc_leading_zeros_ui (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_width_ul (unsigned long int n)
{
  return 8 * sizeof n - stdc_leading_zeros_ul (n);
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_width_ull (unsigned long long int n)
{
  return 8 * sizeof n - stdc_leading_zeros_ull (n);
}

#define stdc_bit_width(n) \
  (sizeof (n) == 1 ? stdc_bit_width_uc (n) \
   : sizeof (n) == sizeof (unsigned short int) ? stdc_bit_width_us (n) \
   : sizeof (n) == sizeof 0u ? stdc_bit_width_ui (n) \
   : sizeof (n) == sizeof 0ul ? stdc_bit_width_ul (n) \
   : stdc_bit_width_ull (n))


_GL_STDBIT_INLINE unsigned char
stdc_bit_floor_uc (unsigned char n)
{
  return n ? 1u << (stdc_bit_width_uc (n) - 1) : 0;
}

_GL_STDBIT_INLINE unsigned short int
stdc_bit_floor_us (unsigned short int n)
{
  return n ? 1u << (stdc_bit_width_us (n) - 1) : 0;
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_floor_ui (unsigned int n)
{
  return n ? 1u << (stdc_bit_width_ui (n) - 1) : 0;
}

_GL_STDBIT_INLINE unsigned long int
stdc_bit_floor_ul (unsigned long int n)
{
  return n ? 1ul << (stdc_bit_width_ul (n) - 1) : 0;
}

_GL_STDBIT_INLINE unsigned long long int
stdc_bit_floor_ull (unsigned long long int n)
{
  return n ? 1ull << (stdc_bit_width_ull (n) - 1) : 0;
}

#define stdc_bit_floor(n) \
  (_GL_STDBIT_TYPEOF_CAST \
   (n, \
    (sizeof (n) == 1 ? stdc_bit_floor_uc (n) \
     : sizeof (n) == sizeof (unsigned short int) ? stdc_bit_floor_us (n) \
     : sizeof (n) == sizeof 0u ? stdc_bit_floor_ui (n) \
     : sizeof (n) == sizeof 0ul ? stdc_bit_floor_ul (n) \
     : stdc_bit_floor_ull (n))))


_GL_STDBIT_INLINE unsigned char
stdc_bit_ceil_uc (unsigned char n)
{
  return n <= 1 ? 1 : 2u << (stdc_bit_width_uc (n - 1) - 1);
}

_GL_STDBIT_INLINE unsigned short int
stdc_bit_ceil_us (unsigned short int n)
{
  return n <= 1 ? 1 : 2u << (stdc_bit_width_us (n - 1) - 1);
}

_GL_STDBIT_INLINE unsigned int
stdc_bit_ceil_ui (unsigned int n)
{
  return n <= 1 ? 1 : 2u << (stdc_bit_width_ui (n - 1) - 1);
}

_GL_STDBIT_INLINE unsigned long int
stdc_bit_ceil_ul (unsigned long int n)
{
  return n <= 1 ? 1 : 2ul << (stdc_bit_width_ul (n - 1) - 1);
}

_GL_STDBIT_INLINE unsigned long long int
stdc_bit_ceil_ull (unsigned long long int n)
{
  return n <= 1 ? 1 : 2ull << (stdc_bit_width_ull (n - 1) - 1);
}

#define stdc_bit_ceil(n) \
  (_GL_STDBIT_TYPEOF_CAST \
   (n, \
    (sizeof (n) == 1 ? stdc_bit_ceil_uc (n) \
     : sizeof (n) == sizeof (unsigned short int) ? stdc_bit_ceil_us (n) \
     : sizeof (n) == sizeof 0u ? stdc_bit_ceil_ui (n) \
     : sizeof (n) == sizeof 0ul ? stdc_bit_ceil_ul (n) \
     : stdc_bit_ceil_ull (n))))


#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

#endif /* STDBIT_H */
