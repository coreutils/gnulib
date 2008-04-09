# acl.m4 - check for access control list (ACL) primitives
# serial 2

# Copyright (C) 2002, 2004-2008 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert and Jim Meyering.

AC_DEFUN([gl_FUNC_ACL],
[
  AC_LIBOBJ([acl])
  AC_LIBOBJ([file-has-acl])

  AC_ARG_ENABLE([acl],
    AS_HELP_STRING([--disable-acl], [do not support ACLs]),
    , [enable_acl=auto])

  LIB_ACL=
  use_acl=0
  if test "x$enable_acl" != "xno"; then
    dnl Prerequisites of lib/acl.c.
    AC_CHECK_HEADERS(sys/acl.h)
    if test $ac_cv_header_sys_acl_h = yes; then
      ac_save_LIBS=$LIBS
      AC_CHECK_FUNCS([acl])
      use_acl=1
      AC_SEARCH_LIBS([acl_trivial], [sec],
	[test "$ac_cv_search_acl_trivial" = "none required" ||
	 LIB_ACL=$ac_cv_search_acl_trivial
	 AC_CHECK_FUNCS([acl_trivial])],
	[AC_CHECK_FUNCS([acl_trivial])
	 if test $ac_cv_func_acl_trivial != yes; then
	   AC_SEARCH_LIBS([acl_get_file], [acl],
	     [test "$ac_cv_search_acl_get_file" = "none required" ||
	      LIB_ACL=$ac_cv_search_acl_get_file
	      AC_CHECK_FUNCS(
		[acl_get_file acl_get_fd acl_set_file acl_set_fd \
		 acl_free acl_from_mode acl_from_text \
		 acl_delete_def_file acl_extended_file])
	      if test $ac_cv_func_acl_get_file = yes; then
		# If the acl_get_file bug is detected, disable all ACL support.
		gl_ACL_GET_FILE( , [use_acl=0])
	      fi
	      if test $use_acl = 1; then
		AC_CHECK_HEADERS([acl/libacl.h])
		if test $ac_cv_func_acl_get_file = yes &&
		   test $ac_cv_func_acl_free = yes; then
		  AC_REPLACE_FUNCS([acl_entries])
		fi
	      else
		LIB_ACL=
	      fi])
	 fi])
      LIBS=$ac_save_LIBS
    fi
    if test "x$enable_acl$use_acl" = "xyes0"; then
      AC_MSG_ERROR([ACLs enabled but support not detected])
    fi
  fi
  AC_SUBST([LIB_ACL])
  AC_DEFINE_UNQUOTED([USE_ACL], [$use_acl],
    [Define to nonzero if you want access control list support.])

  # This is for backwards compatibility; remove this by the end of 2007.
  LIB_ACL_TRIVIAL=
  AC_SUBST([LIB_ACL_TRIVIAL])
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
