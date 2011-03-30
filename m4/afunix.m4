# afunix.m4 serial 6
dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_SOCKET_AFUNIX],
[
  AC_REQUIRE([gl_HEADER_SYS_SOCKET])
  AC_REQUIRE([gl_SOCKET_FAMILY_UNIX])

  AC_MSG_CHECKING([for UNIX domain sockets SCM_RIGHTS])
  AC_CACHE_VAL([gl_cv_socket_unix_scm_rights],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <sys/types.h>
            #ifdef HAVE_SYS_SOCKET_H
            #include <sys/socket.h>
            #endif
            #ifdef HAVE_SYS_UN_H
            #include <sys/un.h>
            #endif
            #ifdef HAVE_WINSOCK2_H
            #include <winsock2.h>
            #endif
          ]],
          [[struct cmsghdr cmh;
            cmh.cmsg_level = SOL_SOCKET;
            cmh.cmsg_type = SCM_RIGHTS;
            if (&cmh) return 0;
          ]])],
       [gl_cv_socket_unix_scm_rights=yes],
       [gl_cv_socket_unix_scm_rights=no])
    ])
  AC_MSG_RESULT([$gl_cv_socket_unix_scm_rights])
  if test $gl_cv_socket_unix_scm_rights = yes; then
    AC_DEFINE([HAVE_UNIXSOCKET_SCM_RIGHTS], [1],
      [Define to 1 if <sys/socket.h> defines SCM_RIGHTS.])
  fi

  dnl Persuade AIX 5.2 <sys/socket.h> to declare CMSG_SPACE, CMSG_LEN.
  AC_DEFINE([_LINUX_SOURCE_COMPAT], [1],
    [Define in order to get some macros on AIX systems.])

  AC_MSG_CHECKING([for UNIX domain sockets SCM_RIGHTS that behave in BSD4.4 way])
  AC_CACHE_VAL([gl_cv_socket_unix_scm_rights_bsd44_way],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <sys/types.h>
            #include <stddef.h>
            #ifdef HAVE_SYS_SOCKET_H
            #include <sys/socket.h>
            #endif
            #ifdef HAVE_SYS_UN_H
            #include <sys/un.h>
            #endif
            #ifdef HAVE_WINSOCK2_H
            #include <winsock2.h>
            #endif
            ]],
            [[struct msghdr msg = {0};
              struct cmsghdr *cmsg;
              int myfds[1] = {0};
              char buf[CMSG_SPACE (sizeof (myfds))];

              msg.msg_control = buf;
              msg.msg_controllen = sizeof buf;
              cmsg = CMSG_FIRSTHDR (&msg);
              cmsg->cmsg_level = SOL_SOCKET;
              cmsg->cmsg_type = SCM_RIGHTS;
              cmsg->cmsg_len = CMSG_LEN (sizeof (int));
              /* fake Initialize the payload: */
              (void) CMSG_DATA (cmsg);
              /* Sum of the length of all control messages in the buffer: */
              msg.msg_controllen = cmsg->cmsg_len;
              return 0;
            ]])],
       [gl_cv_socket_unix_scm_rights_bsd44_way=yes],
       [gl_cv_socket_unix_scm_rights_bsd44_way=no])
    ])
  AC_MSG_RESULT([$gl_cv_socket_unix_scm_rights_bsd44_way])
  if test $gl_cv_socket_unix_scm_rights_bsd44_way = yes; then
    AC_DEFINE([HAVE_UNIXSOCKET_SCM_RIGHTS_BSD44_WAY], [1],
      [Define to 1 if fd can be sent/received in the BSD4.4 way.])
  fi

  AC_MSG_CHECKING([for UNIX domain sockets SCM_RIGHTS that behave in BSD4.3 way])
  AC_CACHE_VAL([gl_cv_socket_unix_scm_rights_bsd43_way],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <sys/types.h>
            #ifdef HAVE_SYS_SOCKET_H
            #include <sys/socket.h>
            #endif
            #ifdef HAVE_SYS_UN_H
            #include <sys/un.h>
            #endif
            #ifdef HAVE_WINSOCK2_H
            #include <winsock2.h>
            #endif
            ]],
            [[struct msghdr msg;
              int fd = 0;
              msg.msg_accrights = &fd;
              msg.msg_accrightslen = sizeof (fd);
              if (&msg) return 0;
            ]])],
       [gl_cv_socket_unix_scm_rights_bsd43_way=yes],
       [gl_cv_socket_unix_scm_rights_bsd43_way=no])
    ])
  AC_MSG_RESULT([$gl_cv_socket_unix_scm_rights_bsd43_way])
  if test $gl_cv_socket_unix_scm_rights_bsd43_way = yes; then
    AC_DEFINE([HAVE_UNIXSOCKET_SCM_RIGHTS_BSD43_WAY], [1],
      [Define to 1 if fd can be sent/received in the BSD4.3 way.])
  fi
])
