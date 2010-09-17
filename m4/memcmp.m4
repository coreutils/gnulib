# memcmp.m4 serial 15
dnl Copyright (C) 2002-2004, 2007-2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_MEMCMP],
[
  if test $cross_compiling != no; then
    # AC_FUNC_MEMCMP as of 2.62 defaults to 'no' when cross compiling.
    # We default to yes if memcmp appears to exist, which works
    # better for MinGW.
    AC_CACHE_CHECK([whether cross-compiling target has memcmp],
                   [ac_cv_func_memcmp_working],
                   [AC_LINK_IFELSE([
                        AC_LANG_PROGRAM([[#include <string.h>
                                ]], [[int ret = memcmp ("foo", "bar", 0);]])],
                        [ac_cv_func_memcmp_working=yes],
                        [ac_cv_func_memcmp_working=no])])
  fi
  AC_FUNC_MEMCMP
  dnl Note: AC_FUNC_MEMCMP does AC_LIBOBJ([memcmp]).
  if test $ac_cv_func_memcmp_working = no; then
    AC_DEFINE([memcmp], [rpl_memcmp],
      [Define to rpl_memcmp if the replacement function should be used.])
    gl_PREREQ_MEMCMP
  fi
])

# Redefine AC_FUNC_MEMCMP, because it is no longer maintained in Autoconf.
AC_DEFUN([AC_FUNC_MEMCMP],
[
  AC_CACHE_CHECK([for working memcmp], [ac_cv_func_memcmp_working],
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
       [ac_cv_func_memcmp_working=yes],
       [ac_cv_func_memcmp_working=no],
       [ac_cv_func_memcmp_working=no])])
  test $ac_cv_func_memcmp_working = no &&
    AC_LIBOBJ([memcmp])
])

# Prerequisites of lib/memcmp.c.
AC_DEFUN([gl_PREREQ_MEMCMP], [:])
