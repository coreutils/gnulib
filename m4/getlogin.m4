# getlogin.m4
# serial 8
dnl Copyright (C) 2010-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_GETLOGIN],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_CHECK_DECLS_ONCE([getlogin])
  if test $ac_cv_have_decl_getlogin = no; then
    HAVE_DECL_GETLOGIN=0
  fi
  AC_CHECK_FUNCS_ONCE([getlogin])
  if test $ac_cv_func_getlogin = no; then
    HAVE_GETLOGIN=0
  else
    dnl On musl libc, getlogin returns getenv ("LOGNAME").
    AC_REQUIRE([AC_CANONICAL_HOST])
    AC_CACHE_CHECK([whether getlogin works],
      [gl_cv_func_getlogin_works],
      [
        dnl Initial guess, used when cross-compiling.
changequote(,)dnl
        case "$host_os" in
                              # Guess no on musl libc.
          *-musl* | midipix*) gl_cv_func_getlogin_works="guessing no" ;;
                              # Guess yes otherwise.
          *)                  gl_cv_func_getlogin_works="guessing yes" ;;
        esac
changequote([,])dnl
        AC_RUN_IFELSE(
          [AC_LANG_SOURCE([[
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if !HAVE_DECL_GETLOGIN
extern
# ifdef __cplusplus
"C"
# endif
char *getlogin (void);
#endif
int
main (void)
{
  int result = 0;
  char *buf;

  putenv ("LOGNAME=ygvfibmslhkmvoetbrcegzwydorcke");
  buf = getlogin ();
  if (buf != NULL && strcmp (buf, "ygvfibmslhkmvoetbrcegzwydorcke") == 0)
    result |= 1;
  return result;
}]])],
          [gl_cv_func_getlogin_works=yes],
          [gl_cv_func_getlogin_works=no],
          [:])
      ])
    case "$gl_cv_func_getlogin_works" in
      *yes) ;;
      *) REPLACE_GETLOGIN=1 ;;
    esac
  fi
])

dnl Determines the library needed by the implementation of the
dnl getlogin and getlogin_r functions.
AC_DEFUN([gl_LIB_GETLOGIN],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  case $host_os in
    mingw* | windows*)
      GETLOGIN_LIB='-ladvapi32' ;;
    *)
      GETLOGIN_LIB= ;;
  esac
  AC_SUBST([GETLOGIN_LIB])
  dnl For backward compatibility.
  LIB_GETLOGIN="$GETLOGIN_LIB"
  AC_SUBST([LIB_GETLOGIN])
])
