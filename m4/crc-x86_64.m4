# crc-x86_64.m4
# serial 3
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_CRC_X86_64_PCLMUL],
[
  AC_CACHE_CHECK([if pclmul intrinsic exists], [gl_cv_crc_pclmul], [
    AC_LINK_IFELSE(
      [AC_LANG_SOURCE(
        [[
          #include <x86intrin.h>

          #if defined __GNUC__ || defined __clang__
          __attribute__ ((__target__ ("pclmul,avx")))
          #endif
          int
          main (void)
          {
            __m128i a, b;
            a = _mm_clmulepi64_si128 (a, b, 0x00);
            a = _mm_shuffle_epi8 (a, b);
            static __m128i_u u;
            b = _mm_loadu_si128 (&u);
            return __builtin_cpu_supports ("pclmul");
          }
        ]])
      ], [
        gl_cv_crc_pclmul=yes
      ], [
        gl_cv_crc_pclmul=no
      ])
  ])
  if test $gl_cv_crc_pclmul = yes; then
    AC_DEFINE([GL_CRC_X86_64_PCLMUL], [1],
              [CRC32 calculation by pclmul hardware instruction enabled])
  fi
  AM_CONDITIONAL([GL_CRC_X86_64_PCLMUL],
                 [test $gl_cv_crc_pclmul = yes])
])
