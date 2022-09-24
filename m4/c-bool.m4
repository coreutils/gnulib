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

  AC_CHECK_HEADERS_ONCE([stdbool.h])
  AC_REQUIRE([AC_CHECK_HEADER_STDBOOL])
  AC_DEFINE_UNQUOTED([HAVE__BOOL],[$HAVE__BOOL])

  dnl The "zz" puts this toward config.h's end, to avoid potential
  dnl collisions with other definitions.
  dnl If 'bool', 'true' and 'false' do not work, arrange for them to work.
  dnl In C, this means including <stdbool.h> if it is not already included.
  dnl However, if the preprocessor mistakenly treats 'true' as 0,
  dnl define it to a bool expression equal to 1; this is needed in
  dnl Sun C++ 5.11 (Oracle Solaris Studio 12.2, 2010) and older.
  AH_VERBATIM([zzbool],
[#ifndef HAVE_C_BOOL
# if !defined __cplusplus && !defined __bool_true_false_are_defined
#  if HAVE_STDBOOL_H
#   include <stdbool.h>
#  else
    /* Substitute.  Keep consistent with gnulib/lib/stdbool.in.h.  */
#   ifndef _GL_STDBOOL_H
#    define _GL_STDBOOL_H
#    ifdef __cplusplus
#     if !defined _MSC_VER
#      define _Bool bool
#      define bool bool
#     endif
#    else
#     if !defined __GNUC__
#      define _Bool signed char
#     else
#      if !HAVE__BOOL
typedef enum { _Bool_must_promote_to_int = -1, false = 0, true = 1 } _Bool;
#      endif
#     endif
#     define bool _Bool
#    endif
#    ifdef __cplusplus
#     if !defined _MSC_VER
#      define false false
#      define true true
#     endif
#     if defined __SUNPRO_CC && true != 1
#      undef true
#      define true (!false)
#     endif
#    else
#     define false 0
#     define true 1
#    endif
#    define __bool_true_false_are_defined 1
#   endif /* _GL_STDBOOL_H */
#  endif
# endif
# if !true
#  define true (!false)
# endif
#endif])
])
