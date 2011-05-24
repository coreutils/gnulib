# strerror_r.m4 serial 9
dnl Copyright (C) 2002, 2007-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_STRERROR_R],
[
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  AC_REQUIRE([gl_HEADER_ERRNO_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade Solaris <string.h> to declare strerror_r().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  dnl Some systems don't declare strerror_r() if _THREAD_SAFE and _REENTRANT
  dnl are not defined.
  AC_CHECK_DECLS_ONCE([strerror_r])
  if test $ac_cv_have_decl_strerror_r = no; then
    HAVE_DECL_STRERROR_R=0
  fi

  AC_CHECK_FUNCS([strerror_r])
  if test $ac_cv_func_strerror_r = yes; then
    if test -z "$ERRNO_H"; then
      dnl The POSIX prototype is:  int strerror_r (int, char *, size_t);
      dnl glibc, Cygwin:           char *strerror_r (int, char *, size_t);
      dnl AIX 5.1, OSF/1 5.1:      int strerror_r (int, char *, int);
      AC_CACHE_CHECK([for strerror_r with POSIX signature],
        [gl_cv_func_strerror_r_posix_signature],
        [AC_COMPILE_IFELSE(
           [AC_LANG_PROGRAM(
              [[#include <string.h>
                int strerror_r (int, char *, size_t);
              ]],
              [])],
           [gl_cv_func_strerror_r_posix_signature=yes],
           [gl_cv_func_strerror_r_posix_signature=no])
        ])
      if test $gl_cv_func_strerror_r_posix_signature = yes; then
        dnl AIX 6.1 strerror_r fails by returning -1, not an error number.
        dnl HP-UX 11.31 strerror_r always fails when the buffer length argument
        dnl is less than 80.
        dnl FreeBSD 8.s strerror_r claims failure on 0
        dnl Solaris 10 strerror_r corrupts errno on failure
        AC_CACHE_CHECK([whether strerror_r works],
          [gl_cv_func_strerror_r_works],
          [AC_RUN_IFELSE(
             [AC_LANG_PROGRAM(
                [[#include <errno.h>
                  #include <string.h>
                ]],
                [[int result = 0;
                  char buf[79];
                  if (strerror_r (EACCES, buf, 0) < 0)
                    result |= 1;
                  errno = 0;
                  if (strerror_r (EACCES, buf, sizeof buf) != 0)
                    result |= 2;
                  if (strerror_r (0, buf, sizeof buf) != 0)
                    result |= 4;
                  if (errno)
                    result |= 8;
                  errno = 0;
                  if (strerror_r (-3, buf, sizeof buf) != 0)
                    result |= 0x10;
                  if (errno)
                    result |= 0x20;
                  return result;
                ]])],
             [gl_cv_func_strerror_r_works=yes],
             [gl_cv_func_strerror_r_works=no],
             [
changequote(,)dnl
              case "$host_os" in
                       # Guess no on AIX.
                aix*)  gl_cv_func_strerror_r_works="guessing no";;
                       # Guess no on HP-UX.
                hpux*) gl_cv_func_strerror_r_works="guessing no";;
                       # Guess no on FreeBSD.
                freebsd*)  gl_cv_func_strerror_r_works="guessing no";;
                       # Guess yes otherwise.
                *)     gl_cv_func_strerror_r_works="guessing yes";;
              esac
changequote([,])dnl
             ])
          ])
        case "$gl_cv_func_strerror_r_works" in
          *no) REPLACE_STRERROR_R=1 ;;
        esac
      else
        dnl The system's strerror() has a wrong signature. Replace it.
        REPLACE_STRERROR_R=1
        dnl glibc >= 2.3.4 and cygwin 1.7.9 have a function __xpg_strerror_r.
        AC_CHECK_FUNCS([__xpg_strerror_r])
      fi
    else
      dnl The system's strerror_r() cannot know about the new errno values we
      dnl add to <errno.h>. Replace it.
      REPLACE_STRERROR_R=1
    fi
  fi
])

# Prerequisites of lib/strerror_r.c.
AC_DEFUN([gl_PREREQ_STRERROR_R], [
  AC_CHECK_FUNCS_ONCE([catgets])
  :
])
