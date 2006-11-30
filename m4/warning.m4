# serial 4
# Find valid warning flags for the C Compiler.           -*-Autoconf-*-

# Copyright (C) 2001, 2002, 2006 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Jesse Thilo.

AC_DEFUN([gl_WARNING_CFLAGS],
  [AC_MSG_CHECKING(whether compiler accepts $1)
   AC_SUBST(WARNING_CFLAGS)
   ac_save_CFLAGS="$CFLAGS"
   CFLAGS="$CFLAGS $1"
   AC_TRY_COMPILE(,
    [int x;],
    WARNING_CFLAGS="$WARNING_CFLAGS $1"
    AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no))
  CFLAGS="$ac_save_CFLAGS"
 ])
