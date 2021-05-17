# Check prerequisites for compiling lib/c-stack.c.

# Copyright (C) 2002-2004, 2008-2021 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.

# serial 23

AC_DEFUN([gl_C_STACK],
[
  dnl 'c-stack' needs -lsigsegv if and only if the 'sigsegv' module needs it.
  if test "$with_libsigsegv" = yes; then
    if test "$gl_cv_lib_sigsegv" = yes; then
      AC_SUBST([LIBCSTACK], [$LIBSIGSEGV])
      AC_SUBST([LTLIBCSTACK], [$LTLIBSIGSEGV])
    fi
  fi
])
