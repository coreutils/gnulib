# selinux-context-h.m4
# serial 4   -*- Autoconf -*-
dnl Copyright (C) 2006-2007, 2009-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

# From Jim Meyering
# Provide <selinux/context.h>, if necessary.

AC_DEFUN([gl_HEADERS_SELINUX_CONTEXT_H],
[
  AC_REQUIRE([gl_LIBSELINUX])
  if test "$with_selinux" != no; then
    AC_CHECK_HEADERS([selinux/context.h],
                     [GL_GENERATE_SELINUX_CONTEXT_H=false],
                     [GL_GENERATE_SELINUX_CONTEXT_H=true])
  else
    GL_GENERATE_SELINUX_CONTEXT_H=true
  fi
])
