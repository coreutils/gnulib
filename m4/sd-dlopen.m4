# sd-dlopen.m4
# serial 1
dnl Copyright (C) 2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_SD_DLOPEN],
[
  AC_REQUIRE([gl_LIBDL])

  AC_CACHE_CHECK([whether the assembler supports .ifndef, .endif, and .balign],
    [gl_cv_asm_ifndef_endif_balign],
    [AC_COMPILE_IFELSE(
       [AC_LANG_SOURCE([[
#if !defined __ELF__ || !(defined __GNUC__ || defined __clang__)
# error "not an ELF target with GNU-style inline assembly"
#endif
__asm__ (".ifndef \"gl_sd_dlopen_test\"\n"
         ".balign 4\n"
         ".endif\n");
]])],
       [gl_cv_asm_ifndef_endif_balign=yes],
       [gl_cv_asm_ifndef_endif_balign=no])])
  if test $gl_cv_asm_ifndef_endif_balign = yes; then
    AC_DEFINE([HAVE_ASM_IFNDEF_ENDIF_BALIGN], [1],
      [Define to 1 if the assembler supports .ifndef, .endif, and .balign.])
  fi
])
