# posix_memalign.m4
# serial 6
dnl Copyright (C) 2020-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_POSIX_MEMALIGN],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade glibc <stdlib.h> to declare posix_memalign().
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  gl_CHECK_FUNCS_ANDROID([posix_memalign], [[#include <stdlib.h>]])
  if test $ac_cv_func_posix_memalign = yes; then
    dnl On OpenBSD 6.1, posix_memalign (&p, 32, 2406) stores a pointer
    dnl that is not a multiple of 32.
    dnl On AIX 7.3, aligned_alloc with a zero size returns NULL.
    AC_CACHE_CHECK(
      [whether posix_memalign works for large alignment or zero size],
      [gl_cv_func_posix_memalign_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stdlib.h>
              /* Use pposix_memalign to test; 'volatile' prevents the compiler
                 from optimizing the posix_memalign call away.  */
              int (*volatile pposix_memalign) (void **, size_t, size_t)
                = posix_memalign;]],
            [[void *p;
              if (pposix_memalign (&p, 0, sizeof (void *)) != 0)
                return 1;
              if (32 % sizeof (void *) == 0
                  && pposix_memalign (&p, 32, 2406) == 0
                  && (unsigned long) p % 32 != 0)
                return 1;
              return 0;
            ]])
         ],
         [gl_cv_func_posix_memalign_works=yes],
         [gl_cv_func_posix_memalign_works=no],
         [case "$host_os" in
changequote(,)dnl
                      # Guess no on AIX.
            aix*)     gl_cv_func_posix_memalign_works="guessing no" ;;
                      # Guess no on OpenBSD through 6.1.
            openbsd[1-5].* | openbsd6.[01] | openbsd6.[01].*)
                      gl_cv_func_posix_memalign_works="guessing no" ;;
                      # If we don't know, obey --enable-cross-guesses.
            *)        gl_cv_func_posix_memalign_works="$gl_cross_guess_normal" ;;
changequote([,])dnl
          esac
         ])
      ])
    case "$gl_cv_func_posix_memalign_works" in
      *yes) ;;
      *) REPLACE_POSIX_MEMALIGN=1 ;;
    esac
  else
    dnl The system does not have posix_memalign.
    HAVE_POSIX_MEMALIGN=0
    case "$gl_cv_onwards_func_posix_memalign" in
      future*) REPLACE_POSIX_MEMALIGN=1 ;;
    esac
  fi
])
