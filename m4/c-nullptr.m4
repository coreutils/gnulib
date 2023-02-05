# Check for nullptr that conforms to C23.

dnl Copyright 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_C_NULLPTR],
[
  AC_CACHE_CHECK([for nullptr], [gl_cv_c_nullptr],
    [AC_COMPILE_IFELSE(
       [AC_LANG_SOURCE([[int *p = nullptr;]])],
       [gl_cv_c_nullptr=yes],
       [gl_cv_c_nullptr=no])])
  if test "$gl_cv_c_nullptr" = yes; then
    AC_DEFINE([HAVE_C_NULLPTR], [1], [Define to 1 if nullptr works.])
  fi
])

  AH_VERBATIM([nullptr],
[#ifndef HAVE_C_NULLPTR
# ifndef __cplusplus
#  define nullptr ((void *) 0)
# elif 3 <= __GNUG__
#  define nullptr __null
# else
#  define nullptr 0L
# endif
#endif])
])
