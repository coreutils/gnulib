# freadable.m4
# serial 4
dnl Copyright (C) 2007, 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_FREADABLE],
[
  AC_CHECK_HEADERS_ONCE([stdio_ext.h])
  gl_CHECK_FUNCS_ANDROID([__freadable], [[#include <stdio_ext.h>]])
])
