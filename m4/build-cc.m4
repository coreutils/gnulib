# build-cc.m4
# serial 1
dnl Copyright (C) 2024-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Written by Bruno Haible.

dnl When the build environment ($build_os) is different from the target runtime
dnl environment ($host_os), a build step that requires execution of the built
dnl program must use a native compiler.
dnl In order to specify this compiler, the user can specify the following
dnl variables (as environment variables or as assignment arguments to
dnl 'configure'):
dnl   - BUILD_CC       - the C compiler
dnl   - BUILD_CPPFLAGS - compiler options used during preprocessing
dnl   - BUILD_CFLAGS   - compiler options used during compilation to object code
dnl   - BUILD_LDFLAGS  - compiler options used durint linking
dnl These are the same conventions as used e.g. by glibc.
dnl
dnl gl_BUILD_CC provides these variables.
dnl If not cross-compiling, the values are the same as those of
dnl CC, CPPFLAGS, CFLAGS, LDFLAGS respectively (so that the user does not have
dnl to specify them twice).
dnl If no native compiler was found, BUILD_CC is set to empty.
dnl
dnl This macro is intentionally simple.
dnl If you need things like BUILD_OBJEXT, BUILD_EXEEXT, etc., consider using
dnl AX_PROG_CC_FOR_BUILD from the Autoconf Archive.

AC_DEFUN_ONCE([gl_BUILD_CC],
[
  AC_REQUIRE([AC_PROG_CC])
  dnl $cross_compiling is 'yes' if executables created by the C compiler $CC
  dnl can be run in the build environment, or 'no' otherwise.
  if test $cross_compiling = yes; then
    if test -z "$BUILD_CC"; then
      dnl If the user has not specified BUILD_CC, try gcc, then cc.
      AC_CHECK_PROGS([BUILD_CC], [gcc cc])
    fi
  else
    BUILD_CC="$CC"
    BUILD_CPPFLAGS="$CPPFLAGS"
    BUILD_CFLAGS="$CFLAGS"
    BUILD_LDFLAGS="$LDFLAGS"
  fi
  AC_SUBST([BUILD_CC])
  AC_SUBST([BUILD_CPPFLAGS])
  AC_SUBST([BUILD_CFLAGS])
  AC_SUBST([BUILD_LDFLAGS])
])
