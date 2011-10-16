# Check for stdalign.h that conforms to C1x.

dnl Copyright 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Prepare for substituting <stdalign.h> if it is not supported.

AC_DEFUN([gl_STDALIGN_H],
[
  AC_CHECK_HEADERS_ONCE([stdalign.h])
  HAVE_ATTRIBUTE_ALIGNED='?'

  if test "$ac_cv_header_stdalign_h" = yes; then
    STDALIGN_H=''
  else
    STDALIGN_H='stdalign.h'
    AC_CACHE_CHECK([for  __attribute__ ((__aligned__ (expr)))],
      [gl_cv_attribute_aligned],
      [AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
            [[char __attribute__ ((__aligned__ (1 << 3))) c;]],
            [[]])],
         [gl_cv_attribute_aligned=yes],
         [gl_cv_attribute_aligned=no])])
    if test $gl_cv_attribute_aligned = yes; then
      HAVE_ATTRIBUTE_ALIGNED=1
    else
      HAVE_ATTRIBUTE_ALIGNED=0
    fi
  fi

  AC_SUBST([HAVE_ATTRIBUTE_ALIGNED])
  AC_SUBST([STDALIGN_H])
  AM_CONDITIONAL([GL_GENERATE_STDALIGN_H], [test -n "$STDALIGN_H"])
])
