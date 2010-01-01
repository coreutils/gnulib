# serial 6

# Copyright (C) 2009, 2010 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by David Bartley.

AC_DEFUN([gl_PRIV_SET],
[
  AC_REQUIRE([AC_C_INLINE])
  AC_CHECK_FUNCS([getppriv])
])
