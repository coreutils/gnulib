# expm1l.m4 serial 2
dnl Copyright (C) 2010-2016 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_EXPM1L],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])

  dnl Persuade glibc <math.h> to declare expm1l().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  EXPM1L_LIBM=
  AC_CACHE_CHECK([whether expm1l() can be used without linking with libm],
    [gl_cv_func_expm1l_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#ifndef __NO_MATH_INLINES
             # define __NO_MATH_INLINES 1 /* for glibc */
             #endif
             #include <math.h>
             extern
             #ifdef __cplusplus
             "C"
             #endif
             long double expm1l (long double);
             long double (*funcptr) (long double) = expm1l;
             long double x;]],
           [[return funcptr (x) > 0.5
                    || expm1l (x) > 0.5;]])],
        [gl_cv_func_expm1l_no_libm=yes],
        [gl_cv_func_expm1l_no_libm=no])
    ])
  if test $gl_cv_func_expm1l_no_libm = no; then
    AC_CACHE_CHECK([whether expm1l() can be used with libm],
      [gl_cv_func_expm1l_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM(
             [[#ifndef __NO_MATH_INLINES
               # define __NO_MATH_INLINES 1 /* for glibc */
               #endif
               #include <math.h>
               extern
               #ifdef __cplusplus
               "C"
               #endif
               long double expm1l (long double);
               long double (*funcptr) (long double) = expm1l;
               long double x;]],
             [[return funcptr (x) > 0.5
                      || expm1l (x) > 0.5;]])],
          [gl_cv_func_expm1l_in_libm=yes],
          [gl_cv_func_expm1l_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_expm1l_in_libm = yes; then
      EXPM1L_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_expm1l_no_libm = yes \
     || test $gl_cv_func_expm1l_in_libm = yes; then
    HAVE_EXPM1L=1
    dnl Also check whether it's declared.
    dnl IRIX 6.5 has expm1l() in libm but doesn't declare it in <math.h>.
    AC_CHECK_DECL([expm1l], , [HAVE_DECL_EXPM1L=0], [[#include <math.h>]])
  else
    HAVE_EXPM1L=0
    HAVE_DECL_EXPM1L=0
    dnl Find libraries needed to link lib/expm1l.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      AC_REQUIRE([gl_FUNC_EXPM1])
      EXPM1L_LIBM="$EXPM1_LIBM"
    else
      AC_REQUIRE([gl_FUNC_ISNANL])
      AC_REQUIRE([gl_FUNC_EXPL])
      AC_REQUIRE([gl_FUNC_ROUNDL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      EXPM1L_LIBM=
      dnl Append $ISNANL_LIBM to EXPM1L_LIBM, avoiding gratuitous duplicates.
      case " $EXPM1L_LIBM " in
        *" $ISNANL_LIBM "*) ;;
        *) EXPM1L_LIBM="$EXPM1L_LIBM $ISNANL_LIBM" ;;
      esac
      dnl Append $EXPL_LIBM to EXPM1L_LIBM, avoiding gratuitous duplicates.
      case " $EXPM1L_LIBM " in
        *" $EXPL_LIBM "*) ;;
        *) EXPM1L_LIBM="$EXPM1L_LIBM $EXPL_LIBM" ;;
      esac
      dnl Append $ROUNDL_LIBM to EXPM1L_LIBM, avoiding gratuitous duplicates.
      case " $EXPM1L_LIBM " in
        *" $ROUNDL_LIBM "*) ;;
        *) EXPM1L_LIBM="$EXPM1L_LIBM $ROUNDL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to EXPM1L_LIBM, avoiding gratuitous duplicates.
      case " $EXPM1L_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) EXPM1L_LIBM="$EXPM1L_LIBM $LDEXPL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([EXPM1L_LIBM])
])
