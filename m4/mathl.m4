# mathl.m4 serial 5
dnl Copyright (c) 2003, 2007, 2009, 2010 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_FUNC_LONG_DOUBLE_MATH], [

AC_CHECK_LIB([m], [atan])
AC_REPLACE_FUNCS(floorl ceill sqrtl asinl acosl atanl \
        logl expl tanl sinl cosl)

])
