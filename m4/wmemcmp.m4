# wmemcmp.m4
# serial 6
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_WMEMCMP],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  dnl We cannot use AC_CHECK_FUNCS here, because the MSVC 9 header files
  dnl provide this function as an inline function definition.
  AC_CACHE_CHECK([for wmemcmp], [gl_cv_func_wmemcmp],
    [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <wchar.h>
          ]],
          [[return ! wmemcmp ((const wchar_t *) 0, (const wchar_t *) 0, 0);]])
       ],
       [gl_cv_func_wmemcmp=yes],
       [gl_cv_func_wmemcmp=no])
    ])
  if test $gl_cv_func_wmemcmp = no; then
    HAVE_WMEMCMP=0
  else
    AC_CACHE_CHECK([whether wmemcmp works for all wide characters],
      [gl_cv_func_wmemcmp_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
            #include <wchar.h>
            int main ()
            {
              wchar_t a = (wchar_t) 0x76547654;
              wchar_t b = (wchar_t) 0x9abc9abc;
              int cmp = wmemcmp (&a, &b, 1);
              if ((wchar_t)-1 < 0)
                return !(cmp > 0);
              else
                return !(cmp < 0);
            }
            ]])
         ],
         [gl_cv_func_wmemcmp_works=yes],
         [gl_cv_func_wmemcmp_works=no],
         [case "$host_on" in
            # Guess no on glibc versions < 2.15.
            *-gnu* | gnu*)
              AC_EGREP_CPP([Unlucky],
                [
#include <features.h>
#ifdef __GNU_LIBRARY__
 #if (__GLIBC__ == 2 && __GLIBC_MINOR__ < 15)
  Unlucky GNU user
 #endif
#endif
                ],
                [gl_cv_func_wmemcmp_works="guessing no"],
                [gl_cv_func_wmemcmp_works="guessing yes"])
              ;;
            # Guess no on musl systems.
            *-musl* | midipix*) gl_cv_func_wmemcmp_works="guessing no" ;;
            # If we don't know, obey --enable-cross-guesses.
            *) gl_cv_func_wmemcmp_works="$gl_cross_guess_normal" ;;
          esac
         ])
      ])
    case "$gl_cv_func_wmemcmp_works" in
      *yes) ;;
      *) REPLACE_WMEMCMP=1 ;;
    esac
  fi
])
