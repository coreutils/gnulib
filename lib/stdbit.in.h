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

/* Taken from count-leading-zeros.h.  */
#include <limits.h>
#include <stdlib.h>

_GL_INLINE_HEADER_BEGIN
#ifndef COUNT_LEADING_ZEROS_INLINE
# define COUNT_LEADING_ZEROS_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Assuming the GCC builtin is BUILTIN and the MSC builtin is MSC_BUILTIN,
   expand to code that computes the number of leading zeros of the local
   variable 'x' of type TYPE (an unsigned integer type) and return it
   from the current function.  */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) \
    || (__clang_major__ >= 4)
# define COUNT_LEADING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)                \
  return x ? BUILTIN (x) : CHAR_BIT * sizeof x;
#elif _MSC_VER
# pragma intrinsic (_BitScanReverse)
# if defined _M_X64
#  pragma intrinsic (_BitScanReverse64)
# endif
# define COUNT_LEADING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)                \
    do                                                                  \
      {                                                                 \
        unsigned long result;                                           \
        if (MSC_BUILTIN (&result, x))                                   \
          return CHAR_BIT * sizeof x - 1 - result;                      \
        return CHAR_BIT * sizeof x;                                     \
      }                                                                 \
    while (0)
#else
# define COUNT_LEADING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)                \
    do                                                                  \
      {                                                                 \
        int count;                                                      \
        unsigned int leading_32;                                        \
        if (! x)                                                        \
          return CHAR_BIT * sizeof x;                                   \
        for (count = 0;                                                 \
             (leading_32 = ((x >> (sizeof (TYPE) * CHAR_BIT - 32))      \
                            & 0xffffffffU),                             \
              count < CHAR_BIT * sizeof x - 32 && !leading_32);         \
             count += 32)                                               \
          x = x << 31 << 1;                                             \
        return count + count_leading_zeros_32 (leading_32);             \
      }                                                                 \
    while (0)

/* Compute and return the number of leading zeros in X,
   where 0 < X < 2**32.  */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_32 (unsigned int x)
{
  /* <https://github.com/gibsjose/BitHacks>
     <https://www.fit.vutbr.cz/~ibarina/pub/bithacks.pdf> */
  static const char de_Bruijn_lookup[32] = {
    31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,
    23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0
  };

  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return de_Bruijn_lookup[((x * 0x07c4acddU) & 0xffffffffU) >> 27];
}
#endif

/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros (unsigned int x)
{
  COUNT_LEADING_ZEROS (__builtin_clz, _BitScanReverse, unsigned int);
}

/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_l (unsigned long int x)
{
  COUNT_LEADING_ZEROS (__builtin_clzl, _BitScanReverse, unsigned long int);
}

/* Compute and return the number of leading zeros in X. */
COUNT_LEADING_ZEROS_INLINE int
count_leading_zeros_ll (unsigned long long int x)
{
#if (defined _MSC_VER && !defined __clang__) && !defined _M_X64
  /* 32-bit MSVC does not have _BitScanReverse64, only _BitScanReverse.  */
  unsigned long result;
  if (_BitScanReverse (&result, (unsigned long) (x >> 32)))
    return CHAR_BIT * sizeof x - 1 - 32 - result;
  if (_BitScanReverse (&result, (unsigned long) x))
    return CHAR_BIT * sizeof x - 1 - result;
  return CHAR_BIT * sizeof x;
#else
  COUNT_LEADING_ZEROS (__builtin_clzll, _BitScanReverse64,
                       unsigned long long int);
#endif
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

/* Taken from count-trailing-zeros.h.  */
#include <limits.h>
#include <stdlib.h>

_GL_INLINE_HEADER_BEGIN
#ifndef COUNT_TRAILING_ZEROS_INLINE
# define COUNT_TRAILING_ZEROS_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Assuming the GCC builtin is BUILTIN and the MSC builtin is MSC_BUILTIN,
   expand to code that computes the number of trailing zeros of the local
   variable 'x' of type TYPE (an unsigned integer type) and return it
   from the current function.  */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) \
    || (__clang_major__ >= 4)
# define COUNT_TRAILING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)               \
  return x ? BUILTIN (x) : CHAR_BIT * sizeof x;
#elif _MSC_VER
# pragma intrinsic (_BitScanForward)
# if defined _M_X64
#  pragma intrinsic (_BitScanForward64)
# endif
# define COUNT_TRAILING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)               \
    do                                                                  \
      {                                                                 \
        unsigned long result;                                           \
        return MSC_BUILTIN (&result, x) ? result : CHAR_BIT * sizeof x; \
      }                                                                 \
    while (0)
#else
# define COUNT_TRAILING_ZEROS(BUILTIN, MSC_BUILTIN, TYPE)               \
    do                                                                  \
      {                                                                 \
        int count = 0;                                                  \
        if (! x)                                                        \
          return CHAR_BIT * sizeof x;                                   \
        for (count = 0;                                                 \
             (count < CHAR_BIT * sizeof x - 32                          \
              && ! (x & 0xffffffffU));                                  \
             count += 32)                                               \
          x = x >> 31 >> 1;                                             \
        return count + count_trailing_zeros_32 (x);                     \
      }                                                                 \
    while (0)

/* Compute and return the number of trailing zeros in the least
   significant 32 bits of X.  One of these bits must be nonzero.  */
COUNT_TRAILING_ZEROS_INLINE int
count_trailing_zeros_32 (unsigned int x)
{
  /* <https://github.com/gibsjose/BitHacks>
     <https://www.fit.vutbr.cz/~ibarina/pub/bithacks.pdf> */
  static const char de_Bruijn_lookup[32] = {
    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
  };
  return de_Bruijn_lookup[(((x & -x) * 0x077cb531U) & 0xffffffffU) >> 27];
}
#endif

/* Compute and return the number of trailing zeros in X. */
COUNT_TRAILING_ZEROS_INLINE int
count_trailing_zeros (unsigned int x)
{
  COUNT_TRAILING_ZEROS (__builtin_ctz, _BitScanForward, unsigned int);
}

/* Compute and return the number of trailing zeros in X. */
COUNT_TRAILING_ZEROS_INLINE int
count_trailing_zeros_l (unsigned long int x)
{
  COUNT_TRAILING_ZEROS (__builtin_ctzl, _BitScanForward, unsigned long int);
}

/* Compute and return the number of trailing zeros in X. */
COUNT_TRAILING_ZEROS_INLINE int
count_trailing_zeros_ll (unsigned long long int x)
{
#if (defined _MSC_VER && !defined __clang__) && !defined _M_X64
  /* 32-bit MSVC does not have _BitScanForward64, only _BitScanForward.  */
  unsigned long result;
  if (_BitScanForward (&result, (unsigned long) x))
    return result;
  if (_BitScanForward (&result, (unsigned long) (x >> 32)))
    return result + 32;
  return CHAR_BIT * sizeof x;
#else
  COUNT_TRAILING_ZEROS (__builtin_ctzll, _BitScanForward64,
                        unsigned long long int);
#endif
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END

/* Taken from count-one-bits.h.  */
#include <limits.h>
#include <stdlib.h>

_GL_INLINE_HEADER_BEGIN
#ifndef COUNT_ONE_BITS_INLINE
# define COUNT_ONE_BITS_INLINE _GL_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Assuming the GCC builtin is GCC_BUILTIN and the MSC builtin is MSC_BUILTIN,
   expand to code that computes the number of 1-bits of the local
   variable 'x' of type TYPE (an unsigned integer type) and return it
   from the current function.  */
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) \
    || (__clang_major__ >= 4)
# define COUNT_ONE_BITS(GCC_BUILTIN, MSC_BUILTIN, TYPE) \
    return GCC_BUILTIN (x)
#else

/* Compute and return the number of 1-bits set in the least
   significant 32 bits of X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_32 (unsigned int x)
{
  x = ((x & 0xaaaaaaaaU) >> 1) + (x & 0x55555555U);
  x = ((x & 0xccccccccU) >> 2) + (x & 0x33333333U);
  x = (x >> 16) + (x & 0xffff);
  x = ((x & 0xf0f0) >> 4) + (x & 0x0f0f);
  return (x >> 8) + (x & 0x00ff);
}

/* Expand to code that computes the number of 1-bits of the local
   variable 'x' of type TYPE (an unsigned integer type) and return it
   from the current function.  */
# define COUNT_ONE_BITS_GENERIC(TYPE)                                   \
    do                                                                  \
      {                                                                 \
        int count = 0;                                                  \
        int bits;                                                       \
        for (bits = 0; bits < sizeof (TYPE) * CHAR_BIT; bits += 32)     \
          {                                                             \
            count += count_one_bits_32 (x);                             \
            x = x >> 31 >> 1;                                           \
          }                                                             \
        return count;                                                   \
      }                                                                 \
    while (0)

# if 1500 <= _MSC_VER && (defined _M_IX86 || defined _M_X64)

/* While gcc falls back to its own generic code if the machine
   on which it's running doesn't support popcount, with Microsoft's
   compiler we need to detect and fallback ourselves.  */

#  if 0
#   include <intrin.h>
#  else
    /* Don't pollute the namespace with too many MSVC intrinsics.  */
#   pragma intrinsic (__cpuid)
#   pragma intrinsic (__popcnt)
#   if defined _M_X64
#    pragma intrinsic (__popcnt64)
#   endif
#  endif

#  if !defined _M_X64
static inline __popcnt64 (unsigned long long x)
{
  return __popcnt ((unsigned int) (x >> 32)) + __popcnt ((unsigned int) x);
}
#  endif

/* Return nonzero if popcount is supported.  */

/* 1 if supported, 0 if not supported, -1 if unknown.  */
extern int popcount_support;

COUNT_ONE_BITS_INLINE int
popcount_supported (void)
{
  if (popcount_support < 0)
    {
      /* Do as described in
         <https://docs.microsoft.com/en-us/cpp/intrinsics/popcnt16-popcnt-popcnt64> */
      int cpu_info[4];
      __cpuid (cpu_info, 1);
      popcount_support = (cpu_info[2] >> 23) & 1;
    }
  return popcount_support;
}

#  define COUNT_ONE_BITS(GCC_BUILTIN, MSC_BUILTIN, TYPE) \
     do                                                  \
       {                                                 \
         if (popcount_supported ())                      \
           return MSC_BUILTIN (x);                       \
         else                                            \
           COUNT_ONE_BITS_GENERIC (TYPE);                \
       }                                                 \
     while (0)

# else

#  define COUNT_ONE_BITS(GCC_BUILTIN, MSC_BUILTIN, TYPE) \
     COUNT_ONE_BITS_GENERIC (TYPE)

# endif
#endif

/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits (unsigned int x)
{
  COUNT_ONE_BITS (__builtin_popcount, __popcnt, unsigned int);
}

/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_l (unsigned long int x)
{
  COUNT_ONE_BITS (__builtin_popcountl, __popcnt, unsigned long int);
}

/* Compute and return the number of 1-bits set in X. */
COUNT_ONE_BITS_INLINE int
count_one_bits_ll (unsigned long long int x)
{
  COUNT_ONE_BITS (__builtin_popcountll, __popcnt64, unsigned long long int);
}

#ifdef __cplusplus
}
#endif

_GL_INLINE_HEADER_END


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
