# isnan.m4 serial 2
dnl Copyright (C) 2007-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ISNAN],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_ISNANF])
  AC_REQUIRE([gl_FUNC_ISNAND])
  AC_REQUIRE([gl_FUNC_ISNANL])

  # If we replaced any of the underlying isnan* functions, replace
  # the isnan macro; it undoubtedly suffers from the same flaws.
  AC_MSG_CHECKING([whether isnan macro works])
  if test $gl_func_isnanf = yes \
     && test $gl_func_isnand = yes \
     && test $gl_func_isnanl = yes; then
    AC_MSG_RESULT([yes])
    ISNAN_LIBM="$ISNANF_LIBM $ISNAND_LIBM $ISNANL_LIBM"
  else
    AC_MSG_RESULT([no])
    REPLACE_ISNAN=1
    # Make sure the rpl_isnan[fdl] functions get built.
    gl_BUILD_ISNANF
    gl_BUILD_ISNAND
    gl_BUILD_ISNANL
    ISNAN_LIBM=
  fi
  AC_SUBST([ISNAN_LIBM])
])
