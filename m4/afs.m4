# serial 10

# Copyright (C) 1999-2001, 2004, 2008-2016 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_AFS],
  [
    AC_ARG_WITH([afs],
                AS_HELP_STRING([--with-afs],
                               [support for the Andrew File System [[default=no]]]),
    test "$withval" = no || with_afs=yes, with_afs=no)
    if test "$with_afs" = yes; then
      AC_DEFINE([AFS], [1], [Define if you have the Andrew File System.])
    fi
  ])
