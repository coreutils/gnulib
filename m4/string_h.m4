# Configure a GNU-like replacement for <string.h>.

# Copyright (C) 2007 Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# Written by Paul Eggert.

AC_DEFUN([gl_HEADER_STRING_H],
[
  dnl Use AC_REQUIRE here, so that the default behavior below is expanded
  dnl once only, before all statements that occur in other macros.
  AC_REQUIRE([gl_HEADER_STRING_H_BODY])
])

AC_DEFUN([gl_HEADER_STRING_H_BODY],
[
  AC_REQUIRE([AC_C_RESTRICT])
  AC_REQUIRE([gl_HEADER_STRING_H_DEFAULTS])
  gl_ABSOLUTE_HEADER([string.h])
  ABSOLUTE_STRING_H=\"$gl_cv_absolute_string_h\"
  AC_SUBST([ABSOLUTE_STRING_H])
])

AC_DEFUN([gl_HEADER_STRING_H_DEFAULTS],
[
  dnl Assume proper GNU behavior unless another module says otherwise.
  HAVE_DECL_MEMMEM=1;		AC_SUBST([HAVE_DECL_MEMMEM])
  HAVE_MEMPCPY=1;		AC_SUBST([HAVE_MEMPCPY])
  HAVE_DECL_MEMRCHR=1;		AC_SUBST([HAVE_DECL_MEMRCHR])
  HAVE_STPCPY=1;		AC_SUBST([HAVE_STPCPY])
  HAVE_STPNCPY=1;		AC_SUBST([HAVE_STPNCPY])
  HAVE_DECL_STRNCASECMP=1;	AC_SUBST([HAVE_DECL_STRNCASECMP])
  HAVE_STRCHRNUL=1;		AC_SUBST([HAVE_STRCHRNUL])
  HAVE_DECL_STRDUP=1;		AC_SUBST([HAVE_DECL_STRDUP])
  HAVE_STRNDUP=1;		AC_SUBST([HAVE_STRNDUP])
  HAVE_DECL_STRNDUP=1;		AC_SUBST([HAVE_DECL_STRNDUP])
  HAVE_DECL_STRNLEN=1;		AC_SUBST([HAVE_DECL_STRNLEN])
  HAVE_STRPBRK=1;		AC_SUBST([HAVE_STRPBRK])
  HAVE_STRSEP=1;		AC_SUBST([HAVE_STRSEP])
  HAVE_DECL_STRTOK_R=1;		AC_SUBST([HAVE_DECL_STRTOK_R])
  REPLACE_STRCASECMP=0;		AC_SUBST([REPLACE_STRCASECMP])
  REPLACE_STRSTR=0;		AC_SUBST([REPLACE_STRSTR])
  REPLACE_STRCASESTR=0;		AC_SUBST([REPLACE_STRCASESTR])
])
