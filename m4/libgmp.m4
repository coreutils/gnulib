# libgmp.m4 serial 3
# Configure the GMP library or a replacement.
dnl Copyright 2020 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl gl_LIBGMP
dnl Searches for an installed libgmp.
dnl If found, it sets and AC_SUBSTs HAVE_LIBGMP=yes and the LIBGMP and LTLIBGMP
dnl variables, and augments the CPPFLAGS variable, and #defines HAVE_LIBGMP.
dnl Otherwise, it sets and AC_SUBSTs HAVE_LIBGMP=no and LIBGMP and LTLIBGMP to
dnl empty.

AC_DEFUN([gl_LIBGMP],
[
  AC_ARG_WITH([libgmp],
    [AS_HELP_STRING([--without-libgmp],
       [do not use the GNU Multiple Precision (GMP) library;
        this is the default on systems lacking libgmp.])])
  case "$with_libgmp" in
    no)
      HAVE_LIBGMP=no
      LIBGMP=
      LTLIBGMP=
      ;;
    *)
      AC_LIB_HAVE_LINKFLAGS([gmp], [],
        [#include <gmp.h>],
        [static const mp_limb_t x[2] = { 0x73, 0x55 };
         mpz_t tmp;
         mpz_roinit_n (tmp, x, 2);
        ],
        [no])
      if test $HAVE_LIBGMP = no; then
        case "$with_libgmp" in
          yes)
            AC_MSG_ERROR([GMP not found, although --with-libgmp was specified. Try specifying --with-libgmp-prefix=DIR.])
            ;;
        esac
      fi
      ;;
  esac
  if test $HAVE_LIBGMP = yes; then
    GMP_H=
  else
    GMP_H=gmp.h
  fi
  AC_SUBST([GMP_H])
  AM_CONDITIONAL([GL_GENERATE_GMP_H], [test -n "$GMP_H"])
])
