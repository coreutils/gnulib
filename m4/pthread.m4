# pthread.m4
dnl Copyright (C) 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_PTHREAD_CHECK],
  [AC_CHECK_HEADERS_ONCE([pthread.h])

   LIB_PTHREAD=
   PTHREAD_H=
   if test "$ac_cv_header_pthread_h" = yes; then
     gl_saved_libs=$LIBS
     AC_SEARCH_LIBS([pthread_create], [pthread],
       [if test "$ac_cv_search_pthread_create" != "none required"; then
          LIB_PTHREAD="$ac_cv_search_pthread_create"
        fi])
     LIBS="$gl_saved_libs"
   else
     PTHREAD_H='pthread.h'
   fi

   AC_SUBST([LIB_PTHREAD])
   AC_SUBST([PTHREAD_H])

   AC_REQUIRE([AC_C_RESTRICT])
])
