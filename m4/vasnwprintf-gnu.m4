# vasnwprintf-gnu.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_VASNWPRINTF_GNU],
[
  AC_REQUIRE([gl_FUNC_VASNWPRINTF_IS_POSIX])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_UPPERCASE_B])
  gl_cv_func_vasnwprintf_gnu=no
  if test $gl_cv_func_vasnwprintf_posix = yes; then
    case "$gl_cv_func_printf_directive_uppercase_b" in
      *yes)
        # vasnwprintf exists and is already POSIX compliant and GNU compatible.
        gl_cv_func_vasnwprintf_gnu=yes
        ;;
    esac
  fi
  if test $gl_cv_func_vasnwprintf_gnu = no; then
    dnl Note: This invokes gl_PREREQ_VASNPRINTF_DIRECTIVE_LC although not needed
    dnl here. Doesn't matter.
    gl_PREREQ_VASNPRINTF_WITH_GNU_EXTRAS
    gl_FUNC_VASNWPRINTF
  fi
])
