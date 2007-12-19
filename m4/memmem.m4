# memmem.m4 serial 6
dnl Copyright (C) 2002, 2003, 2004, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MEMMEM],
[
  dnl Persuade glibc <string.h> to declare memmem().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REPLACE_FUNCS(memmem)
  AC_CHECK_DECLS_ONCE(memmem)
  if test $ac_cv_have_decl_memmem = no; then
    HAVE_DECL_MEMMEM=0
  else
    AC_CACHE_CHECK([whether memmem works], [gl_cv_func_memmem_works],
      [AC_RUN_IFELSE([AC_LANG_PROGRAM([#include <string.h>],
	  [return !memmem ("a", 1, NULL, 0);])],
	[gl_cv_func_memmem_works=yes], [gl_cv_func_memmem_works=no],
	[dnl pessimistically assume the worst, since even glibc 2.6.1
	 dnl has quadratic complexity in its memmem
	 gl_cv_func_memmem_works=no])])
    if test $gl_cv_func_memmem_works = no; then
      REPLACE_MEMMEM=1
      AC_LIBOBJ([memmem])
    fi
  fi
  gl_PREREQ_MEMMEM
])

# Prerequisites of lib/memmem.c.
AC_DEFUN([gl_PREREQ_MEMMEM], [:])
