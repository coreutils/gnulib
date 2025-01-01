# aligned_alloc.m4
# serial 9
dnl Copyright (C) 2020-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_FUNC_ALIGNED_ALLOC],
[
  AC_REQUIRE([gl_STDLIB_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  dnl Persuade glibc and OpenBSD <stdlib.h> to declare aligned_alloc().
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])

  gl_CHECK_FUNCS_ANDROID([aligned_alloc], [[#include <stdlib.h>]])
  if test $ac_cv_func_aligned_alloc = yes; then
    dnl On macOS 11.1 and AIX 7.3.1, aligned_alloc returns NULL when the
    dnl alignment argument is smaller than sizeof (void *).
    dnl On Solaris 11.4, aligned_alloc returns NULL if the size is not a
    dnl multiple of the alignment.
    dnl On macOS 15, AIX 7.3, Solaris 11.4, aligned_alloc with a zero size
    dnl returns NULL.
    AC_CACHE_CHECK([whether aligned_alloc works for small alignments and sizes],
      [gl_cv_func_aligned_alloc_works],
      [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM(
            [[#include <stdlib.h>
              /* Use paligned_alloc to test; 'volatile' prevents the compiler
                 from optimizing the aligned_alloc call away.  */
              void *(*volatile paligned_alloc) (size_t, size_t)
                = aligned_alloc;]],
            [[int result = 0;
              {
                void *p = paligned_alloc (2, 18);
                if (p == NULL)
                  result |= 1;
              }
              {
                void *p = paligned_alloc (sizeof (void *), sizeof (void *) + 1);
                if (p == NULL)
                  result |= 2;
              }
              {
                void *p = paligned_alloc (sizeof (void *), 0);
                if (p == NULL)
                  result |= 4;
              }
              return result;
            ]])
         ],
         [gl_cv_func_aligned_alloc_works=yes],
         [gl_cv_func_aligned_alloc_works=no],
         [case "$host_os" in
                      # Guess no on macOS.
            darwin*)  gl_cv_func_aligned_alloc_works="guessing no" ;;
                      # Guess no on AIX.
            aix*)     gl_cv_func_aligned_alloc_works="guessing no" ;;
                      # Guess no on Solaris.
            solaris*) gl_cv_func_aligned_alloc_works="guessing no" ;;
                      # If we don't know, obey --enable-cross-guesses.
            *)        gl_cv_func_aligned_alloc_works="$gl_cross_guess_normal" ;;
          esac
         ])
      ])
    case "$gl_cv_func_aligned_alloc_works" in
      *yes) ;;
      *) REPLACE_ALIGNED_ALLOC=1 ;;
    esac
  else
    dnl The system does not have aligned_alloc.
    HAVE_ALIGNED_ALLOC=0
    case "$gl_cv_onwards_func_aligned_alloc" in
      future*) REPLACE_ALIGNED_ALLOC=1 ;;
    esac
  fi
])
