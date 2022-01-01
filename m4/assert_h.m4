# assert-h.m4
dnl Copyright (C) 2011-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Paul Eggert.

AC_DEFUN([gl_ASSERT_H],
[
  GL_GENERATE_ASSERT_H=false
  AC_CACHE_CHECK([for static_assert], [gl_cv_static_assert],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <assert.h>
            static_assert (2 + 2 == 4, "arithmetic doesn't work");
            static_assert (2 + 2 == 4);
          ]],
          [[
            static_assert (sizeof (char) == 1, "sizeof doesn't work");
            static_assert (sizeof (char) == 1);
          ]])],
       [gl_cv_static_assert=yes],
       [gl_cv_static_assert=no])])
  if test $gl_cv_static_assert = no; then
    GL_GENERATE_ASSERT_H=true
    gl_NEXT_HEADERS([assert.h])
  fi
])
