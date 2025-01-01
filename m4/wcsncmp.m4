# wcsncmp.m4
# serial 5
dnl Copyright (C) 2011-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_WCSNCMP],
[
  AC_REQUIRE([gl_WCHAR_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CHECK_FUNCS_ONCE([wcsncmp])
  if test $ac_cv_func_wcsncmp = no; then
    HAVE_WCSNCMP=0
  else
    AC_CACHE_CHECK([whether wcsncmp works for all wide characters],
      [gl_cv_func_wcsncmp_works],
      [AC_RUN_IFELSE(
         [AC_LANG_SOURCE([[
            #include <wchar.h>
            int main ()
            {
              int result = 0;
              { /* This test fails on glibc < 2.15, musl libc 1.2.3, macOS 12.5,
                   FreeBSD 13.2, NetBSD 10.0, OpenBSD 7.2, Solaris 11.4.  */
                wchar_t a[2] = { (wchar_t) 0x76547654, 0 };
                wchar_t b[2] = { (wchar_t) 0x9abc9abc, 0 };
                int cmp = wcsncmp (a, b, 1);
                if (!((wchar_t)-1 < 0 ? cmp > 0 : cmp < 0))
                  result |= 1;
              }
              { /* This test fails on AIX in 64-bit mode.  */
                wchar_t c[2] = { (wchar_t) 'x', 0 };
                wchar_t d[3] = { (wchar_t) 'x', (wchar_t) 0x9abc9abc, 0 };
                int cmp = wcsncmp (c, d, 2);
                if (!((wchar_t)-1 < 0 ? cmp > 0 : cmp < 0))
                  result |= 2;
              }
              return result;
            }
            ]])
         ],
         [gl_cv_func_wcsncmp_works=yes],
         [gl_cv_func_wcsncmp_works=no],
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
                [gl_cv_func_wcsncmp_works="guessing no"],
                [gl_cv_func_wcsncmp_works="guessing yes"])
              ;;
            # Guess no on musl systems.
            *-musl* | midipix*) gl_cv_func_wcsncmp_works="guessing no" ;;
            # If we don't know, obey --enable-cross-guesses.
            *) gl_cv_func_wcsncmp_works="$gl_cross_guess_normal" ;;
          esac
         ])
      ])
    case "$gl_cv_func_wcsncmp_works" in
      *yes) ;;
      *) REPLACE_WCSNCMP=1 ;;
    esac
  fi
])
