# vasnwprintf-posix.m4 serial 2
dnl Copyright (C) 2007-2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_VASNWPRINTF_POSIX],
[
  gl_FUNC_VASNWPRINTF
  gl_PREREQ_VASNPRINTF_LONG_DOUBLE
  gl_PREREQ_VASNPRINTF_INFINITE_DOUBLE
  gl_PREREQ_VASNPRINTF_INFINITE_LONG_DOUBLE
  gl_PREREQ_VASNPRINTF_DIRECTIVE_A
  gl_PREREQ_VASNPRINTF_DIRECTIVE_B
  gl_PREREQ_VASNPRINTF_DIRECTIVE_F
  gl_PREREQ_VASNPRINTF_DIRECTIVE_LS
  gl_PREREQ_VASNPRINTF_FLAG_GROUPING
  gl_PREREQ_VASNPRINTF_FLAG_LEFTADJUST
  gl_PREREQ_VASNPRINTF_FLAG_ZERO
  gl_PREREQ_VASNPRINTF_PRECISION
  gl_PREREQ_VASNPRINTF_ENOMEM
])
