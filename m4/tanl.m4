# tanl.m4 serial 2
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TANL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare tanl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  TANL_LIBM=
  AC_CACHE_CHECK([whether tanl() can be used without linking with libm],
    [gl_cv_func_tanl_no_libm],
    [
      AC_TRY_LINK([#ifndef __NO_MATH_INLINES
                   # define __NO_MATH_INLINES 1 /* for glibc */
                   #endif
                   #include <math.h>
                   long double x;],
                  [return tanl (x) > 0.4;],
        [gl_cv_func_tanl_no_libm=yes],
        [gl_cv_func_tanl_no_libm=no])
    ])
  if test $gl_cv_func_tanl_no_libm = no; then
    AC_CACHE_CHECK([whether tanl() can be used with libm],
      [gl_cv_func_tanl_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_TRY_LINK([#ifndef __NO_MATH_INLINES
                     # define __NO_MATH_INLINES 1 /* for glibc */
                     #endif
                     #include <math.h>
                     long double x;],
                    [return tanl (x) > 0.4;],
          [gl_cv_func_tanl_in_libm=yes],
          [gl_cv_func_tanl_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_tanl_in_libm = yes; then
      TANL_LIBM=-lm
    fi
  fi
  if test $gl_cv_func_tanl_no_libm = yes \
     || test $gl_cv_func_tanl_in_libm = yes; then
    dnl Also check whether it's declared.
    dnl MacOS X 10.3 has tanl() in libc but doesn't declare it in <math.h>.
    AC_CHECK_DECL([tanl], , [HAVE_DECL_TANL=0], [#include <math.h>])
  else
    HAVE_DECL_TANL=0
    HAVE_TANL=0
    AC_LIBOBJ([tanl])
    AC_LIBOBJ([trigl])
    AC_REQUIRE([gl_FUNC_ISNANL])
    AC_REQUIRE([gl_FUNC_FLOOR])
    AC_REQUIRE([gl_FUNC_FLOORL])
    dnl Append $ISNANL_LIBM to TANL_LIBM, avoiding gratuitous duplicates.
    case " $TANL_LIBM " in
      *" $ISNANL_LIBM "*) ;;
      *) TANL_LIBM="$TANL_LIBM $ISNANL_LIBM" ;;
    esac
    dnl Append $FLOOR_LIBM to TANL_LIBM, avoiding gratuitous duplicates.
    case " $TANL_LIBM " in
      *" $FLOOR_LIBM "*) ;;
      *) TANL_LIBM="$TANL_LIBM $FLOOR_LIBM" ;;
    esac
    dnl Append $FLOORL_LIBM to TANL_LIBM, avoiding gratuitous duplicates.
    case " $TANL_LIBM " in
      *" $FLOORL_LIBM "*) ;;
      *) TANL_LIBM="$TANL_LIBM $FLOORL_LIBM" ;;
    esac
  fi
  AC_SUBST([TANL_LIBM])
])
