# strtok_r.m4
# serial 17
dnl Copyright (C) 2002-2004, 2006-2007, 2009-2025 Free Software Foundation,
dnl Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_STRTOK_R],
[
  dnl The strtok_r() declaration in lib/string.in.h uses 'restrict'.
  AC_REQUIRE([AC_C_RESTRICT])

  AC_REQUIRE([gl_STRING_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CHECK_FUNCS([strtok_r])
  if test $ac_cv_func_strtok_r = yes; then
    HAVE_STRTOK_R=1
    dnl glibc 2.7 has a bug in strtok_r that causes a segmentation fault
    dnl when the second argument to strtok_r is a constant string that has
    dnl exactly one byte and compiling with optimization.  This bug is, for
    dnl example, present in the glibc 2.7-18 package in Debian "lenny".
    dnl See <https://sourceware.org/PR5614>.
    AC_CACHE_CHECK([whether strtok_r works], [gl_cv_func_strtok_r_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM([[
              #ifndef __OPTIMIZE__
              # define __OPTIMIZE__ 1
              #endif
              #undef __OPTIMIZE_SIZE__
              #undef __NO_INLINE__
              #include <stdlib.h>
              #include <string.h>
            ]],
            [[static const char dummy[] = "\177\01a";
              char delimiters[] = "xxxxxxxx";
              char *save_ptr = (char *) dummy;
              strtok_r (delimiters, "x", &save_ptr);
              strtok_r (NULL, "x", &save_ptr);
              return 0;
            ]])
         ],
         [gl_cv_func_strtok_r_works=yes],
         [gl_cv_func_strtok_r_works=no],
         [case "$host_os" in
                               # Guess no on glibc systems.
            *-gnu* | gnu*)     gl_cv_func_strtok_r_works="guessing no" ;;
                               # Guess yes on native Windows.
            mingw* | windows*) gl_cv_func_strtok_r_works="guessing yes" ;;
            *)                 gl_cv_func_strtok_r_works="guessing yes" ;;
          esac
         ])
      ])
    case "$gl_cv_func_strtok_r_works" in
      *no)
        dnl We could set REPLACE_STRTOK_R=1 here, but it's only the macro
        dnl version in <bits/string2.h> which is wrong. The code compiled
        dnl into libc is fine.
        UNDEFINE_STRTOK_R=1
        ;;
    esac
  else
    HAVE_STRTOK_R=0
  fi
  AC_CHECK_DECLS_ONCE([strtok_r])
  if test $ac_cv_have_decl_strtok_r = no; then
    HAVE_DECL_STRTOK_R=0
  fi
])

# Prerequisites of lib/strtok_r.c.
AC_DEFUN([gl_PREREQ_STRTOK_R], [
  :
])
