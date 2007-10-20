# roundl.m4 serial 1
dnl Copyright (C) 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_ROUNDL],
[
  AC_REQUIRE([gl_MATH_H_DEFAULTS])
  dnl Persuade glibc <math.h> to declare roundl().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Test whether roundl() is declared.
  gl_CHECK_LIBM_FUNC([roundl], [x = roundl(x);], [], [
    dnl No.  Are both floorl() and ceill() available?  If so then we can use
    dnl them to implement roundl(), on the assumption that they're fast.
    gl_CHECK_LIBM_FUNC([floorl], [x = floorl(x);], [
      AC_CHECK_DECL([ceill], 
        [dnl Yes.  Both are declared.  Link against the necessary library.
         ROUNDL_LIBM="$FLOORL_LIBM"],
        [: dnl No. We will use an implementation that doesn't need them.
], [#include <math.h>
])])])])
