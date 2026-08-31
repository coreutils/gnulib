# sd-dlopen.m4
# serial 4
dnl Copyright (C) 2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

AC_DEFUN([gl_SD_DLOPEN],
[
  AC_REQUIRE([gl_LIBDL])
  AC_REQUIRE([AC_CANONICAL_HOST])

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

    AC_CACHE_CHECK([whether the assembler supports the section flag o],
      [gl_cv_asm_section_o],
      [dnl This flag is only supported by binutils >= 2.35 or clang >= 5.
       AC_COMPILE_IFELSE(
         [AC_LANG_SOURCE([[
          __asm__ (".section .def; foobar:; .section .foo,\"ao\",%note,foobar");
         ]])],
         [gl_cv_asm_section_o=yes],
         [gl_cv_asm_section_o=no])])
    if test $gl_cv_asm_section_o = yes; then
      AC_DEFINE([_SD_ELF_NOTE_SUPPORTS_REFERENCES], [1],
        [Define to 1 if the ELF .section command supports the o flag.])
    fi

    AC_CACHE_CHECK([whether the assembler supports the section flag R],
      [gl_cv_asm_section_R],
      [dnl This flag is only supported by binutils >= 2.36 or clang >= 13.
       AC_COMPILE_IFELSE(
         [AC_LANG_SOURCE([[
          __asm__ (".section .foo,\"aR\"");
         ]])],
         [gl_cv_asm_section_R=yes],
         [gl_cv_asm_section_R=no])])
    dnl On Solaris 11.4 (machines cfarm215.cfarm.net, cfarm216.cfarm.net),
    dnl the use of the section flag R causes the assembler to mark the object
    dnl file with ELF OS/ABI "UNIX - GNU" rather than "UNIX - System V",
    dnl which then leads to an error
    dnl   ld: fatal: file test-sd-dlopen.o: wrong ELF OSABI: ELFOSABI_GNU
    dnl Therefore, on Solaris, avoid the section flag R, even if the assembler
    dnl supports it.
    if test $gl_cv_asm_section_R = yes \
       && case "$host_os" in solaris*) false;; *) true;; esac; then
      gl_sd_dlopen_section_flags='"aGR"'
    else
      gl_sd_dlopen_section_flags='"aG"'
    fi
    AC_DEFINE_UNQUOTED([_SD_ELF_NOTE_DLOPEN_SECTION_FLAGS],
      [$gl_sd_dlopen_section_flags],
      [Define to the section flags for sd-dlopen.h.])
  fi
])
