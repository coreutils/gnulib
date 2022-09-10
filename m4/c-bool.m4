# Check for bool that conforms to C2023.

dnl Copyright 2022 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_C_BOOL],
[
  AC_CACHE_CHECK([for bool, true, false], [gl_cv_c_bool],
    [AC_COMPILE_IFELSE(
       [AC_LANG_SOURCE([[
          #if true == false
           #error "true == false"
          #endif
          extern bool b;
          bool b = true == false;]])],
       [gl_cv_c_bool=yes],
       [gl_cv_c_bool=no])])
  if test "$gl_cv_c_bool" = yes; then
    AC_DEFINE([HAVE_C_BOOL], [1],
      [Define to 1 if bool, true and false work as per C2023.])
  fi

  dnl The "zz" puts this toward config.h's end, to avoid potential
  dnl collisions with other definitions.  Check
  dnl __bool_true_false_are_defined to avoid re-including <stdbool.h>.
  AH_VERBATIM([zzbool],
[#if (!defined HAVE_C_BOOL && !defined __cplusplus \
     && !defined __bool_true_false_are_defined)
 #include <stdbool.h>
#endif])
])
