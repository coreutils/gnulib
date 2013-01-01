# Check for variable-length arrays.

# serial 4

# From Paul Eggert

# Copyright (C) 2001, 2009-2013 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([AC_C_VARARRAYS],
[
  AC_CACHE_CHECK([for variable-length arrays],
    ac_cv_c_vararrays,
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
         [],
         [[static int x; char a[++x]; a[sizeof a - 1] = 0; return a[0];]])],
       ac_cv_c_vararrays=yes,
       ac_cv_c_vararrays=no)])
  if test $ac_cv_c_vararrays = yes; then
    AC_DEFINE([HAVE_C_VARARRAYS], [1],
      [Define to 1 if C supports variable-length arrays.])
  fi
])
