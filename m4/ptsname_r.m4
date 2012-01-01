# ptsname_r.m4 serial 2
dnl Copyright (C) 2010-2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_PTSNAME_R],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])

  dnl Persuade glibc <stdlib.h> to declare ptsname_r().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_CHECK_FUNCS_ONCE([ptsname_r])
  if test $ac_cv_func_ptsname_r = no; then
    HAVE_PTSNAME_R=0
  else
    dnl On OSF/1 5.1, the type of the third argument is 'int', not 'size_t',
    dnl and the declaration is missing if _REENTRANT is not defined.
    AC_CACHE_CHECK([whether ptsname_r has the same signature as in glibc],
      [gl_cv_func_ptsname_r_signature_ok],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stddef.h>
              #include <stdlib.h>
              /* Test whether ptsname_r is declared at all.  */
              int (*f) (int, char *, size_t) = ptsname_r;
              /* Test whether it has the same declaration as in glibc.  */
              #undef ptsname_r
              extern
              #ifdef __cplusplus
              "C"
              #endif
              int ptsname_r (int, char *, size_t);
            ]],
            [[return f (0, NULL, 0);]])
         ],
         [gl_cv_func_ptsname_r_signature_ok=yes],
         [gl_cv_func_ptsname_r_signature_ok=no])
      ])
    if test $gl_cv_func_ptsname_r_signature_ok = no; then
      REPLACE_PTSNAME_R=1
    fi
  fi
])

# Prerequisites of lib/ptsname.c.
AC_DEFUN([gl_PREREQ_PTSNAME_R], [
  :
])
