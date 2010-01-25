# mathfunc.m4 serial 2
dnl Copyright (C) 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# gl_MATHFUNC(FUNC, RETTYPE, PARAMTYPES)
# --------------------------------------------------
# tests whether the function FUNC is available in libc or libm.
# RETTYPE is the return type. PARAMTYPES is a parameter list, with parentheses.
# It sets FUNC_LIBM to empty or "-lm" accordingly.

AC_DEFUN([gl_MATHFUNC],
[
  dnl We need the RETTYPE and PARAMTYPES in order to force linking with the
  dnl function. With gcc >= 4.3 on glibc/x86_64, calls to the 'fabs' function
  dnl are inlined by the compiler, therefore linking of these calls does not
  dnl require -lm, but taking the function pointer of 'fabs' does.
  m4_pushdef([func], [$1])
  m4_pushdef([FUNC], [translit([$1],[abcdefghijklmnopqrstuvwxyz],
                                    [ABCDEFGHIJKLMNOPQRSTUVWXYZ])])
  FUNC[]_LIBM=
  AC_CACHE_CHECK([whether func() can be used without linking with libm],
    [gl_cv_func_]func[_no_libm],
    [
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[#ifndef __NO_MATH_INLINES
                           # define __NO_MATH_INLINES 1 /* for glibc */
                           #endif
                           #include <math.h>
                           $2 (*funcptr) $3 = ]func[;]],
                         [[return 0;]])],
        [gl_cv_func_]func[_no_libm=yes],
        [gl_cv_func_]func[_no_libm=no])
    ])
  if test $gl_cv_func_[]func[]_no_libm = no; then
    AC_CACHE_CHECK([whether func() can be used with libm],
      [gl_cv_func_]func[_in_libm],
      [
        save_LIBS="$LIBS"
        LIBS="$LIBS -lm"
        AC_LINK_IFELSE(
          [AC_LANG_PROGRAM([[#ifndef __NO_MATH_INLINES
                             # define __NO_MATH_INLINES 1 /* for glibc */
                             #endif
                             #include <math.h>
                             $2 (*funcptr) $3 = ]func[;]],
                           [[return 0;]])],
          [gl_cv_func_]func[_in_libm=yes],
          [gl_cv_func_]func[_in_libm=no])
        LIBS="$save_LIBS"
      ])
    if test $gl_cv_func_[]func[]_in_libm = yes; then
      FUNC[]_LIBM=-lm
    fi
  fi
  AC_SUBST(FUNC[_LIBM])
  m4_popdef([FUNC])
  m4_popdef([func])
])
