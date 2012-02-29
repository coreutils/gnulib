# hypotl.m4 serial 1
dnl Copyright (C) 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_HYPOTL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_HYPOT])

  dnl Test whether hypotl() exists. Assume that hypotl(), if it exists, is
  dnl defined in the same library as hypot().
  save_LIBS="$LIBS"
  LIBS="$LIBS $HYPOT_LIBM"
  AC_CHECK_FUNCS([hypotl])
  LIBS="$save_LIBS"
  if test $ac_cv_func_hypotl = yes; then
    HYPOTL_LIBM="$HYPOT_LIBM"
  else
    HAVE_HYPOTL=0
    dnl Find libraries needed to link lib/hypotl.c.
    if test $HAVE_SAME_LONG_DOUBLE_AS_DOUBLE = 1; then
      HYPOTL_LIBM="$HYPOT_LIBM"
    else
      AC_REQUIRE([gl_FUNC_FABSL])
      AC_REQUIRE([gl_FUNC_FREXPL])
      AC_REQUIRE([gl_FUNC_LDEXPL])
      AC_REQUIRE([gl_FUNC_SQRTL])
      HYPOTL_LIBM=
      dnl Append $FABSL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $FABSL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $FABSL_LIBM" ;;
      esac
      dnl Append $FREXPL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $FREXPL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $FREXPL_LIBM" ;;
      esac
      dnl Append $LDEXPL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $LDEXPL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $LDEXPL_LIBM" ;;
      esac
      dnl Append $SQRTL_LIBM to HYPOTL_LIBM, avoiding gratuitous duplicates.
      case " $HYPOTL_LIBM " in
        *" $SQRTL_LIBM "*) ;;
        *) HYPOTL_LIBM="$HYPOTL_LIBM $SQRTL_LIBM" ;;
      esac
    fi
  fi
  AC_SUBST([HYPOTL_LIBM])
])
