# obstack-printf-gnu.m4
# serial 2
dnl Copyright (C) 2023-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Part 1 of gl_FUNC_OBSTACK_PRINTF_GNU:
dnl Sets REPLACE_OBSTACK_PRINTF if needed.
AC_DEFUN_ONCE([gl_FUNC_OBSTACK_PRINTF_GNU_REPLACE],
[
  AC_REQUIRE([gl_FUNC_OBSTACK_PRINTF_IS_POSIX])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_UPPERCASE_B])
  gl_cv_func_obstack_printf_gnu=no
  if test $gl_cv_func_obstack_printf_posix = yes; then
    case "$gl_cv_func_printf_directive_uppercase_b" in
      *yes)
        # obstack_printf exists and is already POSIX compliant and GNU compatible.
        gl_cv_func_obstack_printf_gnu=yes
        ;;
    esac
  fi
  if test $gl_cv_func_obstack_printf_gnu = no; then
    gl_REPLACE_OBSTACK_PRINTF
  fi
])

AC_DEFUN([gl_FUNC_OBSTACK_PRINTF_GNU],
[
  gl_FUNC_OBSTACK_PRINTF_REPLACE
  if test $gl_cv_func_obstack_printf_gnu = no; then
    gl_PREREQ_VASNPRINTF_WITH_GNU_EXTRAS
    gl_REPLACE_VASNPRINTF
  fi

  gl_DECL_OBSTACK_PRINTF
])
