# abort-debug.m4
# serial 1
dnl Copyright (C) 2024 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_ABORT_DEBUG_EARLY],
[
  AC_MSG_CHECKING([whether to enable debugging facilities on abort])
  AC_ARG_ENABLE([debug-abort],
    [AS_HELP_STRING([[--disable-debug-abort]],
       [turn off debugging facilities])],
    [case "$enableval" in
       yes | no) ;;
       *) AC_MSG_WARN([invalid argument supplied to --enable-debug-abort])
          enable_debug_abort=yes
          ;;
     esac
    ],
    [enable_debug_abort=yes])
  AC_MSG_RESULT([$enable_debug_abort])

  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  if test $enable_debug_abort = yes; then
    AC_REQUIRE([AC_CANONICAL_HOST])
    case "$host_os" in
      *-gnu* | gnu* | darwin* | freebsd* | dragonfly* | netbsd* | openbsd* | solaris*)
        dnl execinfo might be implemented on this platform.
        REPLACE_ABORT=1
        dnl On *BSD system, link all programs with -lexecinfo. Cf. m4/execinfo.m4.
        case "$host_os" in
          freebsd* | dragonfly* | netbsd* | openbsd*)
            LDFLAGS="$LDFLAGS -lexecinfo"
            ;;
        esac
        dnl Link all programs in such a way that the stack trace includes the
        dnl function names. '-rdynamic' is equivalent to '-Wl,-export-dynamic'.
        case "$host_os" in
          *-gnu* | gnu* | openbsd*)
            LDFLAGS="$LDFLAGS -rdynamic"
            ;;
        esac
        ;;
    esac
  fi
])

AC_DEFUN([gl_ABORT_DEBUG],
[
  AC_REQUIRE([AC_C_INLINE])
])
