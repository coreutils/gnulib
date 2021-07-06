# year2038.m4 serial 5
dnl Copyright (C) 2017-2021 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Attempt to ensure that 'time_t' is a 64-bit type
dnl and that the functions time(), stat(), etc. return 64-bit times.

AC_DEFUN([gl_YEAR2038_EARLY],
[
  AC_REQUIRE([AC_CANONICAL_HOST])
  case "$host_os" in
    mingw*)
      AC_DEFINE([__MINGW_USE_VC2005_COMPAT], [1],
        [For 64-bit time_t on 32-bit mingw.])
      ;;
  esac
])

# gl_YEAR2038_TEST_INCLUDES
# -------------------------
AC_DEFUN([gl_YEAR2038_TEST_INCLUDES],
[[
  #include <time.h>
  /* Check that time_t can represent 2**63 - 1 correctly.
     We can't simply define LARGE_TIME_T to be 9223372036854775807,
     since some C++ compilers masquerading as C compilers
     incorrectly reject 9223372036854775807.  */
  #define LARGE_TIME_T (((time_t) 1 << 31 << 31) - 1 + ((time_t) 1 << 31 << 31))
  int verify_time_t_range[(LARGE_TIME_T % 2147483629 == 721
                           && LARGE_TIME_T % 2147483647 == 1)
                          ? 1 : -1];
]])

# gl_YEAR2038_BODY(REQUIRE-64-BIT)
----------------------------------
AC_DEFUN([gl_YEAR2038_BODY],
[
 AC_ARG_ENABLE([year2038],
   [  --disable-year2038      omit support for timestamps past the year 2038])
 AS_IF([test "$enable_year2038" != no],
 [
  dnl On many systems, time_t is already a 64-bit type.
  dnl On those systems where time_t is still 32-bit, it requires kernel
  dnl and libc support to make it 64-bit. For glibc 2.34 and later on Linux,
  dnl defining _TIME_BITS=64 and _FILE_OFFSET_BITS=64 is needed on x86 and ARM.
  dnl
  dnl On native Windows, the system include files define types __time32_t
  dnl and __time64_t. By default, time_t is an alias of
  dnl   - __time32_t on 32-bit mingw,
  dnl   - __time64_t on 64-bit mingw and on MSVC (since MSVC 8).
  dnl But when compiling with -D__MINGW_USE_VC2005_COMPAT, time_t is an
  dnl alias of __time64_t.
  dnl And when compiling with -D_USE_32BIT_TIME_T, time_t is an alias of
  dnl __time32_t.
  AC_CACHE_CHECK([for 64-bit time_t], [gl_cv_type_time_t_64],
    [AC_COMPILE_IFELSE(
       [AC_LANG_SOURCE([gl_YEAR2038_TEST_INCLUDES])],
       [gl_cv_type_time_t_64=yes], [gl_cv_type_time_t_64=no])
    ])
  if test "$gl_cv_type_time_t_64" = no; then
    AC_CACHE_CHECK([for 64-bit time_t with _TIME_BITS=64],
      [gl_cv_type_time_t_bits_macro],
      [AC_COMPILE_IFELSE(
         [AC_LANG_SOURCE([[#define _TIME_BITS 64
                           #define _FILE_OFFSET_BITS 64
                           ]gl_YEAR2038_TEST_INCLUDES])],
         [gl_cv_type_time_t_bits_macro=yes],
         [gl_cv_type_time_t_bits_macro=no])
      ])
    if test "$gl_cv_type_time_t_bits_macro" = yes; then
      AC_DEFINE([_TIME_BITS], [64],
        [Number of bits in a timestamp, on hosts where this is settable.])
      dnl AC_SYS_LARGFILE also defines this; it's OK if we do too.
      AC_DEFINE([_FILE_OFFSET_BITS], [64],
        [Number of bits in a file offset, on hosts where this is settable.])
      gl_cv_type_time_t_64=yes
    fi
  fi
  if test $gl_cv_type_time_t_64 = no; then
    AC_COMPILE_IFELSE(
      [AC_LANG_SOURCE(
         [[#ifdef _USE_32BIT_TIME_T
             int ok;
           #else
             error fail
           #endif
         ]])],
      [AC_MSG_FAILURE([This package requires a 64-bit 'time_t' type. Remove _USE_32BIT_TIME_T from the compiler flags.])],
      [# If not cross-compiling and $1 says we should check,
       # and 'touch' works with a large timestamp, then evidently 64-bit time_t
       # is desired and supported, so fail and ask the builder to fix the
       # problem.  Otherwise, just warn the builder.
       m4_ifval([$1],
         [if test $cross_compiling = no \
             && TZ=UTC0 touch -t 210602070628.16 conftest.time 2>/dev/null; then
            rm -f conftest.time
            AC_MSG_FAILURE([This package requires a 64-bit 'time_t' type, which your system appears to support. You might try configuring with 'CPPFLAGS="-m64" LDFLAGS="-m64"'. To build with a 32-bit time_t anyway (not recommended), configure with '--disable-year2038'.])
          fi])
       if test "$gl_warned_about_64_bit_time_t" != yes; then
         AC_MSG_WARN([This package requires a 64-bit 'time_t' type if there is any way to access timestamps outside the year range 1901-2038 on your platform. Perhaps you should configure with 'CPPFLAGS="-m64" LDFLAGS="-m64"'?])
         gl_warned_about_64_bit_time_t=yes
       fi
      ])
  fi])
])

AC_DEFUN([gl_YEAR2038],
[
  gl_YEAR2038_BODY([require-64-bit])
])
