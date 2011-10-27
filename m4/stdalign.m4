# Check for stdalign.h that conforms to C1x.

dnl Copyright 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Prepare for substituting <stdalign.h> if it is not supported.

AC_DEFUN([gl_STDALIGN_H],
[
  AC_CHECK_HEADERS_ONCE([stdalign.h])

  if test $ac_cv_header_stdalign_h = yes; then
    STDALIGN_H=''
  else
    STDALIGN_H='stdalign.h'
  fi

  AC_SUBST([STDALIGN_H])
  AM_CONDITIONAL([GL_GENERATE_STDALIGN_H], [test -n "$STDALIGN_H"])
])
