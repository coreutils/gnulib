# warnings.m4 serial 1
dnl Copyright (C) 2008 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Simon Josefsson

# gl_WARN_INIT
# Initializes WARN_CFLAGS variable.
AC_DEFUN([gl_WARN_INIT],
[
  AC_ARG_VAR(WARN_CFLAGS, [C compiler warning flags])
])

# gl_WARN_ADD([parameter]) adds parameter to WARN_CFLAGS if compiler
# supports it.  For example, use gl_WARN_ADD([-Werror]).
AC_DEFUN([gl_WARN_ADD],
[
  pushdef([param],[translit([$1],[ABCDEFGHIJKLMNOPQRSTUVWXYZ./-],
                                 [abcdefghijklmnopqrstuvwxyz___])])

  AC_CACHE_CHECK([whether compiler handles $1], [gl_cv_warn[]param[]], [
    save_CFLAGS="$CFLAGS"
    CFLAGS="${CFLAGS} $1"
    AC_PREPROC_IFELSE([AC_LANG_PROGRAM([])],
                       gl_cv_warn[]param=yes, gl_cv_warn[]param=no)
    CFLAGS="$save_CFLAGS"
  ])

  if test $gl_cv_warn[]param = "yes"; then
    WARN_CFLAGS="$WARN_CFLAGS $1"
  fi
])
