# gnu-make.m4
# serial 1
dnl Copyright (C) 2007, 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Determine whether recent-enough GNU Make is being used.

# Written by Paul Eggert.

# Set GNU_MAKE if we are using a recent-enough version of GNU make.

AC_DEFUN([gl_GNU_MAKE],
[
  AM_CONDITIONAL([GNU_MAKE],
    [LC_ALL=C ${MAKE-make} --version 2>/dev/null | sed -e '2,$d' | grep GNU >/dev/null])
])
