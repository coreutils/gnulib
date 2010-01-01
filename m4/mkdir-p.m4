# mkdir-p.m4 serial 13
dnl Copyright (C) 2002-2006, 2009-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_MKDIR_PARENTS],
[
  AC_LIBOBJ([dirchownmod])
  AC_LIBOBJ([mkdir-p])

  dnl Prerequisites of lib/dirchownmod.c.
  AC_REQUIRE([gl_FUNC_LCHMOD])
  AC_REQUIRE([gl_FUNC_LCHOWN])
  AC_CHECK_FUNCS_ONCE([fchmod])
])
