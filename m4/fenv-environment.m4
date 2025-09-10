# fenv-environment.m4
# serial 6
dnl Copyright (C) 2023-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FENV_ENVIRONMENT],
[
  AC_REQUIRE([gl_FENV_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])

  dnl FE_DFL_ENV references an undefined global variable on
  dnl NetBSD 10.0/{hppa,sparc} and Cygwin 2.9.0 (see
  dnl <https://sourceware.org/pipermail/cygwin/2018-July/237888.html>).
  AC_CACHE_CHECK([whether FE_DFL_ENV is usable],
    [gl_cv_var_FE_DFL_ENV_works],
    [case "$host_os" in
       netbsd* | cygwin*)
         saved_LIBS="$LIBS"
         LIBS="$LIBS -lm"
         AC_LINK_IFELSE(
           [AC_LANG_PROGRAM([[
              #include <fenv.h>
              fenv_t volatile env;
              ]],
              [[fenv_t const * volatile fenv_default = FE_DFL_ENV;
                env = *fenv_default;
              ]])
           ],
           [gl_cv_var_FE_DFL_ENV_works=yes],
           [gl_cv_var_FE_DFL_ENV_works=no])
         LIBS="$saved_LIBS"
         ;;
       *)
         gl_cv_var_FE_DFL_ENV_works="guessing yes"
         ;;
     esac
    ])
  case "$gl_cv_var_FE_DFL_ENV_works" in
    *yes)
      AC_DEFINE([HAVE_FE_DFL_ENV], [1],
        [Define to 1 if FE_DFL_ENV from <fenv.h> is usable.])
      ;;
    *)
      dnl If FE_DFL_ENV is not usable, we not only need to redefine it,
      dnl but also override fesetenv and feupdateenv.
      REPLACE_FESETENV=1
      REPLACE_FEUPDATEENV=1
      ;;
  esac
  gl_MATHFUNC([fesetenv], [int], [(fenv_t const *)], [
    #include <fenv.h>
    fenv_t fenv_ret;
    #if HAVE_FE_DFL_ENV
    /* FreeBSD often has fesetenv inline in <fenv.h>, but needs -lm for
       FE_DFL_ENV.  */
    fenv_t const *fenv_default = FE_DFL_ENV;
    #endif
  ])
  if test $gl_cv_func_fesetenv_no_libm = yes \
     || test $gl_cv_func_fesetenv_in_libm = yes; then
    dnl It needs linking with -lm on
    dnl glibc, FreeBSD, NetBSD, OpenBSD, AIX, HP-UX, Solaris, Android.
    if test $gl_cv_func_fesetenv_no_libm = yes; then
      FENV_ENVIRONMENT_LIBM=
    else
      FENV_ENVIRONMENT_LIBM=-lm
    fi
    dnl The feholdexcept function does not work on glibc 2.5 (see
    dnl <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=a8c79c4088e8c04e4297936efa0dee6c8e6e974d>)
    dnl and on FreeBSD 12.2/arm64.
    dnl Also, on musl libc for most CPUs, it does not clear the exception trap
    dnl bits. It's mentioned in
    dnl <https://wiki.musl-libc.org/functional-differences-from-glibc.html>.
    dnl Likewise on mingw: it does not clear the exception trap bits.
    dnl Whereas on MSVC, this function may even fail.
    AC_CACHE_CHECK([whether feholdexcept works],
      [gl_cv_func_feholdexcept_works],
      [case "$host_os" in
         # Guess no on glibc ≤ 2.5.
         *-gnu* | gnu*)
           AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <string.h> /* for __GNU_LIBRARY__ */
                #ifdef __GNU_LIBRARY__
                 #include <features.h>
                 #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 5
                  Unlucky user
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_feholdexcept_works="guessing yes"],
             [gl_cv_func_feholdexcept_works="guessing no"])
           ;;
         *-musl* | midipix*)
           dnl This is only needed when the module 'fenv-exceptions-trapping' is
           dnl in use.
           m4_ifdef([g][l_FENV_EXCEPTIONS_TRAPPING], [
             case "$host_cpu" in
               riscv*)
                 dnl This architecture does not have exception trap bits anyway.
                 gl_cv_func_feholdexcept_works="guessing yes"
                 ;;
               i?86)
                 dnl On this architecture, the feholdexcept override happens to
                 dnl not be needed.
                 gl_cv_func_feholdexcept_works="guessing yes"
                 ;;
               *)
                 gl_cv_func_feholdexcept_works="guessing no"
                 ;;
             esac
           ], [gl_cv_func_feholdexcept_works="guessing yes"])
           ;;
         mingw* | windows*)
           gl_cv_func_feholdexcept_works="guessing no"
           ;;
         *)
           dnl This test fails on FreeBSD 12.2/arm64.
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[fenv_t env;
                  feclearexcept (FE_ALL_EXCEPT);
                  feraiseexcept (FE_INVALID | FE_OVERFLOW | FE_INEXACT);
                  return (feholdexcept (&env) == 0
                          && fetestexcept (FE_ALL_EXCEPT) != 0);
                ]])
             ],
             [gl_cv_func_feholdexcept_works="guessing yes"],
             [gl_cv_func_feholdexcept_works=no],
             [gl_cv_func_feholdexcept_works="guessing yes"])
           LIBS="$saved_LIBS"
           ;;
       esac
      ])
    case "$gl_cv_func_feholdexcept_works" in
      *yes) ;;
      *) REPLACE_FEHOLDEXCEPT=1 ;;
    esac
    dnl The fegetenv function does not work on glibc 2.19/x86_64
    dnl (see <https://sourceware.org/PR16198>)
    dnl and Mac OS X 10.5/{i386,x86_64}.
    AC_CACHE_CHECK([whether fegetenv works],
      [gl_cv_func_fegetenv_works],
      [case "$host" in
         # Guess no on glibc ≤ 2.19 / x86_64.
         x86_64-*-*-gnu* | x86_64-*-gnu*)
           AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <string.h> /* for __GNU_LIBRARY__ */
                #ifdef __GNU_LIBRARY__
                 #include <features.h>
                 #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 19
                  Unlucky user
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_fegetenv_works="guessing yes"],
             [gl_cv_func_fegetenv_works="guessing no"])
           ;;
         # Guess no on Mac OS X/{i386,x86_64}.
         *86*-*-darwin*)
           gl_cv_func_fegetenv_works="guessing no"
           ;;
         *) gl_cv_func_fegetenv_works="guessing yes" ;;
       esac
      ])
    case "$gl_cv_func_fegetenv_works" in
      *yes) ;;
      *) REPLACE_FEGETENV=1 ;;
    esac
    dnl The fesetenv function does not work on FreeBSD 12.2/arm64 (see
    dnl <https://cgit.freebsd.org/src/commit/?id=34cc08e336987a8ebc316595e3f552a4c09f1fd4>),
    dnl on mingw 13 (where fesetenv (FE_DFL_ENV) does not reset the rounding
    dnl direction),
    dnl on musl libc/{i386,x86_64} and AIX and Solaris and MSVC 14 (where it
    dnl fails to restore the exception trap bits),
    dnl on mingw < 13 (where calling it with FE_DFL_ENV argument has no effect
    dnl on the mxcsr register),
    dnl and on NetBSD/m68k.
    AC_CACHE_CHECK([whether fesetenv works],
      [gl_cv_func_fesetenv_works],
      [case "$host" in
         i?86-*-*-musl* | x86_64-*-*-musl*)
           dnl This is only needed when the module 'fenv-exceptions-trapping' is
           dnl in use.
           m4_ifdef([g][l_FENV_EXCEPTIONS_TRAPPING], [
             gl_cv_func_fesetenv_works="guessing no"
           ], [gl_cv_func_fesetenv_works="guessing yes"])
           ;;
         *-*-aix*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                #include <fptrap.h>
                ]],
                [[fenv_t env;
                  feclearexcept (FE_ALL_EXCEPT);
                  if (fegetenv (&env) == 0)
                    {
                      fp_enable (TRP_DIV_BY_ZERO);
                      if (!fp_is_enabled (TRP_DIV_BY_ZERO))
                        return 1;
                      if (fesetenv (&env) == 0
                          && fp_is_enabled (TRP_DIV_BY_ZERO))
                        return 2;
                    }
                  return 0;
                ]])
             ],
             [gl_cv_func_fesetenv_works="guessing yes"],
             [gl_cv_func_fesetenv_works=no],
             [gl_cv_func_fesetenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         *-*-solaris*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                #include <ieeefp.h>
                ]],
                [[fenv_t env1, env2;
                  feclearexcept (FE_ALL_EXCEPT);
                  if (fegetenv (&env1) == 0)
                    {
                      fpsetmask (FP_X_DZ);
                      if (fegetenv (&env2) == 0)
                        {
                          if (fpgetmask () != FP_X_DZ)
                            return 1;
                          if (fesetenv (&env1) == 0)
                            {
                              if (fpgetmask () != 0)
                                return 2;
                              fpsetmask (FP_X_INV);
                              if (fpgetmask () != FP_X_INV)
                                return 3;
                              if (fesetenv (&env2) == 0)
                                {
                                  if (fpgetmask () != FP_X_DZ)
                                    return 4;
                                }
                            }
                        }
                    }
                  return 0;
                ]])
             ],
             [gl_cv_func_fesetenv_works="guessing yes"],
             [gl_cv_func_fesetenv_works=no],
             [gl_cv_func_fesetenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         # Guess no on NetBSD/m68k.
         m68*-*-netbsd*)
           gl_cv_func_fesetenv_works="guessing no"
           ;;
         # Guess no on mingw and MSVC.
         *86*-*-mingw* | *86*-*-windows*)
           gl_cv_func_fesetenv_works="guessing no"
           ;;
         *)
           dnl This test fails on FreeBSD 12.2/arm64.
           dnl Here we test only the rounding mode restoration, but the function
           dnl is more broken than that.
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[fenv_t env;
                  return (fesetround (FE_TOWARDZERO) == 0
                          && fegetenv (&env) == 0
                          && fesetround (FE_TONEAREST) == 0
                          && fesetenv (&env) == 0
                          && fegetround () != FE_TOWARDZERO);
                ]])
             ],
             [gl_cv_func_fesetenv_works="guessing yes"],
             [gl_cv_func_fesetenv_works=no],
             [gl_cv_func_fesetenv_works="guessing yes"])
           LIBS="$saved_LIBS"
           ;;
       esac
      ])
    case "$gl_cv_func_fesetenv_works" in
      *yes) ;;
      *) REPLACE_FESETENV=1 ;;
    esac
    dnl The feupdateenv function does not work on glibc 2.37/riscv64
    dnl (see <https://sourceware.org/PR31022>)
    dnl and on glibc 2.37/hppa
    dnl (see <https://sourceware.org/PR31023>)
    dnl and on glibc 2.5/{i386,x86_64,ia64} (see
    dnl <https://sourceware.org/git/?p=glibc.git;a=commitdiff;h=a8c79c4088e8c04e4297936efa0dee6c8e6e974d>)
    dnl and on Mac OS X 10.5/{i386,x86_64} (where it forgets about the currently
    dnl set floating-point exception flags)
    dnl and on musl libc/s390x and NetBSD 10.0/powerpc (where it does not
    dnl trigger traps)
    dnl and on musl libc/{i386,x86_64} and AIX and Solaris and mingw 10 (where
    dnl it fails to restore the exception trap bits),
    dnl and on FreeBSD 12.2/arm64 (see
    dnl <https://cgit.freebsd.org/src/commit/?id=34cc08e336987a8ebc316595e3f552a4c09f1fd4>),
    dnl and on mingw 13 (where feupdateenv (FE_DFL_ENV) does not restore the
    dnl rounding direction).
    dnl On MSVC 14 it may even fail.
    AC_CACHE_CHECK([whether feupdateenv works],
      [gl_cv_func_feupdateenv_works],
      [case "$host" in
         # Guess no on glibc/riscv.
         riscv*-*-*-gnu*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[return feupdateenv (FE_DFL_ENV) != 0;
                ]])
             ],
             [gl_cv_func_feupdateenv_works=yes],
             [gl_cv_func_feupdateenv_works=no],
             [gl_cv_func_feupdateenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         # Guess no on glibc/hppa and NetBSD/powerpc64.
         hppa*-*-*-gnu* | powerpc*-*-netbsd*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[fenv_t env;
                  feclearexcept (FE_ALL_EXCEPT);
                  feenableexcept (FE_INVALID);
                  fegetenv (&env);
                  fesetenv (FE_DFL_ENV);
                  feraiseexcept (FE_INVALID);
                  feupdateenv (&env);
                  return 0;
                ]])
             ],
             [gl_cv_func_feupdateenv_works=no],
             [gl_cv_func_feupdateenv_works="guessing yes"],
             [gl_cv_func_feupdateenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         # Guess no on glibc ≤ 2.5/{i386,x86_64,ia64}.
         *86*-*-*-gnu* | *86*-*-gnu* | ia64*-*-*-gnu*)
           AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <string.h> /* for __GNU_LIBRARY__ */
                #ifdef __GNU_LIBRARY__
                 #include <features.h>
                 #if __GLIBC__ == 2 && __GLIBC_MINOR__ <= 5
                  Unlucky user
                 #endif
                #endif
                ]],
                [])],
             [gl_cv_func_feupdateenv_works="guessing yes"],
             [gl_cv_func_feupdateenv_works="guessing no"])
           ;;
         i?86-*-*-musl* | x86_64-*-*-musl* | \
         s390*-*-*-musl*)
           dnl This is only needed when the module 'fenv-exceptions-trapping' is
           dnl in use.
           m4_ifdef([g][l_FENV_EXCEPTIONS_TRAPPING], [
             gl_cv_func_feupdateenv_works="guessing no"
           ], [gl_cv_func_feupdateenv_works="guessing yes"])
           ;;
         *-*-aix*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                #include <fptrap.h>
                ]],
                [[fenv_t env;
                  feclearexcept (FE_ALL_EXCEPT);
                  if (fegetenv (&env) == 0)
                    {
                      fp_enable (TRP_DIV_BY_ZERO);
                      if (!fp_is_enabled (TRP_DIV_BY_ZERO))
                        return 1;
                      if (feupdateenv (&env) == 0
                          && fp_is_enabled (TRP_DIV_BY_ZERO))
                        return 2;
                    }
                  return 0;
                ]])
             ],
             [gl_cv_func_feupdateenv_works="guessing yes"],
             [gl_cv_func_feupdateenv_works=no],
             [gl_cv_func_feupdateenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         *-*-solaris*)
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                #include <ieeefp.h>
                ]],
                [[fenv_t env1, env2;
                  feclearexcept (FE_ALL_EXCEPT);
                  if (fegetenv (&env1) == 0)
                    {
                      fpsetmask (FP_X_DZ);
                      if (fegetenv (&env2) == 0)
                        {
                          if (fpgetmask () != FP_X_DZ)
                            return 1;
                          if (feupdateenv (&env1) == 0)
                            {
                              if (fpgetmask () != 0)
                                return 2;
                              fpsetmask (FP_X_INV);
                              if (fpgetmask () != FP_X_INV)
                                return 3;
                              if (feupdateenv (&env2) == 0)
                                {
                                  if (fpgetmask () != FP_X_DZ)
                                    return 4;
                                }
                            }
                        }
                    }
                  return 0;
                ]])
             ],
             [gl_cv_func_feupdateenv_works="guessing yes"],
             [gl_cv_func_feupdateenv_works=no],
             [gl_cv_func_feupdateenv_works="guessing no"])
           LIBS="$saved_LIBS"
           ;;
         # Guess no on Mac OS X/{i386,x86_64}.
         *86*-*-darwin*)
           gl_cv_func_feupdateenv_works="guessing no"
           ;;
         # Guess no on mingw and MSVC.
         *86*-*-mingw* | *86*-*-windows*)
           gl_cv_func_feupdateenv_works="guessing no"
           ;;
         *)
           dnl This test fails on FreeBSD 12.2/arm64.
           dnl Here we test only the rounding mode restoration, but the function
           dnl is more broken than that.
           saved_LIBS="$LIBS"
           LIBS="$LIBS $FENV_ENVIRONMENT_LIBM"
           AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[
                #include <fenv.h>
                ]],
                [[fenv_t env;
                  return (fesetround (FE_TOWARDZERO) == 0
                          && fegetenv (&env) == 0
                          && fesetround (FE_TONEAREST) == 0
                          && feupdateenv (&env) == 0
                          && fegetround () != FE_TOWARDZERO);
                ]])
             ],
             [gl_cv_func_feupdateenv_works="guessing yes"],
             [gl_cv_func_feupdateenv_works=no],
             [gl_cv_func_feupdateenv_works="guessing yes"])
           LIBS="$saved_LIBS"
           ;;
       esac
      ])
    case "$gl_cv_func_feupdateenv_works" in
      *yes) ;;
      *) REPLACE_FEUPDATEENV=1 ;;
    esac
    dnl Two functions are in the same compilation unit.
    if test $REPLACE_FEGETENV = 1 || test $REPLACE_FESETENV = 1; then
      REPLACE_FEGETENV=1
      REPLACE_FESETENV=1
    fi
    dnl On AIX, feholdexcept, like fegetenv, does not save the exception trap
    dnl bits.
    case "$host_os" in
      aix*)
        if test $REPLACE_FEGETENV = 1; then
          REPLACE_FEHOLDEXCEPT=1
        fi
        ;;
    esac
  else
    HAVE_FEGETENV=0
    HAVE_FESETENV=0
    HAVE_FEUPDATEENV=0
    HAVE_FEHOLDEXCEPT=0
    dnl It needs linking with -lm on platforms which define FE_DFL_ENV as the
    dnl address of a global variable.
    case "$host" in
      powerpc*-*-linux*-gnu* | \
      *-*-freebsd* | *-*-dragonfly* | \
      *-*-netbsd* | \
      *-*-openbsd* | \
      *-*-solaris* )
        FENV_ENVIRONMENT_LIBM=-lm ;;
      *) FENV_ENVIRONMENT_LIBM= ;;
    esac
  fi
  AC_SUBST([FENV_ENVIRONMENT_LIBM])
])
