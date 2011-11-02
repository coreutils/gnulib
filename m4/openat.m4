# serial 40
# See if we need to use our replacement for Solaris' openat et al functions.

dnl Copyright (C) 2004-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Jim Meyering.

AC_DEFUN([gl_FUNC_OPENAT],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  GNULIB_OPENAT=1

  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  GNULIB_FSTATAT=1
  GNULIB_MKDIRAT=1

  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([mkdirat openat])
  AC_REQUIRE([gl_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK])
  case $ac_cv_func_openat+$gl_cv_func_lstat_dereferences_slashed_symlink in
  yes+yes)
    ;;
  yes+*)
    # Solaris 9 has *at functions, but uniformly mishandles trailing
    # slash in all of them.
    REPLACE_OPENAT=1
    ;;
  *)
    HAVE_OPENAT=0
    gl_PREREQ_OPENAT;;
  esac
  if test $ac_cv_func_mkdirat != yes; then
    HAVE_MKDIRAT=0
  fi
  gl_FUNC_FSTATAT

  dnl This is tested at least via getcwd.c.
  gl_MODULE_INDICATOR([openat])
])

# If we have the fstatat function, and it has the bug (in AIX 7.1)
# that it does not fill in st_size correctly, use the replacement function.
AC_DEFUN([gl_FUNC_FSTATAT],
[
  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_REQUIRE([gl_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK])
  AC_CHECK_FUNCS_ONCE([fstatat])

  if test $ac_cv_func_fstatat = no; then
    HAVE_FSTATAT=0
  else
    dnl Test for an AIX 7.1 bug; see
    dnl <http://lists.gnu.org/archive/html/bug-tar/2011-09/msg00015.html>.
    AC_CACHE_CHECK([whether fstatat (..., 0) works],
      [gl_cv_func_fstatat_zero_flag],
      [gl_cv_func_fstatat_zero_flag=no
       AC_RUN_IFELSE(
         [AC_LANG_SOURCE(
            [[
              #include <fcntl.h>
              #include <sys/stat.h>
              int
              main (void)
              {
                struct stat a;
                return fstatat (AT_FDCWD, ".", &a, 0) != 0;
              }
            ]])],
         [gl_cv_func_fstatat_zero_flag=yes])])

    case $gl_cv_func_fstatat_zero_flag+$gl_cv_func_lstat_dereferences_slashed_symlink in
    yes+yes) ;;
    *) REPLACE_FSTATAT=1
       if test $gl_cv_func_fstatat_zero_flag != yes; then
         AC_DEFINE([FSTATAT_ZERO_FLAG_BROKEN], [1],
           [Define to 1 if fstatat (..., 0) does not work,
            as in AIX 7.1.])
       fi
       ;;
    esac
  fi
])

AC_DEFUN([gl_PREREQ_OPENAT],
[
  AC_REQUIRE([AC_C_INLINE])
  AC_REQUIRE([gl_PROMOTED_TYPE_MODE_T])
  :
])
