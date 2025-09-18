# stack-trace.m4
# serial 5
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_STACK_TRACE_EARLY],
[
  AC_MSG_CHECKING([whether to enable debugging facilities])
  AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([[--disable-debug]],
       [turn off debugging facilities])],
    [case "$enableval" in
       yes | no) ;;
       *) AC_MSG_WARN([invalid argument supplied to --enable-debug])
          enable_debug=yes
          ;;
     esac
    ],
    [enable_debug=yes])
  AC_MSG_RESULT([$enable_debug])

  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  if test $enable_debug = yes; then
    dnl The first choice is libbacktrace by Ian Lance Taylor.
    dnl Maintained at https://github.com/ianlancetaylor/libbacktrace,
    dnl mirrored into GCC, installed as part of GCC by a few distros.
    dnl It produces source file names and line numbers, if the binary
    dnl is compiled with debug information.
    AC_CACHE_CHECK([for libbacktrace], [gl_cv_lib_backtrace], [
      gl_saved_LIBS="$LIBS"
      LIBS="$gl_saved_LIBS -lbacktrace"
      AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
           [[#include <backtrace.h>
           ]],
           [[struct backtrace_state *state =
               backtrace_create_state (NULL, 0, NULL, NULL);
           ]])],
        [gl_cv_lib_backtrace=yes],
        [gl_cv_lib_backtrace=no])
      LIBS="$gl_saved_LIBS"
    ])
    if test $gl_cv_lib_backtrace = yes; then
      AC_DEFINE([HAVE_LIBBACKTRACE], [1],
        [Define if you have the libbacktrace library.])
      CAN_PRINT_STACK_TRACE=1
      LIBS="$LIBS -lbacktrace"
    else
      dnl The second choice is libexecinfo.
      dnl It does not produce source file names and line numbers, only addresses
      dnl (which are mostly useless due to ASLR) and _sometimes_ function names.
      AC_REQUIRE([AC_CANONICAL_HOST])
      case "$host_os" in
        *-gnu* | gnu* | darwin* | freebsd* | dragonfly* | netbsd* | openbsd* | solaris*)
          dnl execinfo might be implemented on this platform.
          CAN_PRINT_STACK_TRACE=1
          dnl On *BSD system, link all programs with -lexecinfo, provided that
          dnl libexecinfo actually exists. Cf. m4/execinfo.m4.
          case "$host_os" in
            freebsd* | dragonfly* | netbsd* | openbsd*)
              AC_SEARCH_LIBS([backtrace_symbols_fd], [execinfo],
                [], [CAN_PRINT_STACK_TRACE=0])
              if test $CAN_PRINT_STACK_TRACE = 1; then
                LIBS="$LIBS -lexecinfo"
              fi
              ;;
          esac
          if test $CAN_PRINT_STACK_TRACE = 1; then
            dnl Link all programs in such a way that the stack trace includes
            dnl the function names.
            dnl gcc supports '-rdynamic' as an option equivalent to
            dnl '-Wl,-export-dynamic'. But cc from Oracle DeveloperStudio 12.6
            dnl converts '-rdynamic' into a linker option '-dynamic', which the
            dnl linker does not understand. So better avoid '-rdynamic'.
            case "$host_os" in
              *-gnu* | gnu* | openbsd*)
                LDFLAGS="$LDFLAGS -Wl,-export-dynamic"
                ;;
            esac
          fi
          ;;
      esac
    fi
  fi
])

AC_DEFUN([gl_STACK_TRACE],
[
  AC_REQUIRE([AC_C_INLINE])
])
