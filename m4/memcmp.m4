# memcmp.m4 serial 16
dnl Copyright (C) 2002-2004, 2007-2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MEMCMP],
[
  if test $cross_compiling != no; then
    # The test below defaults to 'no' when cross compiling.
    # We default to yes if memcmp appears to exist, which works
    # better for MinGW.
    AC_CACHE_CHECK([whether cross-compiling target has memcmp],
                   [gl_cv_func_memcmp_working],
                   [AC_LINK_IFELSE([
                        AC_LANG_PROGRAM([[#include <string.h>
                                ]], [[int ret = memcmp ("foo", "bar", 0);]])],
                        [gl_cv_func_memcmp_working=yes],
                        [gl_cv_func_memcmp_working=no])])
  fi
  dnl We don't use AC_FUNC_MEMCMP any more, because it is no longer maintained
  dnl in Autoconf and because it invokes AC_LIBOBJ.
  AC_CACHE_CHECK([for working memcmp], [gl_cv_func_memcmp_working],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [AC_INCLUDES_DEFAULT],
          [[/* Some versions of memcmp are not 8-bit clean.  */
            char c0 = '\100', c1 = '\200', c2 = '\201';
            if (memcmp(&c0, &c2, 1) >= 0 || memcmp(&c1, &c2, 1) >= 0)
              return 1;
            /* The Next x86 OpenStep bug shows up only when comparing 16 bytes
               or more and with at least one buffer not starting on a 4-byte
               boundary.  William Lewis provided this test program.   */
            {
              char foo[21];
              char bar[21];
              int i;
              for (i = 0; i < 4; i++)
                {
                  char *a = foo + i;
                  char *b = bar + i;
                  strcpy (a, "--------01111111");
                  strcpy (b, "--------10000000");
                  if (memcmp (a, b, 16) >= 0)
                    return 1;
                }
              return 0;
            }
          ]])],
       [gl_cv_func_memcmp_working=yes],
       [gl_cv_func_memcmp_working=no],
       [gl_cv_func_memcmp_working=no])])
  if test $gl_cv_func_memcmp_working = no; then
    AC_DEFINE([memcmp], [rpl_memcmp],
      [Define to rpl_memcmp if the replacement function should be used.])
  fi
])

# Prerequisites of lib/memcmp.c.
AC_DEFUN([gl_PREREQ_MEMCMP], [:])
