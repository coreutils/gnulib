#serial 1

# Copyright (C) 2006 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# glibc provides __gen_tempname as a wrapper for mk[ds]temp.  Expose
# it as a public API, and provide it on systems that are lacking.
AC_DEFUN([gl_FUNC_GEN_TEMPNAME],
[
  AC_REQUIRE([AC_SYS_LARGEFILE])
  AC_CHECK_FUNCS_ONCE([__gen_tempname])

  AC_LIBOBJ([tempname])
  gl_PREREQ_TEMPNAME
])

# Prerequisites of lib/tempname.c.
AC_DEFUN([gl_PREREQ_TEMPNAME],
[
  AC_CHECK_HEADERS_ONCE([sys/time.h])
  AC_CHECK_FUNCS_ONCE([gettimeofday])
])
