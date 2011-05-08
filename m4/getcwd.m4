# getcwd.m4 - check for working getcwd that is compatible with glibc

# Copyright (C) 2001, 2003-2007, 2009-2011 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.
# serial 7

AC_DEFUN([gl_FUNC_GETCWD_NULL],
  [
   AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
   AC_CACHE_CHECK([whether getcwd (NULL, 0) allocates memory for result],
     [gl_cv_func_getcwd_null],
     [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#        include <unistd.h>
#        ifndef getcwd
         char *getcwd ();
#        endif
]], [[
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/* mingw cwd does not start with '/', but getcwd does allocate.
   However, mingw fails to honor non-zero size.  */
#else
           if (chdir ("/") != 0)
             return 1;
           else
             {
               char *f = getcwd (NULL, 0);
               if (! f)
                 return 2;
               if (f[0] != '/')
                 return 3;
               if (f[1] != '\0')
                 return 4;
               return 0;
             }
#endif
         /* If size is non-zero, allocation must fail if size is too small */
         if (getcwd (NULL, 1))
           return 5;
         ]])],
        [gl_cv_func_getcwd_null=yes],
        [gl_cv_func_getcwd_null=no],
        [[
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_getcwd_null="guessing yes";;
                               # Guess yes on Cygwin.
         cygwin*)              gl_cv_func_getcwd_null="guessing yes";;
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_getcwd_null="guessing no";;
       esac
        ]])])
])


dnl Guarantee that getcwd will malloc with a NULL first argument.  Assumes
dnl that either the system getcwd is robust, or that calling code is okay
dnl with spurious failures when run from a directory with an absolute name
dnl larger than 4k bytes.
dnl
dnl Assumes that getcwd exists; if you are worried about obsolete
dnl platforms that lacked getcwd(), then you need to use the GPL module.
AC_DEFUN([gl_FUNC_GETCWD_LGPL],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([gl_FUNC_GETCWD_NULL])

  case $gl_cv_func_getcwd_null in
  *yes) ;;
  *)
    dnl Minimal replacement lib/getcwd-lgpl.c.
    REPLACE_GETCWD=1
    ;;
  esac
])

dnl Check for all known getcwd bugs; useful for a program likely to be
dnl executed from an arbitrary location.
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
    dnl Full replacement lib/getcwd.c, overrides LGPL replacement.
    REPLACE_GETCWD=1;;
  esac
])

# Prerequisites of lib/getcwd.c, when full replacement is in effect.
AC_DEFUN([gl_PREREQ_GETCWD],
[
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_CHECK_TYPE_STRUCT_DIRENT_D_INO])
  :
])
