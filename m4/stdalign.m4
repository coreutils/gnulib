# Check for alignas and alignof that conform to C23.

dnl Copyright 2011-2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Prepare for substituting <stdalign.h> if it is not supported.

AC_DEFUN([gl_STDALIGN_H],
[
  AC_CACHE_CHECK([for alignas and alignof],
    [gl_cv_header_working_stdalign_h],
    [gl_save_CFLAGS=$CFLAGS
     for gl_working in "yes, keywords" "yes, <stdalign.h> macros"; do
      AS_CASE([$gl_working],
        [*stdalign.h*], [CFLAGS="$gl_save_CFLAGS -DINCLUDE_STDALIGN_H"])
      AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <stdint.h>
            #ifdef INCLUDE_STDALIGN_H
             #include <stdalign.h>
            #endif
            #include <stddef.h>

            /* Test that alignof yields a result consistent with offsetof.
               This catches GCC bug 52023
               <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52023>.  */
            #ifdef __cplusplus
               template <class t> struct alignof_helper { char a; t b; };
            # define ao(type) offsetof (alignof_helper<type>, b)
            #else
            # define ao(type) offsetof (struct { char a; type b; }, b)
            #endif
            char test_double[ao (double) % _Alignof (double) == 0 ? 1 : -1];
            char test_long[ao (long int) % _Alignof (long int) == 0 ? 1 : -1];
            char test_alignof[alignof (double) == _Alignof (double) ? 1 : -1];

            /* Test alignas only on platforms where gnulib can help.  */
            #if \
                ((defined __cplusplus && 201103 <= __cplusplus) \
                 || (__TINYC__ && defined __attribute__) \
                 || (defined __APPLE__ && defined __MACH__ \
                     ? 4 < __GNUC__ + (1 <= __GNUC_MINOR__) \
                     : __GNUC__) \
                 || (__ia64 && (61200 <= __HP_cc || 61200 <= __HP_aCC)) \
                 || __ICC || 0x590 <= __SUNPRO_C || 0x0600 <= __xlC__ \
                 || 1300 <= _MSC_VER)
              struct alignas_test { char c; char alignas (8) alignas_8; };
              char test_alignas[offsetof (struct alignas_test, alignas_8) == 8
                                ? 1 : -1];
            #endif
          ]])],
       [gl_cv_header_working_stdalign_h=$gl_working],
       [gl_cv_header_working_stdalign_h=no])

      CFLAGS=$gl_save_CFLAGS
      test "$gl_cv_header_working_stdalign_h" != no && break
     done])

  GL_GENERATE_STDALIGN_H=false
  AS_CASE([$gl_cv_header_working_stdalign_h],
    [no],
      [GL_GENERATE_STDALIGN_H=true],
    [yes*keyword*],
      [AC_DEFINE([HAVE_C_ALIGNASOF], [1],
         [Define to 1 if the alignas and alignof keywords work.])])

  dnl The "zz" puts this toward config.h's end, to avoid potential
  dnl collisions with other definitions.
  AH_VERBATIM([zzalignas],
[#if !defined HAVE_C_ALIGNASOF && __cplusplus < 201103 && !defined alignof
 #include <stdalign.h>
#endif])
])
