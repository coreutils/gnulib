# sys_un_h.m4
# serial 3
dnl Copyright 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN_ONCE([gl_SYS_UN_H],
[
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl Check if UNIX domain sockets are supported.
  AC_REQUIRE([gl_SOCKET_FAMILY_UNIX])

  gl_PREREQ_SYS_SA_FAMILY
  AC_CACHE_CHECK([whether <sys/un.h> defines sa_family_t],
    [gl_cv_type_sys_un_sa_family_t],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[
          #include <stddef.h>
          #include <string.h>
          #include <sys/un.h>
          ]], [[
          sa_family_t f;
          ]])
       ],
       [gl_cv_type_sys_un_sa_family_t=yes],
       [gl_cv_type_sys_un_sa_family_t=no])
    ])
  if test $gl_cv_type_sys_un_sa_family_t = yes; then
    HAVE_SA_FAMILY_T_IN_SYS_UN_H=1
  else
    HAVE_SA_FAMILY_T_IN_SYS_UN_H=0
  fi
  AC_SUBST([HAVE_SA_FAMILY_T_IN_SYS_UN_H])

  GL_GENERATE_SYS_UN_H=false
  if test $gl_cv_socket_unix = yes; then
    dnl Check if using a Windows version that supports AF_UNIX.
    dnl See <https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/>.
    if test "$ac_cv_header_winsock2_h" = yes \
       && test "$ac_cv_header_afunix_h" = yes; then
      GL_GENERATE_SYS_UN_H=true
    fi
    case "$host_os" in
      dnl On glibc systems, we need to include <string.h> before <sys/un.h>,
      dnl at least in C++ mode with clang.
      *-gnu* | gnu*) GL_GENERATE_SYS_UN_H=true ;;
    esac
  fi
  if test $HAVE_SA_FAMILY_T_IN_SYS_UN_H = 0; then
    GL_GENERATE_SYS_UN_H=true
  fi

  if $GL_GENERATE_SYS_UN_H; then
    AC_CHECK_HEADERS([sys/un.h])
    gl_CHECK_NEXT_HEADERS([sys/un.h])

    if test $ac_cv_header_sys_un_h = yes; then
      HAVE_SYS_UN_H=1
    else
      HAVE_SYS_UN_H=0
    fi
    AC_SUBST([HAVE_SYS_UN_H])

    dnl Checked in gl_SOCKET_FAMILY_UNIX.
    if test "$ac_cv_header_afunix_h" = yes; then
      HAVE_AFUNIX_H=1
    else
      HAVE_AFUNIX_H=0
    fi
    AC_SUBST([HAVE_AFUNIX_H])
  fi
])
