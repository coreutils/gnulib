# acl.m4 - check for access control list (ACL) primitives

# Copyright (C) 2002, 2004, 2005, 2006, 2007 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert and Jim Meyering.

AC_DEFUN([AC_FUNC_ACL],
[
  AC_LIBOBJ([acl])
  AC_LIBOBJ([file-has-acl])

  dnl Prerequisites of lib/acl.c.
  AC_CHECK_HEADERS(sys/acl.h)
  AC_CHECK_FUNCS(acl)
  ac_save_LIBS="$LIBS"
    AC_SEARCH_LIBS(acl_get_file, acl,
		   [test "$ac_cv_search_acl_get_file" = "none required" ||
		    LIB_ACL=$ac_cv_search_acl_get_file])
    AC_SUBST(LIB_ACL)
    AC_CHECK_HEADERS(acl/libacl.h)
    AC_CHECK_FUNCS(acl_get_file acl_get_fd acl_set_file acl_set_fd \
		   acl_free acl_from_mode acl_from_text \
		   acl_delete_def_file acl_extended_file)
    if test $ac_cv_header_sys_acl_h = yes; then
      use_acl=1
      if test $ac_cv_func_acl_get_file = yes; then
	# If we detect the acl_get_file bug, disable ACL support altogether.
	gl_ACL_GET_FILE( , [use_acl=0])
      fi
    else
      use_acl=0
    fi
    if test $use_acl = 1 &&
       test $ac_cv_func_acl_get_file = yes &&
       test $ac_cv_func_acl_free = yes; then
      AC_REPLACE_FUNCS([acl_entries])
    fi
  LIBS="$ac_save_LIBS"
  if test $use_acl = 1; then
    ac_save_LIBS="$LIBS"
    AC_SEARCH_LIBS([acl_trivial], [sec],
      [AC_DEFINE([HAVE_ACL_TRIVIAL], 1,
	 [Define to 1 if you have the `acl_trivial' function.])
       test "$ac_cv_search_acl_trivial" = "none required" ||
       LIB_ACL_TRIVIAL="$ac_cv_search_acl_trivial"])
    AC_SUBST([LIB_ACL_TRIVIAL])
    LIBS="$ac_save_LIBS"
  fi
  AC_DEFINE_UNQUOTED(USE_ACL, $use_acl,
		     [Define if you want access control list support.])
])

# gl_ACL_GET_FILE(IF-WORKS, IF-NOT)
# -------------------------------------
# If `acl_get_file' works (does not have a particular bug),
# run IF-WORKS, otherwise, IF-NOT.
# This tests for a Darwin 8.7.0 bug, whereby acl_get_file returns NULL,
# but sets errno = ENOENT for an existing file or directory.
AC_DEFUN([gl_ACL_GET_FILE],
[
  AC_CACHE_CHECK([for working acl_get_file], gl_cv_func_working_acl_get_file,
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
	  [[#include <sys/types.h>
	   #include <sys/acl.h>
	   #include <errno.h>
	  ]],
	  [[return !! (!acl_get_file (".", ACL_TYPE_ACCESS)
		       && errno == ENOENT);]])],
       [gl_cv_func_working_acl_get_file=yes],
       [gl_cv_func_working_acl_get_file=no],
       [gl_cv_func_working_acl_get_file=cross-compiling])])

  AS_IF([test $gl_cv_func_working_acl_get_file = yes], [$1], [$2])
])
