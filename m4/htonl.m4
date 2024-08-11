# htonl.m4
# serial 1
dnl Copyright (C) 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Collin Funk.

AC_DEFUN([gl_FUNC_HTONL],
[
  AC_REQUIRE([gl_ARPA_INET_H_DEFAULTS])

  if test $ac_cv_header_arpa_inet_h = yes; then
    AC_CACHE_CHECK([if arpa/inet.h defines htonl, htons, ntohl, ntohs],
      [gl_cv_func_htonl_working],
      [gl_cv_func_htonl_working=no
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
[[
#include <arpa/inet.h>
]],
[[
/* Host to network.  */
int network_1 = htons (0.0);
int network_2 = htonl (0.0);

/* Network to host.  */
int host_1 = ntohs (0.0);
int host_2 = ntohl (0.0);

/* Make sure the variables get used.  */
return !(network_1 + network_2 + host_1 + host_2);
]])],
         [gl_cv_func_htonl_working=yes],
         [gl_cv_func_htonl_working=no])
      ])
    if test $gl_cv_func_htonl_working = no; then
      REPLACE_HTONL=1
    fi
  fi
])
