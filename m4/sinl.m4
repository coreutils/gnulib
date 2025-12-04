# sinl.m4
# serial 11
dnl Copyright (C) 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_SINL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare sinl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  SINL_LIBM=
  AC_CACHE_CHECK([whether sinl() can be used without linking with libm],
    [gl_cv_func_sinl_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             long double (* volatile funcptr) (long double) = sinl;
             long double x;]],
           [[return funcptr (x) > 0.4
                    || sinl (x) > 0.4;]])],
        [gl_cv_func_sinl_no_libm=yes],
        [gl_cv_func_sinl_no_libm=no])
    ])
  if test $gl_cv_func_sinl_no_libm = no; then
    AC_CACHE_CHECK([whether sinl() can be used with libm],
      [gl_cv_func_sinl_in_libm],
      [
        saved_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               long double (* volatile funcptr) (long double) = sinl;
               long double x;]],
             [[return funcptr (x) > 0.4
                      || sinl (x) > 0.4;]])],
          [gl_cv_func_sinl_in_libm=yes],
          [gl_cv_func_sinl_in_libm=no])
        LIBS="$saved_LIBS"
      ])
    if test $gl_cv_func_sinl_in_libm = yes; then
      SINL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_sinl_no_libm = yes \
     || test $gl_cv_func_sinl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl Mac OS X 10.3 has sinl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([sinl], , [HAVE_DECL_SINL=0], [[#include <math.h>]])
    if test $REPLACE_SINL = 0; then
      AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
      AC_CACHE_CHECK([whether sinl works],
        [gl_cv_func_sinl_works],
        [
          saved_LIBS="$LIBS"
          LIBS="$LIBS $SINL_LIBM"
          AC_RUN_IFELSE(
            [AC_LANG_SOURCE([[
#ifndef __NO_MATH_INLINES
# define __NO_MATH_INLINES 1 /* for glibc */
#endif
#include <math.h>
#undef sinl
extern
#ifdef __cplusplus
"C"
#endif
long double sinl (long double);
static long double dummy (long double x) { return 0; }
int main (int argc, char *argv[])
{
  long double (* volatile my_sinl) (long double) = argc ? sinl : dummy;
  int result = 0;
  /* On NetBSD 10.0 the system's native sinl() is buggy:
     it does not obey the inequality |sin(x)| <= |x|.  */
  {
    volatile long double x = 0.000000000000000004L;
    if (my_sinl (x) > x)
      result |= 1;
  }
  return result;
}
            ]])],
            [gl_cv_func_sinl_works=yes],
            [gl_cv_func_sinl_works=no],
            [case "$host_os" in
                                   # Guess yes on glibc systems.
               *-gnu* | gnu*)      gl_cv_func_sinl_works="guessing yes" ;;
                                   # Guess yes on musl systems.
               *-musl* | midipix*) gl_cv_func_sinl_works="guessing yes" ;;
                                   # Guess yes on native Windows.
               mingw* | windows*)  gl_cv_func_sinl_works="guessing yes" ;;
                                   # If we don't know, obey --enable-cross-guesses.
               *)                  gl_cv_func_sinl_works="$gl_cross_guess_normal" ;;
             esac
            ])
          LIBS="$saved_LIBS"
        ])
      case "$gl_cv_func_sinl_works" in
        *yes) ;;
        *) REPLACE_SINL=1 ;;
      esac
    fi
  else
    HAVE_DECL_SINL=0
    HAVE_SINL=0
  fi
  if test $HAVE_SINL = 0 || test $REPLACE_SINL = 1; then
    dnl Find libraries needed to link lib/sinl.c, lib/sincosl.c, lib/trigl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_SIN])
      SINL_LIBM="$SIN_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_FLOOR])
      AC_REQUIRE([gl_FUNC_FLOORL])
      dnl Append $ISNANL_LIBM to SINL_LIBM, avoiding gratuitous duplicates.
      case " $SINL_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) SINL_LIBM="$SINL_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $FLOOR_LIBM to SINL_LIBM, avoiding gratuitous duplicates.
      case " $SINL_LIBM " in
        *" $FLOOR_LIBM "*) ;;
        *) SINL_LIBM="$SINL_LIBM $FLOOR_LIBM" ;;
      esac
      dnl Append $FLOORL_LIBM to SINL_LIBM, avoiding gratuitous duplicates.
      case " $SINL_LIBM " in
        *" $FLOORL_LIBM "*) ;;
        *) SINL_LIBM="$SINL_LIBM $FLOORL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([SINL_LIBM])
])
