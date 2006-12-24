dnl A placeholder for ISO C99 <wctype.h>, for platforms that lack it.

dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_WCTYPE_H],
[
  AC_CHECK_FUNCS_ONCE([iswcntrl])
  AC_CHECK_HEADERS_ONCE([wctype.h])
  AC_REQUIRE([AC_C_INLINE])

  AC_REQUIRE([gt_TYPE_WINT_T])
  AC_SUBST([HAVE_WINT_T])

  if test $ac_cv_header_wctype_h = yes; then
    gl_ABSOLUTE_HEADER([wctype.h])
    ABSOLUTE_WCTYPE_H=\"$gl_cv_absolute_wctype_h\"
    HAVE_WCTYPE_H=1
  else
    ABSOLUTE_WCTYPE_H=\"no/such/file/wctype.h\"
    HAVE_WCTYPE_H=0
  fi
  AC_SUBST([ABSOLUTE_WCTYPE_H])
  AC_SUBST([HAVE_WCTYPE_H])

  WCTYPE_H=wctype.h
  HAVE_WCTYPE_CTMP_BUG=0
  if test $ac_cv_header_wctype_h = yes; then
    dnl IRIX 5.3 has a bug: its isw* macros reference an undefined variable
    dnl _ctmp_. Test against this bug.
    AC_CACHE_CHECK([whether wctype macros need _ctmp_ declared],
      [gl_cv_wctype_ctmp_bug],
      [gl_cv_wctype_ctmp_bug=no
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
	    [[#include <wctype.h>
	    ]],
	    [[return iswprint (0);]])],
	 [gl_cv_wctype_ctmp_bug='no, but bare wctype.h does not work'],
	 [AC_COMPILE_IFELSE(
	    [AC_LANG_PROGRAM(
	       [[#include <wctype.h>
		 static wint_t _ctmp_;]],
	       [[return iswprint (0);]])],
	    [gl_cv_wctype_ctmp_bug=yes])])])
    case $gl_cv_wctype_ctmp_bug,$ac_cv_func_iswcntrl in #(
      yes,*)
	HAVE_WCTYPE_CTMP_BUG=1;; #(
      no,yes)
	WCTYPE_H=;;
    esac
  fi
  AC_SUBST([WCTYPE_H])
  AC_SUBST([HAVE_WCTYPE_CTMP_BUG])
])
