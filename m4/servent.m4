# servent.m4
# serial 6
dnl Copyright (C) 2008, 2010-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_SERVENT],
[
  dnl Where are getservent(), setservent(), endservent(), getservbyname(),
  dnl getservbyport() defined?
  dnl Where are getprotoent(), setprotoent(), endprotoent(), getprotobyname(),
  dnl getprotobynumber() defined?
  dnl - On Solaris < 11.4, they are in libsocket. Ignore libxnet.
  dnl - On Haiku, they are in libnetwork.
  dnl - On BeOS, they are in libnet.
  dnl - On native Windows, they are in ws2_32.dll.
  dnl - Otherwise they are in libc.
  AC_REQUIRE([gl_SYS_SOCKET_H])dnl for HAVE_SYS_SOCKET_H, HAVE_WINSOCK2_H
  SERVENT_LIB=
  gl_saved_libs="$LIBS"
  AC_SEARCH_LIBS([getservbyname], [socket network net],
    [if test "$ac_cv_search_getservbyname" != "none required"; then
       SERVENT_LIB="$ac_cv_search_getservbyname"
     fi])
  LIBS="$gl_saved_libs"
  if test -z "$SERVENT_LIB"; then
    AC_CHECK_FUNCS([getservbyname], , [
      AC_CACHE_CHECK([for getservbyname in winsock2.h and -lws2_32],
        [gl_cv_w32_getservbyname],
        [gl_cv_w32_getservbyname=no
         gl_saved_LIBS="$LIBS"
         LIBS="$LIBS -lws2_32"
         AC_LINK_IFELSE(
           [AC_LANG_PROGRAM(
              [[
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#include <stddef.h>
              ]],
              [[getservbyname(NULL,NULL);]])],
           [gl_cv_w32_getservbyname=yes])
         LIBS="$gl_saved_LIBS"
        ])
      if test "$gl_cv_w32_getservbyname" = "yes"; then
        SERVENT_LIB="-lws2_32"
      fi
    ])
  fi
  AC_SUBST([SERVENT_LIB])
  AC_CACHE_CHECK([whether getservbyname and getservbyport work reasonably],
    [gl_cv_func_getservbyport_works],
    [case "$host_os" in
       mingw* | windows*)
         # On native Windows, the functions return NULL if WSAStartup has not
         # been called yet.
         gl_cv_func_getservbyport_works=no
         ;;
       *)
         saved_LIBS="$LIBS"
         LIBS="$LIBS $SERVENT_LIB"
         AC_RUN_IFELSE(
           [AC_LANG_PROGRAM(
              [[
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <string.h>
                static int
                is_aliases_member (const struct servent *se, const char *name)
                {
                  int i;
                  for (i = 0; se->s_aliases[i] != NULL; i++)
                    if (strcmp (se->s_aliases[i], name) == 0)
                      return 1;
                  return 0;
                }
              ]],
              [[int result = 0;
                /* This test fails on musl libc.  */
                {
                  struct servent *se = getservbyport (htons (80), "tcp");
                  if (!(se != NULL
                        && se->s_port == htons (80)
                        && strcmp (se->s_proto, "tcp") == 0
                        && ((strcmp (se->s_name, "http") == 0
                             && is_aliases_member (se, "www"))
                            || (strcmp (se->s_name, "www") == 0
                                && is_aliases_member (se, "http")))))
                    result |= 1;
                }
                return result;
              ]])],
           [gl_cv_func_getservbyport_works=yes],
           [gl_cv_func_getservbyport_works=no],
           [case "$host_os" in
                                 # Guess no on musl.
              linux* | midipix*) gl_cv_func_getservbyport_works="guessing no" ;;
                                 # If we don't know, obey --enable-cross-guesses.
              *)                 gl_cv_func_getservbyport_works="$gl_cross_guess_normal" ;;
            esac
           ])
         LIBS="$saved_LIBS"
         ;;
     esac
    ])
  case "$gl_cv_func_getservbyport_works" in
    *yes) ;;
    *) REPLACE_GETSERVBYNAME=1; REPLACE_GETSERVBYPORT=1 ;;
  esac
])
