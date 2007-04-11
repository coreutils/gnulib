# vfprintf-posix.m4 serial 3
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_VFPRINTF_POSIX],
[
  AC_REQUIRE([gl_EOVERFLOW])
  AC_REQUIRE([gl_PRINTF_SIZES_C99])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_A])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_F])
  AC_REQUIRE([gl_PRINTF_DIRECTIVE_N])
  AC_REQUIRE([gl_PRINTF_POSITIONS])
  gl_cv_func_vfprintf_posix=no
  case "$gl_cv_func_printf_sizes_c99" in
    *yes)
      case "$gl_cv_func_printf_directive_a" in
        *yes)
          case "$gl_cv_func_printf_directive_f" in
            *yes)
              case "$gl_cv_func_printf_directive_n" in
                *yes)
                  case "$gl_cv_func_printf_positions" in
                    *yes)
                      # vfprintf exists and is already POSIX compliant.
                      gl_cv_func_vfprintf_posix=yes
                      ;;
                  esac
                  ;;
              esac
              ;;
          esac
          ;;
      esac
      ;;
  esac
  if test $gl_cv_func_vfprintf_posix = no; then
    gl_PREREQ_VASNPRINTF_DIRECTIVE_A
    gl_PREREQ_VASNPRINTF_DIRECTIVE_F
    gl_REPLACE_VASNPRINTF
    gl_REPLACE_VFPRINTF
  fi
])

AC_DEFUN([gl_REPLACE_VFPRINTF],
[
  AC_REQUIRE([gl_STDIO_H_DEFAULTS])
  AC_LIBOBJ([vfprintf])
  REPLACE_VFPRINTF=1
  gl_PREREQ_VFPRINTF
])

AC_DEFUN([gl_PREREQ_VFPRINTF], [:])
