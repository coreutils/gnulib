# Check for nullptr that conforms to C23 and C++11.

dnl Copyright 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_NULLPTR],
[
  AS_IF([test ${CC+set}],
    [AC_CACHE_CHECK([for C nullptr], [gl_cv_c_nullptr],
       [AC_COMPILE_IFELSE(
          [AC_LANG_SOURCE([[int *p = nullptr;]])],
          [gl_cv_c_nullptr=yes],
          [gl_cv_c_nullptr=no])])])
  if test "$gl_cv_c_nullptr" = yes; then
    AC_DEFINE([HAVE_C_NULLPTR], [1], [Define to 1 if C nullptr works.])
  fi
  AS_IF([test ${CXX+set}],
    [AC_CACHE_CHECK([for C++ nullptr], [gl_cv_cxx_nullptr],
       [AC_COMPILE_IFELSE(
          [AC_LANG_SOURCE([[int *p = nullptr;]])],
          [gl_cv_cxx_nullptr=yes],
          [gl_cv_cxx_nullptr=no])])])
  if test "$gl_cv_cxx_nullptr" = yes; then
    AC_DEFINE([HAVE_CXX_NULLPTR], [1], [Define to 1 if C++ nullptr works.])
  fi
])

  AH_VERBATIM([nullptr],
[#ifndef nullptr /* keep config.h idempotent */
# if !defined __cplusplus && !defined HAVE_C_NULLPTR
#  define nullptr ((void *) 0)
# elif defined __cplusplus && !defined HAVE_CXX_NULLPTR
#  if 3 <= __GNUG__
#   define nullptr __null
#  else
#   define nullptr 0L
#  endif
# endif
#endif])
])
