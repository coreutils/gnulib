# unlink-busy.m4
# serial 17
dnl Copyright (C) 2000-2001, 2004, 2007, 2009-2025 Free Software Foundation,
dnl Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl From J. David Anglin.

dnl HPUX and other systems can't unlink shared text that is being executed.

AC_DEFUN([gl_FUNC_UNLINK_BUSY_TEXT],
[
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether a running program can be unlinked],
    [gl_cv_func_unlink_busy_text],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE(
           [AC_INCLUDES_DEFAULT
            GL_MDA_DEFINES[
            int
            main (int argc, char **argv)
            {
              int result = 0;
              if (argc == 0)
                result |= 1;
              else if (unlink (argv[0]) != 0)
                result |= 2;
              return result;
            }]])],
        [gl_cv_func_unlink_busy_text=yes],
        [gl_cv_func_unlink_busy_text=no],
        [case "$host_os" in
                              # Guess no on native Windows.
           mingw* | windows*) gl_cv_func_unlink_busy_text="guessing no" ;;
           *)                 gl_cv_func_unlink_busy_text="guessing no" ;;
         esac
        ]
      )
    ]
  )

  case "$gl_cv_func_unlink_busy_text" in
    *no) INSTALL=$ac_install_sh ;;
  esac
])
