# fenv-exceptions-tracking-c23.m4
# serial 2
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FENV_EXCEPTIONS_TRACKING_C23],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  gl_MATHFUNC([fesetexcept], [int], [(int)], [#include <fenv.h>])
  if test $gl_cv_func_fesetexcept_no_libm != yes \
     && test $gl_cv_func_fesetexcept_in_libm != yes; then
    HAVE_FESETEXCEPT=0
  else
    dnl Persuade glibc <fenv.h> to declare feenableexcept().
    AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
    gl_MATHFUNC([feenableexcept], [int], [(int)], [#include <fenv.h>])
    dnl On glibc 2.37 for PowerPC and i386, fesetexcept may raise traps.
    AC_CACHE_CHECK([whether fesetexcept is guaranteed non-trapping],
      [gl_cv_func_fesetexcept_works],
      [if test $gl_cv_func_feenableexcept_no_libm = yes \
          || test $gl_cv_func_feenableexcept_in_libm = yes; then
         dnl A platform that has feenableexcept.
         saved_LIBS="$LIBS"
         if test $gl_cv_func_feenableexcept_no_libm != yes; then
           LIBS="$LIBS -lm"
         fi
         AC_RUN_IFELSE(
           [AC_LANG_PROGRAM([[
              #include <fenv.h>
              static volatile double a, b;
              static volatile long double al, bl;
              ]],
              [[if (feclearexcept (FE_INVALID) == 0
                    && feenableexcept (FE_INVALID) == 0
                    && fesetexcept (FE_INVALID) == 0)
                  {
                    a = 1.0; b = a + a;
                    al = 1.0L; bl = al + al;
                  }
                return 0;
              ]])
           ],
           [gl_cv_func_fesetexcept_works=yes],
           [gl_cv_func_fesetexcept_works=no],
           [case "$host_os" in
              # Guess yes or no on glibc systems, depending on CPU.
              *-gnu*)
                case "$host_cpu" in
changequote(,)dnl
                  powerpc* | i[34567]86 | x86_64)
changequote([,])dnl
                    gl_cv_func_fesetexcept_works="guessing no" ;;
                  *)
                    gl_cv_func_fesetexcept_works="guessing yes" ;;
                esac
                ;;
              # If we don't know, obey --enable-cross-guesses.
              *) gl_cv_func_fesetexcept_works="$gl_cross_guess_normal" ;;
            esac
           ])
         LIBS="$saved_LIBS"
       else
         gl_cv_func_fesetexcept_works="guessing yes"
       fi
      ])
    case "$gl_cv_func_fesetexcept_works" in
      *yes) ;;
      *) REPLACE_FESETEXCEPT=1 ;;
    esac
  fi

  dnl Modify FENV_EXCEPTIONS_TRACKING_LIBM, set by gl_FENV_EXCEPTIONS_TRACKING.
  AC_REQUIRE([gl_FENV_EXCEPTIONS_TRACKING])
  if test $HAVE_FESETEXCEPT = 0 || test $REPLACE_FESETEXCEPT = 1; then
    gl_PREREQ_FENV_EXCEPTIONS
    dnl Possibly need -lm for fpgetsticky(), fpsetsticky().
    if test $gl_cv_func_fpsetsticky_no_libm = no \
       && test $gl_cv_func_fpsetsticky_in_libm = yes \
       && test -z "$FENV_EXCEPTIONS_TRACKING_LIBM"; then
      FENV_EXCEPTIONS_TRACKING_LIBM=-lm
    fi
  fi
])
