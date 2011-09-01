# pthread.m4 serial 2
dnl Copyright (C) 2009-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_PTHREAD_CHECK],
[
   AC_REQUIRE([gl_PTHREAD_DEFAULTS])
   gl_CHECK_NEXT_HEADERS([pthread.h])
   if test $ac_cv_header_pthread_h = yes; then
     HAVE_PTHREAD_H=1
   else
     HAVE_PTHREAD_H=0
   fi

   AC_CHECK_TYPES([pthread_t, pthread_spinlock_t], [], [],
     [AC_INCLUDES_DEFAULT[
      #if HAVE_PTHREAD_H
       #include <pthread.h>
      #endif]])
   if test $ac_cv_type_pthread_t != yes; then
     HAVE_PTHREAD_T=0
   fi
   if test $ac_cv_type_pthread_spinlock_t != yes; then
     HAVE_PTHREAD_SPINLOCK_T=0
   fi

   if test $ac_cv_header_pthread_h != yes ||
      test $ac_cv_type_pthread_t != yes ||
      test $ac_cv_type_pthread_spinlock_t != yes; then
     PTHREAD_H='pthread.h'
   else
     PTHREAD_H=
   fi
   AC_SUBST([PTHREAD_H])
   AM_CONDITIONAL([GL_GENERATE_PTHREAD_H], [test -n "$PTHREAD_H"])

   LIB_PTHREAD=
   if test $ac_cv_header_pthread_h = yes; then
     gl_saved_libs=$LIBS
     AC_SEARCH_LIBS([pthread_join], [pthread],
       [if test "$ac_cv_search_pthread_join" != "none required"; then
          LIB_PTHREAD="$ac_cv_search_pthread_join"
        fi])
     LIBS="$gl_saved_libs"
   fi
   AC_SUBST([LIB_PTHREAD])

   AC_REQUIRE([AC_C_INLINE])
   AC_REQUIRE([AC_C_RESTRICT])
])

AC_DEFUN([gl_PTHREAD_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_PTHREAD_H=1;              AC_SUBST([HAVE_PTHREAD_H])
  HAVE_PTHREAD_T=1;              AC_SUBST([HAVE_PTHREAD_T])
  HAVE_PTHREAD_SPINLOCK_T=1;     AC_SUBST([HAVE_PTHREAD_SPINLOCK_T])
])
