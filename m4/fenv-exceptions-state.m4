# fenv-exceptions-state.m4
# serial 5
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FENV_EXCEPTIONS_STATE],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  gl_MATHFUNC([fesetexceptflag], [int], [(fexcept_t const *, int)],
    [#include <fenv.h>
     fexcept_t fx_ret;
    ])
  if test $gl_cv_func_fesetexceptflag_no_libm = yes \
     || test $gl_cv_func_fesetexceptflag_in_libm = yes; then
    dnl It needs linking with -lm on
    dnl glibc, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Android.
    if test $gl_cv_func_fesetexceptflag_no_libm = yes; then
      FENV_EXCEPTIONS_STATE_LIBM=
    else
      FENV_EXCEPTIONS_STATE_LIBM=-lm
    fi
    dnl On glibc 2.19/s390,s390x, fegetexceptflag is not consistent with the
    dnl generic implementation of fetestexceptflag, leading to a failure of
    dnl test-fenv-except-state-3. Fixed through
    dnl <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=5d96fe8c0dc3450bafe6c2aae2dabc76819df3e0>.
    case "$host" in
      s390*-*-linux*)
        AC_CACHE_CHECK([whether fegetexceptflag works],
          [gl_cv_func_fegetexceptflag_works],
          [AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <string.h> /* for __GNU_LIBRARY__ */
                #ifdef __GNU_LIBRARY__
                 #include <features.h>
                 #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 22
                  Unlucky user
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_fegetexceptflag_works="guessing yes"],
             [gl_cv_func_fegetexceptflag_works="guessing no"])
          ])
        case "$gl_cv_func_fegetexceptflag_works" in
          *yes) ;;
          *)
            REPLACE_FEGETEXCEPTFLAG=1
            REPLACE_FESETEXCEPTFLAG=1
            ;;
        esac
        ;;
    esac
    if test $REPLACE_FESETEXCEPTFLAG = 0; then
      dnl Persuade glibc <fenv.h> to declare feenableexcept().
      AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
      gl_MATHFUNC([feenableexcept], [int], [(int)], [#include <fenv.h>])
      dnl On glibc 2.37 for PowerPC, i386, x86_64, fesetexceptflag may raise
      dnl traps. Likewise on Mac OS X 10.5.8 on i386, x86_64, on mingw, and
      dnl on Haiku on i386, x86_64.
      AC_CACHE_CHECK([whether fesetexceptflag is guaranteed non-trapping],
        [gl_cv_func_fesetexceptflag_works1],
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
                [[fexcept_t saved_flags;
                  if (feraiseexcept (FE_INVALID) == 0
                      && fegetexceptflag (&saved_flags, FE_INVALID) == 0
                      && feclearexcept (FE_INVALID) == 0
                      && feenableexcept (FE_INVALID) == 0
                      && fesetexceptflag (&saved_flags, FE_INVALID) == 0)
                    {
                      a = 1.0; b = a + a;
                      al = 1.0L; bl = al + al;
                    }
                  return 0;
                ]])
             ],
             [gl_cv_func_fesetexceptflag_works1=yes],
             [dnl On OpenBSD 7.4/mips64 this test fails because feclearexcept
              dnl is buggy, not because of fesetexceptflag.
              case "$host" in
                mips*-*-openbsd*)
                  gl_cv_func_fesetexceptflag_works1="guessing yes"
                  ;;
                *)
                  gl_cv_func_fesetexceptflag_works1=no
                  ;;
              esac
             ],
             [case "$host_os" in
                # Guess yes or no on glibc systems, depending on CPU.
                *-gnu*)
                  case "$host_cpu" in
changequote(,)dnl
                    powerpc* | i[34567]86 | x86_64)
changequote([,])dnl
                      gl_cv_func_fesetexceptflag_works1="guessing no" ;;
                    *)
                      gl_cv_func_fesetexceptflag_works1="guessing yes" ;;
                  esac
                  ;;
                # If we don't know, obey --enable-cross-guesses.
                *) gl_cv_func_fesetexceptflag_works1="$gl_cross_guess_normal" ;;
              esac
             ])
           LIBS="$saved_LIBS"
         else
           case "$host_os" in
             # Guess no on musl libc.
             # In musl/src/fenv/fesetexceptflag.c, fesetexceptflag() explicitly
             # invokes feraiseexcept(). It's mentioned in
             # <https://wiki.musl-libc.org/functional-differences-from-glibc.html>.
             *-musl* | midipix*)
               gl_cv_func_fesetexceptflag_works1="guessing no"
               ;;
             # Guess no on macOS.
             darwin*)
               gl_cv_func_fesetexceptflag_works1="guessing no"
               ;;
             # Guess no on mingw.
             mingw* | windows*)
               AC_EGREP_CPP([Problem], [
#ifdef __MINGW32__
 Problem
#endif
                 ],
                 [gl_cv_func_fesetexceptflag_works1="guessing no"],
                 [gl_cv_func_fesetexceptflag_works1="guessing yes"])
               ;;
             # Guess no on Haiku.
             haiku*)
               gl_cv_func_fesetexceptflag_works1="guessing no"
               ;;
             *) gl_cv_func_fesetexceptflag_works1="guessing yes" ;;
           esac
         fi
        ])
      dnl On glibc 2.37 for alpha, fesetexceptflag clears too many flag bits.
      AC_CACHE_CHECK([whether fesetexceptflag obeys its arguments],
        [gl_cv_func_fesetexceptflag_works2],
        [saved_LIBS="$LIBS"
         LIBS="$LIBS $FENV_EXCEPTIONS_STATE_LIBM"
         AC_RUN_IFELSE(
           [AC_LANG_PROGRAM([[
              #include <fenv.h>
              ]],
              [[fexcept_t f1, f2;
                if (feraiseexcept (FE_DIVBYZERO | FE_OVERFLOW | FE_INEXACT) == 0
                    && fegetexceptflag (&f1, FE_DIVBYZERO | FE_OVERFLOW | FE_INEXACT) == 0
                    && feclearexcept (FE_OVERFLOW | FE_INEXACT) == 0
                    && fegetexceptflag (&f2, FE_OVERFLOW) == 0
                    && fesetexceptflag (&f1, FE_DIVBYZERO | FE_OVERFLOW) == 0
                    && fesetexceptflag (&f2, FE_OVERFLOW) == 0
                    && fetestexcept (FE_DIVBYZERO) != FE_DIVBYZERO)
                  return 1;
                return 0;
              ]])
           ],
           [gl_cv_func_fesetexceptflag_works2=yes],
           [gl_cv_func_fesetexceptflag_works2=no],
           [case "$host_os" in
              # Guess yes or no on glibc systems, depending on CPU.
              *-gnu*)
                case "$host_cpu" in
                  alpha*)
                    gl_cv_func_fesetexceptflag_works2="guessing no" ;;
                  *)
                    gl_cv_func_fesetexceptflag_works2="guessing yes" ;;
                esac
                ;;
              # If we don't know, obey --enable-cross-guesses.
              *) gl_cv_func_fesetexceptflag_works2="$gl_cross_guess_normal" ;;
            esac
           ])
         LIBS="$saved_LIBS"
        ])
      case "$gl_cv_func_fesetexceptflag_works1" in
        *yes) ;;
        *) REPLACE_FESETEXCEPTFLAG=1 ;;
      esac
      case "$gl_cv_func_fesetexceptflag_works2" in
        *yes) ;;
        *) REPLACE_FESETEXCEPTFLAG=1 ;;
      esac
      dnl Additionally, on FreeBSD/powerpc64 and NetBSD/powerpc, the unit test
      dnl test-fenv-except-state-2 fails if we don't override fesetexceptflag.
      dnl The function fesetexceptflag apparently fails to restore the
      dnl FE_INVALID flag.
      case "$host" in
        powerpc*-*-freebsd* | powerpc*-*-netbsd*)
          REPLACE_FESETEXCEPTFLAG=1
          ;;
      esac
      dnl Additionally, on AIX, the unit test test-fenv-except-state-1 fails
      dnl if we don't override fesetexceptflag.
      case "$host" in
        powerpc*-*-aix*)
          REPLACE_FESETEXCEPTFLAG=1
          ;;
      esac
      dnl Additionally, on MSVC, we want the bits in the saved state to be
      dnl identified by the FE_* macros, so that the fetestexceptflag function
      dnl can be implemented like on other platforms. This requires conversions
      dnl (exceptions_to_x86hardware, x86hardware_to_exceptions) in both
      dnl of the fegetexceptflag, fesetexceptflag functions.
      case "$host_os" in
        mingw* | windows*)
          AC_EGREP_CPP([Problem], [
#ifndef __MINGW32__
 Problem
#endif
            ],
            [REPLACE_FEGETEXCEPTFLAG=1; REPLACE_FESETEXCEPTFLAG=1],
            [])
          ;;
      esac
    fi
  else
    HAVE_FEGETEXCEPTFLAG=0
    HAVE_FESETEXCEPTFLAG=0
    FENV_EXCEPTIONS_STATE_LIBM=
  fi
  if { test $HAVE_FEGETEXCEPTFLAG = 0 || test $REPLACE_FEGETEXCEPTFLAG = 1; } \
     || { test $HAVE_FESETEXCEPTFLAG = 0 || test $REPLACE_FESETEXCEPTFLAG = 1; }; then
    gl_PREREQ_FENV_EXCEPTIONS
    dnl Possibly need -lm for fpgetsticky(), fpsetsticky().
    if test $gl_cv_func_fpsetsticky_no_libm = no \
       && test $gl_cv_func_fpsetsticky_in_libm = yes \
       && test -z "$FENV_EXCEPTIONS_STATE_LIBM"; then
      FENV_EXCEPTIONS_STATE_LIBM=-lm
    fi
  fi
  AC_SUBST([FENV_EXCEPTIONS_STATE_LIBM])
])
