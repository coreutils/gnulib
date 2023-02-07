# Check for nullptr that conforms to C23.

dnl Copyright 2023 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_NULLPTR],
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
[#ifndef nullptr /* keep config.h idempotent */
# ifdef __cplusplus
/* For the C++ compiler the result of the configure test is irrelevant.
   We know that at least g++ and clang with option -std=c++11 or higher, as well
   as MSVC 14 or newer, already have nullptr.  */
#  if !(((defined __GNUC__ || defined __clang__) && __cplusplus >= 201103L) \
        || (defined _MSC_VER && 1900 <= _MSC_VER))
/* Define nullptr as a macro, the best we can.  */
#   if 3 <= __GNUG__
#    define nullptr __null
#   else
#    define nullptr 0L
#   endif
#  endif
# else
/* For the C compiler, use the result of the configure test.  */
#  ifndef HAVE_C_NULLPTR
#   define nullptr ((void *) 0)
#  endif
# endif
#endif])
])
