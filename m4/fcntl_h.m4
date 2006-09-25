# Configure fcntl.h.
dnl Copyright (C) 2006 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Written by Paul Eggert.

AC_DEFUN([gl_FCNTL_H],
[
  AC_CACHE_CHECK([for working fcntl.h], gl_cv_header_working_fcntl_h,
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
	  [[#include <sys/types.h>
	   #include <sys/stat.h>
	   #include <unistd.h>
	   #include <fcntl.h>
	   #ifndef O_NOFOLLOW
	    #define O_NOFOLLOW 0
	   #endif
	   static int const constants[] =
	    {
	      O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC, O_APPEND,
	      O_NONBLOCK, O_SYNC, O_ACCMODE, O_RDONLY, O_RDWR, O_WRONLY
	    };
	  ]],
	  [[static char const sym[] = "conftest.sym";
	    if (O_NOFOLLOW)
	      {
		if (symlink (".", sym) != 0)
		  return 1;
		if (0 <= open (sym, O_RDONLY | O_NOFOLLOW))
		  return 1;
	      }
	    return !constants;]])],
       [gl_cv_header_working_fcntl_h=yes],
       [gl_cv_header_working_fcntl_h=no],
       [gl_cv_header_working_fcntl_h=cross-compiling])])

  if test $gl_cv_header_working_fcntl_h != yes; then
    AC_DEFINE([O_NOFOLLOW_IS_INEFFECTIVE], 1,
      [Define to 1 if O_NOFOLLOW is ineffective.])
  fi

  gl_ABSOLUTE_HEADER([fcntl.h])
  ABSOLUTE_FCNTL_H=\"$gl_cv_absolute_fcntl_h\"
  AC_SUBST([ABSOLUTE_FCNTL_H])
  FCNTL_H='fcntl.h'
  AC_SUBST([FCNTL_H])
])
