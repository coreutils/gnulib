#serial 14

# Copyright (C) 2001, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# On some hosts (e.g., HP-UX 10.20, SunOS 4.1.4, Solaris 2.5.1), mkstemp has a
# silly limit that it can create no more than 26 files from a given template.
# Other systems lack mkstemp altogether.
# On OSF1/Tru64 V4.0F, the system-provided mkstemp function can create
# only 32 files per process.
# On systems like the above, arrange to use the replacement function.
AC_DEFUN([gl_FUNC_MKSTEMP],
[
  AC_REQUIRE([AC_SYS_LARGEFILE])

  AC_CACHE_CHECK([for working mkstemp],
    [gl_cv_func_working_mkstemp],
    [
      mkdir conftest.mkstemp
      AC_RUN_IFELSE(
	[AC_LANG_PROGRAM(
	  [AC_INCLUDES_DEFAULT],
	  [[int i;
	    off_t large = (off_t) 4294967295u;
	    if (large < 0)
	      large = 2147483647;
	    for (i = 0; i < 70; i++)
	      {
		char template[] = "conftest.mkstemp/coXXXXXX";
		int (*mkstemp_function) (char *) = mkstemp;
		int fd = mkstemp_function (template);
		if (fd < 0 || lseek (fd, large, SEEK_SET) != large)
		  return 1;
		close (fd);
	      }
	    return 0;]])],
	[gl_cv_func_working_mkstemp=yes],
	[gl_cv_func_working_mkstemp=no],
	[gl_cv_func_working_mkstemp=no])
      rm -rf conftest.mkstemp
    ])

  if test $gl_cv_func_working_mkstemp != yes; then
    AC_DEFINE([__MKSTEMP_PREFIX], [[rpl_]],
      [Define to rpl_ if the mkstemp replacement function should be used.])
    AC_LIBOBJ(mkstemp)
    AC_LIBOBJ(tempname)
    gl_PREREQ_MKSTEMP
    gl_PREREQ_TEMPNAME
  fi
])

# Prerequisites of lib/mkstemp.c.
AC_DEFUN([gl_PREREQ_MKSTEMP],
[
])

# Prerequisites of lib/tempname.c.
AC_DEFUN([gl_PREREQ_TEMPNAME],
[
  AC_CHECK_HEADERS_ONCE(sys/time.h)
  AC_CHECK_FUNCS(__secure_getenv gettimeofday)
])
