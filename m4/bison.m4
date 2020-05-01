# serial 8

# Copyright (C) 2002-2006, 2008-2020 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# There are two types of parser skeletons:
#
# * Those that can be used with any Yacc implementation, including bison.
#   For these, in the configure.ac, up to Autoconf 2.69, you could use
#     AC_PROG_YACC
#   In newer Autoconf versions, however, this macro is broken. See
#     https://lists.gnu.org/archive/html/autoconf-patches/2013-03/msg00000.html
#     https://lists.gnu.org/archive/html/bug-autoconf/2018-12/msg00001.html
#   In the Makefile.am you could use
#     $(SHELL) $(YLWRAP) $(srcdir)/foo.y \
#                        y.tab.c foo.c \
#                        y.tab.h foo.h \
#                        y.output foo.output \
#                        -- $(YACC) $(YFLAGS) $(AM_YFLAGS)
#   or similar.
#
# * Those that make use of Bison extensions. For example,
#     - %define api.pure   requires bison 2.7 or newer,
#     - %precedence        requires bison 3.0 or newer.
#   For these, in the configure.ac you will need an invocation of
#     gl_PROG_BISON([VARIABLE], [MIN_BISON_VERSION], [BISON_VERSIONS_TO_EXCLUDE])
#   Example:
#     gl_PROG_BISON([PARSE_DATETIME_BISON], [2.4], [1.* | 2.[0-3] | 2.[0-3].*])
#   With this preparation, in the Makefile.am there are two ways to formulate
#   the invocation. Both are direct, without use of 'ylwrap'.
#   (a) You can invoke
#         $(VARIABLE) -d $(SOME_BISON_OPTIONS) --output foo.c $(srcdir)/foo.y
#       or similar.
#   (b) If you want the invocation to honor an YFLAGS=... parameter passed to
#       'configure' or an YFLAGS environment variable present at 'configure'
#       time, add an invocation of gl_BISON to the configure.ac, and write
#         $(VARIABLE) -d $(YFLAGS) $(AM_YFLAGS) $(srcdir)/foo.y
#       or similar.

# This macro defines the autoconf variable VARIABLE to 'bison' if the specified
# minimum version of bison is found in $PATH, or to ':' otherwise.
AC_DEFUN([gl_PROG_BISON],
[
  AC_CHECK_PROGS([$1], [bison])
  if test -z "$[$1]"; then
    ac_verc_fail=yes
  else
    dnl Found it, now check the version.
    AC_MSG_CHECKING([version of bison])
changequote(<<,>>)dnl
    ac_prog_version=`$<<$1>> --version 2>&1 | sed -n 's/^.*GNU Bison.* \([0-9]*\.[0-9.]*\).*$/\1/p'`
    case $ac_prog_version in
      '') ac_prog_version="v. ?.??, bad"; ac_verc_fail=yes;;
      <<$3>>)
         ac_prog_version="$ac_prog_version, bad"; ac_verc_fail=yes;;
      *) ac_prog_version="$ac_prog_version, ok"; ac_verc_fail=no;;
    esac
changequote([,])dnl
    AC_MSG_RESULT([$ac_prog_version])
  fi
  if test $ac_verc_fail = yes; then
    [$1]=:
  fi
  AC_SUBST([$1])
])

# This macro sets the autoconf variables YACC (for old-style yacc Makefile
# rules) and YFLAGS (to allow options to be passed as 'configure' time).
AC_DEFUN([gl_BISON],
[
  : ${YACC='bison -o y.tab.c'}
dnl
dnl Declaring YACC & YFLAGS precious will not be necessary after GNULIB
dnl requires an Autoconf greater than 2.59c, but it will probably still be
dnl useful to override the description of YACC in the --help output, re
dnl parse-datetime.y assuming 'bison -o y.tab.c'.
  AC_ARG_VAR([YACC],
[The "Yet Another C Compiler" implementation to use.  Defaults to
'bison -o y.tab.c'.  Values other than 'bison -o y.tab.c' will most likely
break on most systems.])dnl
  AC_ARG_VAR([YFLAGS],
[YFLAGS contains the list arguments that will be passed by default to Bison.
This script will default YFLAGS to the empty string to avoid a default value of
'-d' given by some make applications.])dnl
])
