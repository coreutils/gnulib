# Test for GCC-style __attribute__ support.

dnl Copyright (C) 2011 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl from Paul Eggert

# Currently only __attribute__ ((__noreturn__)) is done,
# but other macros can be added to this file as needed.

# Test whether __attribute__ ((__noreturn__)) works.
AC_DEFUN([gl_ATTRIBUTE_NORETURN], [
  AC_CACHE_CHECK([for  __attribute__ ((__noreturn__))],
    [gl_cv_attribute_noreturn],
    [gl_save_werror_flag=$ac_[]_AC_LANG_ABBREV[]_werror_flag
     ac_[]_AC_LANG_ABBREV[]_werror_flag=yes
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM(
          [[void never_come_back (int) __attribute__ ((__noreturn__));]])],
       [gl_cv_attribute_noreturn=yes],
       [gl_cv_attribute_noreturn=no])
     ac_[]_AC_LANG_ABBREV[]_werror_flag=$gl_save_werror_flag])
  if test $gl_cv_attribute_noreturn = no; then
    HAVE_ATTRIBUTE_NORETURN=0
  fi
  AC_SUBST([HAVE_ATTRIBUTE_NORETURN])
])
