# sys_select_h.m4 serial 8
dnl Copyright (C) 2006-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_HEADER_SYS_SELECT],
[
  AC_REQUIRE([gl_HEADER_SYS_SOCKET])
  AC_REQUIRE([gl_SYS_SELECT_H_DEFAULTS])
  AC_CACHE_CHECK([whether <sys/select.h> is self-contained],
    [gl_cv_header_sys_select_h_selfcontained],
    [
      dnl Test against two bugs:
      dnl 1. On many platforms, <sys/select.h> assumes prior inclusion of
      dnl    <sys/types.h>.
      dnl 2. On OSF/1 4.0, <sys/select.h> provides only a forward declaration
      dnl    of 'struct timeval', and no definition of this type.
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/select.h>]],
					 [[struct timeval b;]])],
        [gl_cv_header_sys_select_h_selfcontained=yes],
        [gl_cv_header_sys_select_h_selfcontained=no])
      dnl Test against another bug:
      dnl 3. On Solaris 10, <sys/select.h> provides an FD_ZERO implementation
      dnl    that relies on memset(), but without including <string.h>.
      if test $gl_cv_header_sys_select_h_selfcontained = yes; then
        AC_COMPILE_IFELSE(
          [AC_LANG_PROGRAM([[#include <sys/select.h>]],
                           [[int memset; int bzero;]])
          ],
          [AC_LINK_IFELSE(
             [AC_LANG_PROGRAM([[#include <sys/select.h>]], [[
                  #undef memset
                  #define memset nonexistent_memset
                  extern void *memset (void *, int, unsigned long);
                  #undef bzero
                  #define bzero nonexistent_bzero
                  extern void bzero (void *, unsigned long);
                  fd_set fds;
                  FD_ZERO (&fds);
                ]])
             ],
             [],
             [gl_cv_header_sys_select_h_selfcontained=no])
          ])
      fi
    ])
  if test $gl_cv_header_sys_select_h_selfcontained = yes; then
    SYS_SELECT_H=''
  else
    SYS_SELECT_H='sys/select.h'
    gl_CHECK_NEXT_HEADERS([sys/select.h])
    if test $ac_cv_header_sys_select_h = yes; then
      HAVE_SYS_SELECT_H=1
    else
      HAVE_SYS_SELECT_H=0
    fi
    AC_SUBST([HAVE_SYS_SELECT_H])
    gl_PREREQ_SYS_H_WINSOCK2
  fi
  AC_SUBST([SYS_SELECT_H])
])

AC_DEFUN([gl_SYS_SELECT_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_SYS_SELECT_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_SYS_SELECT_H_DEFAULTS],
[
  GNULIB_SELECT=0; AC_SUBST([GNULIB_SELECT])
  dnl Assume proper GNU behavior unless another module says otherwise.
  REPLACE_SELECT=0; AC_SUBST([REPLACE_SELECT])
])
