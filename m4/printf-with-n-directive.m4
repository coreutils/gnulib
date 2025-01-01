# printf-with-n-directive.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl This macro is a witness that support of the %n directive in *printf
dnl functions is desired.
AC_DEFUN([gl_PRINTF_SUPPORT_N_DIRECTIVE], [
  AC_DEFINE([NEED_PRINTF_WITH_N_DIRECTIVE], [1],
    [Define to 1 to enable support of the %n directive in *printf functions.])
])
