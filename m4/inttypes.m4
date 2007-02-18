# inttypes.m4 serial 8
dnl Copyright (C) 2006-2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl From Derek Price, Bruno Haible.
dnl Test whether <inttypes.h> is supported or must be substituted.

AC_DEFUN([gl_INTTYPES_H],
[
  AC_REQUIRE([gl_STDINT_H])
  AC_REQUIRE([gt_INTTYPES_PRI])
  AC_CHECK_DECLS_ONCE([imaxabs])
  AC_CHECK_DECLS_ONCE([imaxdiv])
  AC_CHECK_DECLS_ONCE([strtoimax])
  AC_CHECK_DECLS_ONCE([strtoumax])

  dnl Now see if we need a substitute <inttypes.h>.
  dnl A complete <inttypes.h> requires
  dnl   - a complete <stdint.h>,
  dnl   - the existence of an <inttypes.h>,
  dnl   - that imaxabs, imaxdiv, strtoimax, strtoumax are declared,
  dnl   - some additional tests.
  AC_CACHE_CHECK([whether inttypes.h conforms to C99],
    [gl_cv_header_working_inttypes_h],
    [gl_cv_header_working_inttypes_h=no
     if test "$gl_cv_header_working_stdint_h" = yes \
	&& test $ac_cv_header_inttypes_h = yes \
	&& test "$ac_cv_have_decl_imaxabs" = yes \
	&& test "$ac_cv_have_decl_imaxdiv" = yes \
	&& test "$ac_cv_have_decl_strtoimax" = yes \
	&& test "$ac_cv_have_decl_strtoumax" = yes; then
       AC_COMPILE_IFELSE([
	 AC_LANG_PROGRAM([
#include <stddef.h>
#define __STDC_LIMIT_MACROS 1 /* to make it work also in C++ mode */
#define __STDC_CONSTANT_MACROS 1 /* to make it work also in C++ mode */
#define __STDC_FORMAT_MACROS 1 /* to make it work also in C++ mode */
#include ABSOLUTE_INTTYPES_H

/* No need to duplicate the tests of stdint.m4; they are subsumed by
   $gl_cv_header_working_stdint_h = yes.  */

/* Tests for macros supposed to be defined in inttypes.h.  */

const char *k = /* implicit string concatenation */
#ifdef INT8_MAX
  PRId8 PRIi8
#endif
#ifdef UINT8_MAX
  PRIo8 PRIu8 PRIx8 PRIX8
#endif
#ifdef INT16_MAX
  PRId16 PRIi16
#endif
#ifdef UINT16_MAX
  PRIo16 PRIu16 PRIx16 PRIX16
#endif
#ifdef INT32_MAX
  PRId32 PRIi32
#endif
#ifdef UINT32_MAX
  PRIo32 PRIu32 PRIx32 PRIX32
#endif
#ifdef INT64_MAX
  PRId64 PRIi64
#endif
#ifdef UINT64_MAX
  PRIo64 PRIu64 PRIx64 PRIX64
#endif
  PRIdLEAST8 PRIiLEAST8 PRIoLEAST8 PRIuLEAST8 PRIxLEAST8 PRIXLEAST8
  PRIdLEAST16 PRIiLEAST16 PRIoLEAST16 PRIuLEAST16 PRIxLEAST16 PRIXLEAST16
  PRIdLEAST32 PRIiLEAST32 PRIoLEAST32 PRIuLEAST32 PRIxLEAST32 PRIXLEAST32
  PRIdLEAST64 PRIiLEAST64
  PRIoLEAST64 PRIuLEAST64 PRIxLEAST64 PRIXLEAST64
  PRIdFAST8 PRIiFAST8 PRIoFAST8 PRIuFAST8 PRIxFAST8 PRIXFAST8
  PRIdFAST16 PRIiFAST16 PRIoFAST16 PRIuFAST16 PRIxFAST16 PRIXFAST16
  PRIdFAST32 PRIiFAST32 PRIoFAST32 PRIuFAST32 PRIxFAST32 PRIXFAST32
  PRIdFAST64 PRIiFAST64
  PRIoFAST64 PRIuFAST64 PRIxFAST64 PRIXFAST64
  PRIdMAX PRIiMAX PRIoMAX PRIuMAX PRIxMAX PRIXMAX
#ifdef INTPTR_MAX
  PRIdPTR PRIiPTR
#endif
#ifdef UINTPTR_MAX
  PRIoPTR PRIuPTR PRIxPTR PRIXPTR
#endif
  ;
const char *l = /* implicit string concatenation */
#ifdef INT8_MAX
  SCNd8 SCNi8
#endif
#ifdef UINT8_MAX
  SCNo8 SCNu8 SCNx8
#endif
#ifdef INT16_MAX
  SCNd16 SCNi16
#endif
#ifdef UINT16_MAX
  SCNo16 SCNu16 SCNx16
#endif
#ifdef INT32_MAX
  SCNd32 SCNi32
#endif
#ifdef UINT32_MAX
  SCNo32 SCNu32 SCNx32
#endif
#ifdef INT64_MAX
  SCNd64 SCNi64
#endif
#ifdef UINT64_MAX
  SCNo64 SCNu64 SCNx64
#endif
  SCNdLEAST8 SCNiLEAST8 SCNoLEAST8 SCNuLEAST8 SCNxLEAST8
  SCNdLEAST16 SCNiLEAST16 SCNoLEAST16 SCNuLEAST16 SCNxLEAST16
  SCNdLEAST32 SCNiLEAST32 SCNoLEAST32 SCNuLEAST32 SCNxLEAST32
  SCNdLEAST64 SCNiLEAST64
  SCNoLEAST64 SCNuLEAST64 SCNxLEAST64
  SCNdFAST8 SCNiFAST8 SCNoFAST8 SCNuFAST8 SCNxFAST8
  SCNdFAST16 SCNiFAST16 SCNoFAST16 SCNuFAST16 SCNxFAST16
  SCNdFAST32 SCNiFAST32 SCNoFAST32 SCNuFAST32 SCNxFAST32
  SCNdFAST64 SCNiFAST64
  SCNoFAST64 SCNuFAST64 SCNxFAST64
  SCNdMAX SCNiMAX SCNoMAX SCNuMAX SCNxMAX
#ifdef INTPTR_MAX
  SCNdPTR SCNiPTR
#endif
#ifdef UINTPTR_MAX
  SCNoPTR SCNuPTR SCNxPTR
#endif
  ;
	 ])],
	 [gl_cv_header_working_inttypes_h=yes])
     fi])

  dnl Override <inttypes.h> always, so that the portability warnings work.
  if false && test $gl_cv_header_working_inttypes_h = yes; then
    dnl Use the existing <inttypes.h>.
    INTTYPES_H=''
  else

    AC_REQUIRE([gl_INTTYPES_H_DEFAULTS])
    dnl AC_INCLUDES_DEFAULT defines $ac_cv_header_inttypes_h.
    if test $ac_cv_header_inttypes_h = yes; then
      gl_ABSOLUTE_HEADER([inttypes.h])
      ABSOLUTE_INTTYPES_H=\"$gl_cv_absolute_inttypes_h\"
    else
      ABSOLUTE_INTTYPES_H=\"no/such/file/inttypes.h\"
    fi
    AC_SUBST([ABSOLUTE_INTTYPES_H])

    PRIPTR_PREFIX=
    if test -n "$STDINT_H"; then
      dnl Using the gnulib <stdint.h>. It always defines intptr_t to 'long'.
      PRIPTR_PREFIX='"l"'
    else
      dnl Using the system's <stdint.h>.
      for glpfx in '' l ll I64; do
        case $glpfx in
          '')  gltype1='int';;
          l)   gltype1='long int';;
          ll)  gltype1='long long int';;
          I64) gltype1='__int64';;
        esac
        AC_COMPILE_IFELSE(
          [AC_LANG_PROGRAM([#include <stdint.h>
             extern intptr_t foo;
             extern $gltype1 foo;])],
          [PRIPTR_PREFIX='"'$glpfx'"'])
        test -n "$PRIPTR_PREFIX" && break
      done
    fi
    AC_SUBST([PRIPTR_PREFIX])

    if test "$ac_cv_have_decl_imaxabs" = yes; then
      HAVE_DECL_IMAXABS=1
    else
      HAVE_DECL_IMAXABS=0
    fi

    if test "$ac_cv_have_decl_imaxdiv" = yes; then
      HAVE_DECL_IMAXDIV=1
    else
      HAVE_DECL_IMAXDIV=0
    fi

    if test "$ac_cv_have_decl_strtoimax" = yes; then
      HAVE_DECL_STRTOIMAX=1
    else
      HAVE_DECL_STRTOIMAX=0
    fi

    if test "$ac_cv_have_decl_strtoumax" = yes; then
      HAVE_DECL_STRTOUMAX=1
    else
      HAVE_DECL_STRTOUMAX=0
    fi

    INTTYPES_H='inttypes.h'
  fi
  AC_SUBST(INTTYPES_H)
])

AC_DEFUN([gl_INTTYPES_MODULE_INDICATOR],
[
  dnl Use AC_REQUIRE here, so that the default settings are expanded once only.
  AC_REQUIRE([gl_INTTYPES_H_DEFAULTS])
  GNULIB_[]m4_translit([$1],[abcdefghijklmnopqrstuvwxyz./-],[ABCDEFGHIJKLMNOPQRSTUVWXYZ___])=1
])

AC_DEFUN([gl_INTTYPES_H_DEFAULTS],
[
  GNULIB_IMAXABS=0;      AC_SUBST([GNULIB_IMAXABS])
  GNULIB_IMAXDIV=0;      AC_SUBST([GNULIB_IMAXDIV])
  GNULIB_STRTOIMAX=0;    AC_SUBST([GNULIB_STRTOIMAX])
  GNULIB_STRTOUMAX=0;    AC_SUBST([GNULIB_STRTOUMAX])
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_DECL_IMAXABS=1;   AC_SUBST([HAVE_DECL_IMAXABS])
  HAVE_DECL_IMAXDIV=1;   AC_SUBST([HAVE_DECL_IMAXDIV])
  HAVE_DECL_STRTOIMAX=1; AC_SUBST([HAVE_DECL_STRTOIMAX])
  HAVE_DECL_STRTOUMAX=1; AC_SUBST([HAVE_DECL_STRTOUMAX])
])
