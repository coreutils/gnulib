# isnanl.m4 serial 2
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ISNANL],
[
  AC_REQUIRE([gt_TYPE_LONGDOUBLE])
  ISNANL_LIBM=
  if test $gt_cv_c_long_double = yes; then
    gl_HAVE_ISNANL_NO_LIBM
    if test $gl_cv_func_isnanl_no_libm = no; then
      gl_HAVE_ISNANL_IN_LIBM
      if test $gl_cv_func_isnanl_in_libm = yes; then
        ISNANL_LIBM=-lm
      fi
    fi
    if test $gl_cv_func_isnanl_no_libm = yes \
       || test $gl_cv_func_isnanl_in_libm = yes; then
      save_LIBS="$LIBS"
      LIBS="$LIBS $ISNANL_LIBM"
      gl_FUNC_ISNANL_WORKS
      LIBS="$save_LIBS"
      case "$gl_cv_func_isnanl_works" in
        *yes) gl_func_isnanl=yes ;;
        *)    gl_func_isnanl=no; ISNANL_LIBM= ;;
      esac
    else
      gl_func_isnanl=no
    fi
    if test $gl_func_isnanl = yes; then
      AC_DEFINE([HAVE_ISNANL], 1,
        [Define if the isnan(long double) function is available.])
    else
      AC_LIBOBJ([isnanl])
      gl_LONG_DOUBLE_EXPONENT_LOCATION
    fi
  fi
])

AC_DEFUN([gl_FUNC_ISNANL_NO_LIBM],
[
  AC_REQUIRE([gt_TYPE_LONGDOUBLE])
  if test $gt_cv_c_long_double = yes; then
    gl_HAVE_ISNANL_NO_LIBM
    gl_func_isnanl_no_libm=$gl_cv_func_isnanl_no_libm
    if test $gl_func_isnanl_no_libm = yes; then
      gl_FUNC_ISNANL_WORKS
      case "$gl_cv_func_isnanl_works" in
        *yes) ;;
        *)    gl_func_isnanl_no_libm=no ;;
      esac
    fi
    if test $gl_func_isnanl_no_libm = yes; then
      AC_DEFINE([HAVE_ISNANL_IN_LIBC], 1,
        [Define if the isnan(long double) function is available in libc.])
    else
      AC_LIBOBJ([isnanl])
      gl_LONG_DOUBLE_EXPONENT_LOCATION
    fi
  fi
])

dnl Test whether isnanl() can be used without libm.
AC_DEFUN([gl_HAVE_ISNANL_NO_LIBM],
[
  AC_CACHE_CHECK([whether isnan(long double) can be used without linking with libm],
    [gl_cv_func_isnanl_no_libm],
    [
      AC_TRY_LINK([#include <math.h>
                   #ifdef isnan
                   # undef isnanl
                   # define isnanl(x) isnan ((long double)(x))
                   #endif
                   long double x;],
                  [return isnanl (x);],
        [gl_cv_func_isnanl_no_libm=yes],
        [gl_cv_func_isnanl_no_libm=no])
    ])
])

dnl Test whether isnanl() can be used with libm.
AC_DEFUN([gl_HAVE_ISNANL_IN_LIBM],
[
  AC_CACHE_CHECK([whether isnan(long double) can be used with libm],
    [gl_cv_func_isnanl_in_libm],
    [
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([#include <math.h>
                   #ifdef isnan
                   # undef isnanl
                   # define isnanl(x) isnan ((long double)(x))
                   #endif
                   long double x;],
                  [return isnanl (x);],
        [gl_cv_func_isnanl_in_libm=yes],
        [gl_cv_func_isnanl_in_libm=no])
      LIBS="$save_LIBS"
    ])
])

dnl Test whether isnanl() recognizes all numbers which are neither finite nor
dnl infinite. This test fails e.g. on NetBSD/i386.
AC_DEFUN([gl_FUNC_ISNANL_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gt_TYPE_LONGDOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether isnanl works], [gl_cv_func_isnanl_works],
    [
      AC_TRY_RUN([
#include <limits.h>
#include <math.h>
#ifdef isnan
# undef isnanl
# define isnanl(x) isnan ((long double)(x))
#endif
#define NWORDS \
  ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { long double value; unsigned int word[NWORDS]; }
        memory_long_double;
int main ()
{
  memory_long_double m;
  unsigned int i;

  /* The isnanl function should be immune against changes in the sign bit and
     in the mantissa bits.  The xor operation twiddles a bit that can only be
     a sign bit or a mantissa bit (since the exponent never extends to
     bit 31).  */
  m.value = 0.0L / 0.0L;
  m.word[NWORDS / 2] ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
  for (i = 0; i < NWORDS; i++)
    m.word[i] |= 1;
  if (!isnanl (m.value))
    return 1;

  return 0;
}], [gl_cv_func_isnanl_works=yes], [gl_cv_func_isnanl_works=no],
      [case "$host_os" in
         netbsd*) gl_cv_func_isnanl_works="guessing no";;
         *)       gl_cv_func_isnanl_works="guessing yes";;
       esac
      ])
    ])
])

AC_DEFUN([gl_LONG_DOUBLE_EXPONENT_LOCATION],
[
  AC_CACHE_CHECK([where to find the exponent in a 'long double'],
    [gl_cv_cc_long_double_expbit0],
    [
      AC_TRY_RUN([
#include <float.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define NWORDS \
  ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
typedef union { long double value; unsigned int word[NWORDS]; }
        memory_long_double;
static unsigned int ored_words[NWORDS];
static unsigned int anded_words[NWORDS];
static void add_to_ored_words (long double x)
{
  memory_long_double m;
  size_t i;
  /* Clear it first, in case
     sizeof (long double) < sizeof (memory_long_double).  */
  memset (&m, 0, sizeof (memory_long_double));
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
  add_to_ored_words (0.25L);
  add_to_ored_words (0.5L);
  add_to_ored_words (1.0L);
  add_to_ored_words (2.0L);
  add_to_ored_words (4.0L);
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
        [gl_cv_cc_long_double_expbit0=`cat conftest.out`],
        [gl_cv_cc_long_double_expbit0="unknown"],
        [
          dnl When cross-compiling, we don't know. It depends on the
          dnl ABI and compiler version. There are too many cases.
          gl_cv_cc_long_double_expbit0="unknown"
        ])
      rm -f conftest.out
    ])
  case "$gl_cv_cc_long_double_expbit0" in
    word*bit*)
      word=`echo "$gl_cv_cc_long_double_expbit0" | sed -e 's/word //' -e 's/ bit.*//'`
      bit=`echo "$gl_cv_cc_long_double_expbit0" | sed -e 's/word.*bit //'`
      AC_DEFINE_UNQUOTED([LDBL_EXPBIT0_WORD], [$word],
        [Define as the word index where to find the exponent of 'long double'.])
      AC_DEFINE_UNQUOTED([LDBL_EXPBIT0_BIT], [$bit],
        [Define as the bit index in the word where to find bit 0 of the exponent of 'long double'.])
      ;;
  esac
])
