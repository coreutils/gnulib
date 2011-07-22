# Enable large inode numbers on systems normally without them. -*- Autoconf -*-

# Copyright (C) 2011 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# written by Paul Eggert

AC_DEFUN([gl_SYS_LARGE_INODE],
[
  dnl Many systems enable large inodes if you enable large offsets.
  AC_REQUIRE([AC_SYS_LARGEFILE])

  dnl Some Mac OS X variants won't access large inode numbers by default.
  dnl Defining _DARWIN_USE_64_BIT_INODE fixes this.  See
  dnl <http://developer.apple.com/library/mac/releasenotes/Darwin/SymbolVariantsRelNotes>.
  dnl
  dnl The simplest thing is to define this symbol everywhere.
  dnl That helps on the affected systems, and doesn't hurt anywhere.
  AC_DEFINE([_DARWIN_USE_64_BIT_INODE], [1],
    [Define if you want to use large inode numbers
     when running on Mac OS X 10.5 or later.])
])
