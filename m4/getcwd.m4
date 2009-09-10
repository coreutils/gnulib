# getcwd.m4 - check for working getcwd that is compatible with glibc

# Copyright (C) 2001, 2003, 2004, 2005, 2006, 2007, 2009 Free Software
# Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.
# serial 2

AC_DEFUN([gl_FUNC_GETCWD_NULL],
  [
   AC_CACHE_CHECK([whether getcwd (NULL, 0) allocates memory for result],
     [gl_cv_func_getcwd_null],
     [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#	 include <unistd.h>
#	 ifndef getcwd
	 char *getcwd ();
#	 endif
]], [[
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* mingw cwd does not start with '/', but getcwd does allocate.  */
#else
	   if (chdir ("/") != 0)
	     return 1;
	   else
	     {
	       char *f = getcwd (NULL, 0);
	       return ! (f && f[0] == '/' && !f[1]);
	     }
#endif
	 ]])],
	[gl_cv_func_getcwd_null=yes],
	[gl_cv_func_getcwd_null=no],
	[[
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_getcwd_null="guessing yes";;
                               # Guess yes on Cygwin.
         cygwin*)              gl_cv_func_getcwd_null="guessing yes";;
                               # Guess yes on mingw.
         mingw*)               gl_cv_func_getcwd_null="guessing yes";;
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_getcwd_null="guessing no";;
       esac
	]])])
])

AC_DEFUN([gl_FUNC_GETCWD],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_GETCWD_NULL])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles

  gl_abort_bug=no
  case $gl_cv_func_getcwd_null,$host_os in
  *,mingw*)
    gl_cv_func_getcwd_path_max=yes;;
  yes,*)
    gl_FUNC_GETCWD_PATH_MAX
    gl_FUNC_GETCWD_ABORT_BUG([gl_abort_bug=yes]);;
  esac

  case $gl_cv_func_getcwd_null,$gl_cv_func_getcwd_path_max,$gl_abort_bug in
  *yes,yes,no) ;;
  *)
    REPLACE_GETCWD=1
    AC_LIBOBJ([getcwd])
    gl_PREREQ_GETCWD;;
  esac
])

# Prerequisites of lib/getcwd.c.
AC_DEFUN([gl_PREREQ_GETCWD],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_CHECK_TYPE_STRUCT_DIRENT_D_INO])
  :
])
