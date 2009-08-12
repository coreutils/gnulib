# getopt.m4 serial 20
dnl Copyright (C) 2002-2006, 2008-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# The getopt module assume you want GNU getopt, with getopt_long etc,
# rather than vanilla POSIX getopt.  This means your code should
# always include <getopt.h> for the getopt prototypes.

# This is gnulib's entry-point.
AC_DEFUN([gl_GETOPT],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  gl_GETOPT_IFELSE([
    gl_REPLACE_GETOPT
  ])
])

# Request the gnulib implementation of the getopt functions unconditionally.
# argp.m4 uses this.
AC_DEFUN([gl_REPLACE_GETOPT],
[
  dnl Arrange for getopt.h to be created.
  gl_GETOPT_SUBSTITUTE_HEADER
  dnl Arrange for unistd.h to include getopt.h.
  GNULIB_UNISTD_H_GETOPT=1
  dnl Arrange to compile the getopt implementation.
  AC_LIBOBJ([getopt])
  AC_LIBOBJ([getopt1])
  gl_PREREQ_GETOPT
])

# emacs' configure.in uses this.
AC_DEFUN([gl_GETOPT_IFELSE],
[
  AC_REQUIRE([gl_GETOPT_CHECK_HEADERS])
  AS_IF([test -n "$gl_replace_getopt"], [$1], [$2])
])

# Determine whether to replace the entire getopt facility.
AC_DEFUN([gl_GETOPT_CHECK_HEADERS],
[
  dnl Persuade Solaris <unistd.h> to declare optarg, optind, opterr, optopt.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  gl_replace_getopt=
  if test -z "$gl_replace_getopt"; then
    AC_CHECK_HEADERS([getopt.h], [], [gl_replace_getopt=yes])
  fi

  if test -z "$gl_replace_getopt"; then
    AC_CHECK_FUNCS([getopt_long_only], [], [gl_replace_getopt=yes])
  fi

  dnl BSD getopt_long uses an incompatible method to reset option processing,
  dnl and (as of 2004-10-15) mishandles optional option-arguments.
  if test -z "$gl_replace_getopt"; then
    AC_CHECK_DECL([optreset], [gl_replace_getopt=yes], [],
      [#include <getopt.h>])
  fi

  dnl Solaris 10 getopt doesn't handle `+' as a leading character in an
  dnl option string (as of 2005-05-05).
  if test -z "$gl_replace_getopt"; then
    AC_CACHE_CHECK([for working GNU getopt function], [gl_cv_func_gnu_getopt],
      [AC_RUN_IFELSE(
	[AC_LANG_PROGRAM([[#include <getopt.h>]],
	   [[
	     char *myargv[3];
	     myargv[0] = "conftest";
	     myargv[1] = "-+";
	     myargv[2] = 0;
	     return getopt (2, myargv, "+a") != '?';
	   ]])],
	[gl_cv_func_gnu_getopt=yes],
	[gl_cv_func_gnu_getopt=no],
	[dnl cross compiling - pessimistically guess based on decls
	 dnl Solaris 10 getopt doesn't handle `+' as a leading character in an
	 dnl option string (as of 2005-05-05).
	 AC_CHECK_DECL([getopt_clip],
	   [gl_cv_func_gnu_getopt=no], [gl_cv_func_gnu_getopt=yes],
	   [#include <getopt.h>])])])
    if test "$gl_cv_func_gnu_getopt" = "no"; then
      gl_replace_getopt=yes
    fi
  fi
])

# emacs' configure.in uses this.
AC_DEFUN([gl_GETOPT_SUBSTITUTE_HEADER],
[
  GETOPT_H=getopt.h
  AC_DEFINE([__GETOPT_PREFIX], [[rpl_]],
    [Define to rpl_ if the getopt replacement functions and variables
     should be used.])
  AC_SUBST([GETOPT_H])
])

# Prerequisites of lib/getopt*.
# emacs' configure.in uses this.
AC_DEFUN([gl_PREREQ_GETOPT],
[
  AC_CHECK_DECLS_ONCE([getenv])
])
