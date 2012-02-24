# Check for stdnoreturn.h that conforms to C11.

dnl Copyright 2012 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Prepare for substituting <stdnoreturn.h> if it is not supported.

AC_DEFUN([gl_STDNORETURN_H],
[
  AC_CACHE_CHECK([for working stdnoreturn.h],
    [gl_cv_header_working_stdnoreturn_h],
    [AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <stdlib.h>
            #include <stdnoreturn.h>
            void noreturn foo1 (void) { exit (0); }
            void _Noreturn foo2 (void) { exit (0); }
            noreturn void foo3 (void) { exit (0); }
            _Noreturn void foo4 (void) { exit (0); }
            int main (int argc, char **argv) {
              if (argc & 1)
                return 0;
              ((argv[0][0]
                ? (argv[0][1] ? foo1 : foo2)
                : (argv[0][1] ? foo3 : foo4))
               ());
            }
          ]])],
       [gl_cv_header_working_stdnoreturn_h=yes],
       [gl_cv_header_working_stdnoreturn_h=no])])

  if test $gl_cv_header_working_stdnoreturn_h = yes; then
    STDNORETURN_H=''
  else
    STDNORETURN_H='stdnoreturn.h'
  fi

  AC_SUBST([STDNORETURN_H])
  AM_CONDITIONAL([GL_GENERATE_STDNORETURN_H], [test -n "$STDNORETURN_H"])
])
