# roundf.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDF],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundf().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether roundf() is declared.
  gl_CHECK_LIBM_FUNC([roundf], [x = roundf(x);], [], [
    dnl No.  Are both floorf() and ceilf() available?  If so then we can use
    dnl them to implement roundf(), on the assumption that they're fast.
    gl_CHECK_LIBM_FUNC([floorf], [x = floorf(x);], [
      AC_CHECK_DECL([ceilf], 
        [dnl Yes.  Both are declared.  Link against the necessary library.
         ROUNDF_LIBM="$FLOORF_LIBM"],
        [: dnl No. We will use an implementation that doesn't need them.
], [#include <math.h>
])])])])
