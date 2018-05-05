# af_alg.m4 serial 1
dnl Check whether linux/if_alg.h has needed features.

dnl Copyright 2018 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Matteo Croce.

AC_DEFUN_ONCE([gl_AF_ALG],
[
  AC_REQUIRE([gl_HEADER_SYS_SOCKET])
  AC_CACHE_CHECK([whether linux/if_alg.h has struct sockaddr_alg.],
    [gl_cv_header_linux_if_alg_salg],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[#include <sys/socket.h>
                          #include <linux/if_alg.h>
                          struct sockaddr_alg salg = {
                            .salg_family = AF_ALG,
                            .salg_type = "hash",
                            .salg_name = "sha1",
                          };]])],
       [gl_cv_header_linux_if_alg_salg=yes],
       [gl_cv_header_linux_if_alg_salg=no])])
  if test "$gl_cv_header_linux_if_alg_salg" = yes; then
    AC_DEFINE([HAVE_LINUX_IF_ALG_H], [1],
      [Define to 1 if you have 'struct sockaddr_alg' defined.])
  fi
])
