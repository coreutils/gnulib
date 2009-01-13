# serial 14

# See if we need to emulate a missing ftruncate function using fcntl or chsize.

# Copyright (C) 2000, 2001, 2003-2007, 2009 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# FIXME: remove this macro, along with all uses of HAVE_FTRUNCATE in 2010,
# if the check below provokes no more reports.  So far, the only report
# arose from a test build of this gnulib module, cross-compiling to mingw:
# <http://thread.gmane.org/gmane.comp.lib.gnulib.bugs/9203>

AC_DEFUN([gl_FUNC_FTRUNCATE],
[
  AC_REQUIRE([gl_UNISTD_H_DEFAULTS])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CHECK_FUNCS_ONCE([ftruncate])
  if test $ac_cv_func_ftruncate = no; then
    HAVE_FTRUNCATE=0
    AC_LIBOBJ([ftruncate])
    gl_PREREQ_FTRUNCATE
    case "$host_os" in
      mingw*)
        # Yes, we know mingw lacks ftruncate.
        ;;
      *)
        # If someone lacks ftruncate, make configure fail, and request
        # a bug report to inform us about it.
        if test x"$SKIP_FTRUNCATE_CHECK" != xyes; then
          AC_MSG_FAILURE([Your system lacks the ftruncate function.
	      Please report this, along with the output of "uname -a", to the
	      bug-coreutils@gnu.org mailing list.  To continue past this point,
	      rerun configure with SKIP_FTRUNCATE_CHECK=yes.
	      E.g., ./configure SKIP_FTRUNCATE_CHECK=yes])
        fi
    esac
  fi
])

# Prerequisites of lib/ftruncate.c.
AC_DEFUN([gl_PREREQ_FTRUNCATE],
[
  AC_CHECK_FUNCS([chsize])
])
