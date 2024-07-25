# sys_un_h.m4
# serial 1
dnl Copyright 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN_ONCE([gl_SYS_UN_H],
[
  dnl Check if UNIX domain sockets are supported.
  AC_REQUIRE([gl_SOCKET_FAMILY_UNIX])

  GL_GENERATE_SYS_UN_H=false

  if test $gl_cv_socket_unix = yes; then

    dnl Check if using a Windows version that supports AF_UNIX.
    dnl See <https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/>.
    if test "$ac_cv_header_winsock2_h" = yes \
       && test "$ac_cv_header_afunix_h" = yes; then
      GL_GENERATE_SYS_UN_H=true
    fi
  fi

  dnl Checked in gl_SOCKET_FAMILY_UNIX.
  if test "$ac_cv_header_afunix_h" = yes; then
    HAVE_AFUNIX_H=1
  else
    HAVE_AFUNIX_H=0
  fi
  AC_SUBST([HAVE_AFUNIX_H])
])
