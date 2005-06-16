#serial 4

# Copyright (C) 2002 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_BISON],
[
  # getdate.y works with bison only.
  : ${YACC='bison -y'}
  AC_ARG_VAR(YACC,
[The `Yet Another C Compiler' implementation to use.  Defaults to `bison -y'.
Values other than `bison -y' will most likely break on most systems.])dnl
  AC_ARG_VAR(YFLAGS,
[YFLAGS contains the list arguments that will be passed by default to Bison.
This script will default YFLAGS to the empty string to avoid a default value of
`-d' given by some make applications.])dnl
])
