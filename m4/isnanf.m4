# isnanf.m4 serial 6
dnl Copyright (C) 2007-2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Check how to get or define isnanf() without linking with libm.

AC_DEFUN([gl_FUNC_ISNANF_NO_LIBM],
[
  gl_HAVE_ISNANF_NO_LIBM
  if test $gl_cv_func_isnanf_no_libm = yes; then
    gl_ISNANF_WORKS
  fi
  if test $gl_cv_func_isnanf_no_libm = yes \
     && { case "$gl_cv_func_isnanf_works" in
            *yes) true;;
            *) false;;
          esac
        }; then
    AC_DEFINE([HAVE_ISNANF_IN_LIBC], 1,
      [Define if the isnan(float) function is available in libc.])
  else
    AC_LIBOBJ([isnanf])
    gl_FLOAT_EXPONENT_LOCATION
  fi
])

dnl Test whether isnanf() can be used without libm.
AC_DEFUN([gl_HAVE_ISNANF_NO_LIBM],
[
  AC_CACHE_CHECK([whether isnan(float) can be used without linking with libm],
    [gl_cv_func_isnanf_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   #if __GNUC__ >= 4
                   # undef isnanf
                   # define isnanf(x) __builtin_isnanf ((float)(x))
                   #elif defined isnan
                   # undef isnanf
                   # define isnanf(x) isnan ((float)(x))
                   #endif
                   float x;],
                  [return isnanf (x);],
        [gl_cv_func_isnanf_no_libm=yes],
        [gl_cv_func_isnanf_no_libm=no])
    ])
])

dnl Test whether isnanf() rejects Infinity (this fails on Solaris 2.5.1),
dnl recognizes a NaN (this fails on IRIX 6.5 with cc), and recognizes a NaN
dnl with in-memory representation 0x7fbfffff (this fails on IRIX 6.5).
AC_DEFUN([gl_ISNANF_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_FLOAT_EXPONENT_LOCATION])
  AC_CACHE_CHECK([whether isnan(float) works], [gl_cv_func_isnanf_works],
    [
      AC_TRY_RUN([
#include <math.h>
#if __GNUC__ >= 4
# undef isnanf
# define isnanf(x) __builtin_isnanf ((float)(x))
#elif defined isnan
# undef isnanf
# define isnanf(x) isnan ((float)(x))
#endif
/* The Compaq (ex-DEC) C 6.4 compiler chokes on the expression 0.0 / 0.0.  */
#ifdef __DECC
static float
NaN ()
{
  static float zero = 0.0f;
  return zero / zero;
}
#else
# define NaN() (0.0f / 0.0f)
#endif
#define NWORDS \
  ((sizeof (float) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { unsigned int word[NWORDS]; float value; } memory_float;
int main()
{
  memory_float m;

  if (isnanf (1.0f / 0.0f))
    return 1;

  if (!isnanf (NaN ()))
    return 1;

#if defined FLT_EXPBIT0_WORD && defined FLT_EXPBIT0_BIT
  /* The isnanf function should be immune against changes in the sign bit and
     in the mantissa bits.  The xor operation twiddles a bit that can only be
     a sign bit or a mantissa bit.  */
  if (FLT_EXPBIT0_WORD == 0 && FLT_EXPBIT0_BIT > 0)
    {
      m.value = NaN ();
      /* Set the bits below the exponent to 01111...111.  */
      m.word[0] &= -1U << FLT_EXPBIT0_BIT;
      m.word[0] |= 1U << (FLT_EXPBIT0_BIT - 1) - 1;
      if (!isnanf (m.value))
        return 1;
    }
#endif

  return 0;
}], [gl_cv_func_isnanf_works=yes], [gl_cv_func_isnanf_works=no],
        [case "$host_os" in
           irix* | solaris*) gl_cv_func_isnanf_works="guessing no";;
           *)                gl_cv_func_isnanf_works="guessing yes";;
         esac
        ])
    ])
])

AC_DEFUN([gl_FLOAT_EXPONENT_LOCATION],
[
  AC_CACHE_CHECK([where to find the exponent in a 'float'],
    [gl_cv_cc_float_expbit0],
    [
      AC_TRY_RUN([
#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define NWORDS \
  ((sizeof (float) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { float value; unsigned int word[NWORDS]; } memory_float;
static unsigned int ored_words[NWORDS];
static unsigned int anded_words[NWORDS];
static void add_to_ored_words (float x)
{
  memory_float m;
  size_t i;
  /* Clear it first, in case
     sizeof (float) < sizeof (memory_float).  */
  memset (&m, 0, sizeof (memory_float));
  m.value = x;
  for (i = 0; i < NWORDS; i++)
    {
      ored_words[i] |= m.word[i];
      anded_words[i] &= m.word[i];
    }
}
int main ()
{
  size_t j;
  FILE *fp = fopen ("conftest.out", "w");
  if (fp == NULL)
    return 1;
  for (j = 0; j < NWORDS; j++)
    anded_words[j] = ~ (unsigned int) 0;
  add_to_ored_words (0.25f);
  add_to_ored_words (0.5f);
  add_to_ored_words (1.0f);
  add_to_ored_words (2.0f);
  add_to_ored_words (4.0f);
  /* Remove bits that are common (e.g. if representation of the first mantissa
     bit is explicit).  */
  for (j = 0; j < NWORDS; j++)
    ored_words[j] &= ~anded_words[j];
  /* Now find the nonzero word.  */
  for (j = 0; j < NWORDS; j++)
    if (ored_words[j] != 0)
      break;
  if (j < NWORDS)
    {
      size_t i;
      for (i = j + 1; i < NWORDS; i++)
        if (ored_words[i] != 0)
          {
            fprintf (fp, "unknown");
            return (fclose (fp) != 0);
          }
      for (i = 0; ; i++)
        if ((ored_words[j] >> i) & 1)
          {
            fprintf (fp, "word %d bit %d", (int) j, (int) i);
            return (fclose (fp) != 0);
          }
    }
  fprintf (fp, "unknown");
  return (fclose (fp) != 0);
}
        ],
        [gl_cv_cc_float_expbit0=`cat conftest.out`],
        [gl_cv_cc_float_expbit0="unknown"],
        [gl_cv_cc_float_expbit0="word 0 bit 23"])
      rm -f conftest.out
    ])
  case "$gl_cv_cc_float_expbit0" in
    word*bit*)
      word=`echo "$gl_cv_cc_float_expbit0" | sed -e 's/word //' -e 's/ bit.*//'`
      bit=`echo "$gl_cv_cc_float_expbit0" | sed -e 's/word.*bit //'`
      AC_DEFINE_UNQUOTED([FLT_EXPBIT0_WORD], [$word],
        [Define as the word index where to find the exponent of 'float'.])
      AC_DEFINE_UNQUOTED([FLT_EXPBIT0_BIT], [$bit],
        [Define as the bit index in the word where to find bit 0 of the exponent of 'float'.])
      ;;
  esac
])
