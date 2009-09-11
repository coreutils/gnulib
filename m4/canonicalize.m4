# canonicalize.m4 serial 14

# Copyright (C) 2003, 2004, 2005, 2006, 2007, 2009 Free Software
# Foundation, Inc.

# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Jim Meyering.

AC_DEFUN([gl_FUNC_CANONICALIZE_FILENAME_MODE],
[
  AC_LIBOBJ([canonicalize])

  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([canonicalize_file_name])
])
