# write-any-file.m4
# serial 2
dnl Copyright (C) 2007, 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# Determine whether we can write any file.

# Written by Paul Eggert.

AC_DEFUN([gl_WRITE_ANY_FILE],
[
  AC_CHECK_FUNCS_ONCE([geteuid])
])
