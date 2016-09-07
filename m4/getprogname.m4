# getprogname.m4 - check for getprogname or replacements for it

# Copyright (C) 2016 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

AC_DEFUN([gl_FUNC_GETPROGNAME],
[
  AC_CHECK_FUNCS_ONCE([getprogname getexecname])
])
