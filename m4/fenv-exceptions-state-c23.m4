# fenv-exceptions-state-c23.m4
# serial 1
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FENV_EXCEPTIONS_STATE_C23],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])

  gl_MATHFUNC([fetestexceptflag], [int], [(fexcept_t const *, int)],
    [#include <fenv.h>
     fexcept_t fx_ret;
    ])
  if test $gl_cv_func_fetestexceptflag_no_libm != yes \
     && test $gl_cv_func_fetestexceptflag_in_libm != yes; then
    HAVE_FETESTEXCEPTFLAG=0
  fi
])
