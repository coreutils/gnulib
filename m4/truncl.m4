# truncl.m4 serial 4
dnl Copyright (C) 2007-2008, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TRUNCL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  dnl Persuade glibc <math.h> to declare truncl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether truncl() is declared.
  AC_CHECK_DECLS([truncl], , , [#include <math.h>])
  if test "$ac_cv_have_decl_truncl" = yes; then
    dnl Test whether truncl() can be used without libm.
    TRUNCL_LIBM=?
    AC_TRY_LINK([
       #ifndef __NO_MATH_INLINES
       # define __NO_MATH_INLINES 1 /* for glibc */
       #endif
       #include <math.h>
       long double x;],
      [x = truncl(x);],
      [TRUNCL_LIBM=])
    if test "$TRUNCL_LIBM" = "?"; then
      save_LIBS="$LIBS"
      LIBS="$LIBS -lm"
      AC_TRY_LINK([
         #ifndef __NO_MATH_INLINES
         # define __NO_MATH_INLINES 1 /* for glibc */
         #endif
         #include <math.h>
         long double x;],
        [x = truncl(x);],
        [TRUNCL_LIBM="-lm"])
      LIBS="$save_LIBS"
    fi
    if test "$TRUNCL_LIBM" = "?"; then
      TRUNCL_LIBM=
    fi
    dnl Test whether truncl() works. It crashes on OSF/1 4.0d.
    AC_CACHE_CHECK([whether truncl works], [gl_cv_func_truncl_works],
      [
        AC_TRY_RUN([
#include <math.h>
long double x;
int main()
{
  x = truncl (0.0L);
  return 0;
}], [gl_cv_func_truncl_works=yes], [gl_cv_func_truncl_works=no],
          [case "$host_os" in
             osf4*) gl_cv_func_truncl_works="guessing no";;
             *)     gl_cv_func_truncl_works="guessing yes";;
           esac
          ])
      ])
    case "$gl_cv_func_truncl_works" in
      *yes) ;;
      *) REPLACE_TRUNCL=1 ;;
    esac
  else
    HAVE_DECL_TRUNCL=0
  fi
  if test $HAVE_DECL_TRUNCL = 0 || test $REPLACE_TRUNCL = 1; then
    AC_LIBOBJ([truncl])
    TRUNCL_LIBM=
  fi
  AC_SUBST([TRUNCL_LIBM])
])
