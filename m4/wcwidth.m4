# wcwidth.m4 serial 2
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_WCWIDTH],
[
  dnl Persuade glibc <wchar.h> to declare wcwidth().
  AC_REQUIRE([AC_GNU_SOURCE])

  AC_REQUIRE([AC_C_INLINE])
  AC_REQUIRE([gt_TYPE_WCHAR_T])

  AC_CHECK_HEADERS_ONCE([wchar.h wctype.h])
  AC_CHECK_FUNCS_ONCE([iswprint wcwidth])

  AC_CACHE_CHECK([whether wcwidth is declared], [ac_cv_have_decl_wcwidth],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
/* AIX 3.2.5 declares wcwidth in <string.h>. */
#if HAVE_STRING_H
# include <string.h>
#endif
#if HAVE_WCHAR_H
# include <wchar.h>
#endif
], [
#ifndef wcwidth
  char *p = (char *) wcwidth;
#endif
])], [ac_cv_have_decl_wcwidth=yes], [ac_cv_have_decl_wcwidth=no])])
  if test $ac_cv_have_decl_wcwidth = yes; then
    ac_val=1
  else
    ac_val=0
  fi
  AC_DEFINE_UNQUOTED([HAVE_DECL_WCWIDTH], [$ac_val],
    [Define to 1 if you have the declaration of wcwidth(), and to 0
     otherwise.])
]) 
