# sockets.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SOCKETS],
[
  AC_REQUIRE([gl_HEADER_SYS_SOCKET])dnl for HAVE_SYS_SOCKET_H, HAVE_WINSOCK2_H

  AC_CACHE_CHECK([if we need to call WSAStartup in winsock2.h and -lws2_32],
                 [gl_cv_func_wsastartup], [
    am_save_LIBS="$LIBS"
    LIBS="$LIBS -lws2_32"
    AC_TRY_LINK([
#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#endif], [
      WORD wVersionRequested = MAKEWORD(1, 1);
      WSADATA wsaData;
      int err = WSAStartup(wVersionRequested, &wsaData);
      WSACleanup ();],
      gl_cv_func_wsastartup=yes, gl_cv_func_wsastartup=no)
    LIBS="$am_save_LIBS"])
  if test "$gl_cv_func_wsastartup" = "yes"; then
    AC_DEFINE([WINDOWS_SOCKETS], 1, [Define if WSAStartup is needed.])
    LIBS="$LIBS -lws2_32"
  fi
  gl_PREREQ_SOCKETS
])

# Prerequisites of lib/sockets.c.
AC_DEFUN([gl_PREREQ_SOCKETS], [
  :
])
