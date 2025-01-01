# ldexpf.m4
# serial 3
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_LDEXPF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_LDEXP])

  dnl Persuade glibc <math.h> to declare ldexpf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Test whether ldexpf() exists. We cannot assume that ldexpf(), if it
  dnl exists, is defined in the same library as ldexp(). This is not the case
  dnl on FreeBSD, NetBSD, OpenBSD.
  gl_MATHFUNC([ldexpf], [float], [(float, int)])
  if test $gl_cv_func_ldexpf_no_libm = no \
     && test $gl_cv_func_ldexpf_in_libm = no; then
    HAVE_LDEXPF=0
    LDEXPF_LIBM="$LDEXP_LIBM"
  fi
  AC_SUBST([LDEXPF_LIBM])
])

dnl Test whether ldexpf() can be used without linking with libm.
dnl Set gl_cv_func_ldexpf_no_libm to 'yes' or 'no' accordingly.
AC_DEFUN([gl_CHECK_LDEXPF_NO_LIBM],
[
  AC_CACHE_CHECK([whether ldexpf() can be used without linking with libm],
    [gl_cv_func_ldexpf_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[#ifndef __NO_MATH_INLINES
                           # define __NO_MATH_INLINES 1 /* for glibc */
                           #endif
                           #include <math.h>
                           float (*funcptr) (float, int) = ldexpf;
                           float x;]],
                         [[return ldexpf (x, -1) > 0;]])],
        [gl_cv_func_ldexpf_no_libm=yes],
        [gl_cv_func_ldexpf_no_libm=no])
    ])
])
