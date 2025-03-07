# net_if_h.m4
# serial 2
dnl Copyright (C) 2006-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_HEADER_NET_IF],
[
  AC_CACHE_CHECK([whether <net/if.h> is self-contained],
    [gl_cv_header_net_if_h_selfcontained],
    [
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <net/if.h>]],
                                         [[struct if_nameindex ni;]])],
        [gl_cv_header_net_if_h_selfcontained=yes],
        [gl_cv_header_net_if_h_selfcontained=no])
    ])
  if test $gl_cv_header_net_if_h_selfcontained = yes; then
    GL_GENERATE_NET_IF_H=false
  else
    GL_GENERATE_NET_IF_H=true
    AC_CHECK_HEADERS([net/if.h], [], [], [[#include <sys/socket.h>]])
    gl_NEXT_HEADERS([net/if.h])
    if test $ac_cv_header_net_if_h = yes; then
      HAVE_NET_IF_H=1
    else
      HAVE_NET_IF_H=0
    fi
    AC_SUBST([HAVE_NET_IF_H])
  fi
])
