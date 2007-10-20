# check-libm.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl 
dnl AC_CHECK_LIBM_FUNC (MATH_FUNCTION, INVOCATION, 
dnl                     [RUN-IF-FOUND], [RUN-IF-NOT-FOUND])
dnl
dnl Checks for a declaration of the given MATH_FUNCTION in <math.h>, and
dnl substitutes HAVE_DECL_<func> accordingly.  If a declaration is found,
dnl determines the needed library (if any), assigns it to <func>_LIBM, and
dnl executes RUN-IF-FOUND; otherwise, executes RUN-IF-NOT-FOUND.
dnl 
dnl INVOCATION should be a C statement that invokes MATH_FUNCTION, both
dnl using and assigning back to double variable 'x', e.g. "x = floor
dnl (x);".
AC_DEFUN([gl_CHECK_LIBM_FUNC],
[
m4_pushdef([FUNC_LIBM], m4_toupper([$1])[_LIBM])dnl
m4_pushdef([HAVE_DECL_FUNC], HAVE_DECL_[]m4_toupper([$1]))dnl
  AC_CHECK_DECLS([$1], , , [#include <math.h>])
  if test "$ac_cv_have_decl_$1" = yes; then
    save_LIBS=$LIBS
    FUNC_LIBM=?
    for libm in "" "-lm"; do
      LIBS="$save_LIBS $libm"
      AC_TRY_LINK([
	 #ifndef __NO_MATH_INLINES
	 # define __NO_MATH_INLINES 1 /* for glibc */
	 #endif
	 #include <math.h>
	 double x;],
	[$2],
	[FUNC_LIBM=$libm
break])
    done
    LIBS=$save_LIBS
    if test "$FUNC_LIBM" = "?"; then
      FUNC_LIBM=
    fi
m4_ifvaln([$3], [$3])dnl
  else
    HAVE_DECL_FUNC=
    FUNC_LIBM=
m4_ifvaln([$4], [$4])dnl
  fi
  AC_SUBST(HAVE_DECL_FUNC)
  AC_SUBST(FUNC_LIBM)
m4_popdef([FUNC_LIBM])
m4_popdef([HAVE_DECL_FUNC])])
