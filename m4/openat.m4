# serial 21
# See if we need to use our replacement for Solaris' openat et al functions.

dnl Copyright (C) 2004-2009 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Written by Jim Meyering.

AC_DEFUN([gl_FUNC_OPENAT],
[
  AC_REQUIRE([gl_FCNTL_H_DEFAULTS])
  GNULIB_OPENAT=1

  AC_REQUIRE([gl_SYS_STAT_H_DEFAULTS])
  GNULIB_FCHMODAT=1
  GNULIB_FSTATAT=1
  GNULIB_MKDIRAT=1

  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  GNULIB_FCHOWNAT=1
  GNULIB_UNLINKAT=1

  AC_LIBOBJ([openat-proc])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  AC_CHECK_FUNCS_ONCE([lchmod])
  AC_REPLACE_FUNCS([fchmodat mkdirat openat])
  AC_REQUIRE([AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK])
  case $ac_cv_func_openat+$ac_cv_func_lstat_dereferences_slashed_symlink in
  yes+yes) ;;
  yes+*)
    AC_LIBOBJ([fstatat])
    REPLACE_FSTATAT=1
    ;;
  *)
    HAVE_OPENAT=0
    HAVE_UNLINKAT=0 # No known system with unlinkat but not openat
    HAVE_FSTATAT=0 # No known system with fstatat but not openat
    gl_PREREQ_OPENAT;;
  esac
  if test $ac_cv_func_fchmodat != yes; then
    HAVE_FCHMODAT=0
  fi
  if test $ac_cv_func_mkdirat != yes; then
    HAVE_MKDIRAT=0
  fi
  gl_FUNC_FCHOWNAT
])

# gl_FUNC_FCHOWNAT_DEREF_BUG([ACTION-IF-BUGGY[, ACTION-IF-NOT_BUGGY]])
AC_DEFUN([gl_FUNC_FCHOWNAT_DEREF_BUG],
[
  AC_CACHE_CHECK([whether fchownat works with AT_SYMLINK_NOFOLLOW],
    gl_cv_func_fchownat_nofollow_works,
    [
     gl_dangle=conftest.dangle
     # Remove any remnants of a previous test.
     rm -f $gl_dangle
     # Arrange for deletion of the temporary file this test creates.
     ac_clean_files="$ac_clean_files $gl_dangle"
     ln -s conftest.no-such $gl_dangle
     AC_RUN_IFELSE(
       [AC_LANG_SOURCE(
	  [[
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
int
main ()
{
  return (fchownat (AT_FDCWD, "$gl_dangle", -1, getgid (),
		    AT_SYMLINK_NOFOLLOW) != 0
	  && errno == ENOENT);
}
	  ]])],
    [gl_cv_func_fchownat_nofollow_works=yes],
    [gl_cv_func_fchownat_nofollow_works=no],
    [gl_cv_func_fchownat_nofollow_works=no],
    )
  ])
  AS_IF([test $gl_cv_func_fchownat_nofollow_works = no], [$1], [$2])
])

# If we have the fchownat function, and it has the bug (in glibc-2.4)
# that it dereferences symlinks even with AT_SYMLINK_NOFOLLOW, then
# use the replacement function.
# Also use the replacement function if fchownat is simply not available.
AC_DEFUN([gl_FUNC_FCHOWNAT],
[
  AC_CHECK_FUNC([fchownat],
    [gl_FUNC_FCHOWNAT_DEREF_BUG([REPLACE_FCHOWNAT=1])],
    [HAVE_FCHOWNAT=0])
  if test $HAVE_FCHOWNAT = 0 || test $REPLACE_FCHOWNAT = 1; then
    AC_LIBOBJ([fchownat])
  fi
])

AC_DEFUN([gl_PREREQ_OPENAT],
[
  AC_REQUIRE([gl_PROMOTED_TYPE_MODE_T])
  :
])
