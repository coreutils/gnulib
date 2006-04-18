# full-header-path.m4 serial 1
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Derek Price.

# gl_FULL_HEADER_PATH(HEADER1 HEADER2 ...)
# ----------------------------------------
# Find the full path to a header file, when the #include_next directive
# doesn't work and the header exists in the first place.  If the header were
# sys/inttypes.h (and it existed and #include_next didn't work), this macro
# would define FULL_PATH_SYS_INTTYPES_H to the quoted full path to
# sys/inttypes.h in config.h
# (e.g. `#define FULL_PATH_SYS_INTTYPES_H "/usr/include/sys/inttypes.h"').
AC_DEFUN([gl_FULL_HEADER_PATH],
[AC_LANG_PREPROC_REQUIRE()dnl
AC_REQUIRE([gl_INCLUDE_NEXT])dnl
if test $gl_cv_have_include_next = no; then
AC_FOREACH([gl_HEADER_NAME], [$1],
  [AS_VAR_PUSHDEF([gl_full_header_path],
                  [gl_cv_full_path_]m4_quote(m4_defn([gl_HEADER_NAME])))dnl
  AC_CACHE_CHECK([full path to <]m4_quote(m4_defn([gl_HEADER_NAME]))[>],
    m4_quote(m4_defn([gl_full_header_path])),
    [AS_VAR_PUSHDEF([ac_header_exists],
                    [ac_cv_header_]m4_quote(m4_defn([gl_HEADER_NAME])))dnl
    AC_CHECK_HEADERS_ONCE(m4_quote(m4_defn([gl_HEADER_NAME])))dnl
    if test AS_VAR_GET(ac_header_exists) = yes; then
      AC_LANG_CONFTEST([AC_LANG_SOURCE([[#include <]]m4_dquote(m4_defn([gl_HEADER_NAME]))[[>]])])
dnl eval is necessary to expand ac_cpp.
dnl Ultrix and Pyramid sh refuse to redirect output of eval, so use subshell.
      AS_VAR_SET(gl_full_header_path,
[`(eval "$ac_cpp conftest.$ac_ext") 2>&AS_MESSAGE_LOG_FD |
sed -n '\#/]m4_quote(m4_defn([gl_HEADER_NAME]))[#{s#.*"\(.*/]m4_quote(m4_defn([gl_HEADER_NAME]))[\)".*#\1#;p;q;}'`])
    fi
    AS_VAR_POPDEF([ac_header_exists])dnl
    ])dnl
  AC_DEFINE_UNQUOTED([FULL_PATH_]m4_quote(translit(m4_defn([gl_HEADER_NAME]),
                                                   [-./abcdefghijklmnopqrstuvwxyz],
                                                   [___ABCDEFGHIJKLMNOPQRSTUVWXYZ])),
                     ["AS_VAR_GET(gl_full_header_path)"],
                     [Define this to the full path to <]m4_quote(m4_defn([gl_HEADER_NAME]))[>.])
  AS_VAR_POPDEF([gl_full_header_path])dnl
])dnl
fi
])# gl_FULL_HEADER_PATH
