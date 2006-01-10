# sys_socket_h.m4 serial 1
dnl Copyright (C) 2005, 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Simon Josefsson.

AC_DEFUN([gl_HEADER_SYS_SOCKET],
[
  AC_CHECK_HEADERS_ONCE([sys/socket.h])
  if test $ac_cv_header_sys_socket_h = yes; then
    SYS_SOCKET_H=''
  else
    AC_CHECK_HEADERS_ONCE([winsock2.h ws2tcpip.h])
    SYS_SOCKET_H='sys/socket.h'
  fi
  AC_SUBST(SYS_SOCKET_H)
])
