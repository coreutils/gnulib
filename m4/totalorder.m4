# totalorder.m4
dnl Copyright 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_TOTALORDERF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([totalorderf], [int],
    [(float const *, float const *)],
    [extern
     #ifdef __cplusplus
     "C"
     #endif
     int totalorderf (float const *, float const *);
    ])
  AS_IF([test $gl_cv_func_totalorderf_no_libm != yes &&
         test $gl_cv_func_totalorderf_in_libm != yes],
    [gl_saved_LIBS=$LIBS
     AC_SEARCH_LIBS([totalorderf], [m])
     LIBS=$gl_saved_LIBS
     if test "$ac_cv_search_totalorderf" = no; then
       HAVE_TOTALORDERF=0
     else
       REPLACE_TOTALORDERF=1
     fi
     TOTALORDER_LIBM='$(ISNANF_LIBM)'])
  AC_SUBST([TOTALORDERF_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDER],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([totalorder], [int],
    [(double const *, double const *)],
    [extern
     #ifdef __cplusplus
     "C"
     #endif
     int totalorder (double const *, double const *);
    ])
  AS_IF([test $gl_cv_func_totalorder_no_libm != yes &&
         test $gl_cv_func_totalorder_in_libm != yes],
    [gl_saved_LIBS=$LIBS
     AC_SEARCH_LIBS([totalorder], [m])
     LIBS=$gl_saved_LIBS
     if test "$ac_cv_search_totalorder" = no; then
       HAVE_TOTALORDER=0
     else
       REPLACE_TOTALORDER=1
     fi
     TOTALORDER_LIBM='$(ISNAND_LIBM)'])
  AC_SUBST([TOTALORDER_LIBM])
])

AC_DEFUN([gl_FUNC_TOTALORDERL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_MATHFUNC([totalorderl], [int],
    [(long double const *, long double const *)],
    [extern
     #ifdef __cplusplus
     "C"
     #endif
     int totalorderl (long double const *, long double const *);
    ])
  AS_IF([test $gl_cv_func_totalorderl_no_libm != yes &&
         test $gl_cv_func_totalorderl_in_libm != yes],
    [gl_saved_LIBS=$LIBS
     AC_SEARCH_LIBS([totalorderl], [m])
     LIBS=$gl_saved_LIBS
     if test "$ac_cv_search_totalorderl" = no; then
       HAVE_TOTALORDERL=0
     else
       REPLACE_TOTALORDERL=1
     fi
     TOTALORDERL_LIBM='$(ISNANL_LIBM)'])
  AC_SUBST([TOTALORDERL_LIBM])
])
