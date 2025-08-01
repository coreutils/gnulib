# printf.m4
# serial 96
dnl Copyright (C) 2003, 2007-2025 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Test whether the *printf family of functions supports the 'j', 'z', 't',
dnl 'L' size specifiers. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_sizes_c99.

AC_DEFUN([gl_PRINTF_SIZES_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_AC_HEADER_STDINT_H])
  AC_REQUIRE([gl_AC_HEADER_INTTYPES_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports size specifiers as in C99],
    [gl_cv_func_printf_sizes_c99],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#if HAVE_STDINT_H_WITH_UINTMAX
# include <stdint.h>
#endif
#if HAVE_INTTYPES_H_WITH_UINTMAX
# include <inttypes.h>
#endif
static char buf[100];
int main ()
{
  int result = 0;
#if HAVE_STDINT_H_WITH_UINTMAX || HAVE_INTTYPES_H_WITH_UINTMAX
  buf[0] = '\0';
  if (sprintf (buf, "%ju %d", (uintmax_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 1;
#else
  result |= 1;
#endif
  buf[0] = '\0';
  if (sprintf (buf, "%zu %d", (size_t) 12345672, 33, 44, 55) < 0
      || strcmp (buf, "12345672 33") != 0)
    result |= 2;
  buf[0] = '\0';
  if (sprintf (buf, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55) < 0
      || strcmp (buf, "12345673 33") != 0)
    result |= 4;
  buf[0] = '\0';
  if (sprintf (buf, "%Lg %d", (long double) 1.5, 33, 44, 55) < 0
      || strcmp (buf, "1.5 33") != 0)
    result |= 8;
  return result;
}]])],
        [gl_cv_func_printf_sizes_c99=yes],
        [gl_cv_func_printf_sizes_c99=no],
        [
         case "$host_os" in
changequote(,)dnl
                                 # Guess yes on glibc systems.
           *-gnu* | gnu*)        gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on FreeBSD >= 5.
           freebsd[1-4].*)       gl_cv_func_printf_sizes_c99="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_printf_sizes_c99="guessing yes";;
           midnightbsd*)         gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_printf_sizes_c99="guessing no";;
           darwin*)              gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on OpenBSD >= 3.9.
           openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                                 gl_cv_func_printf_sizes_c99="guessing no";;
           openbsd*)             gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on Solaris >= 2.10.
           solaris2.[1-9][0-9]*) gl_cv_func_printf_sizes_c99="guessing yes";;
           solaris*)             gl_cv_func_printf_sizes_c99="guessing no";;
                                 # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                 gl_cv_func_printf_sizes_c99="guessing no";;
           netbsd*)              gl_cv_func_printf_sizes_c99="guessing yes";;
                                 # Guess yes on Android.
           linux*-android*)      gl_cv_func_printf_sizes_c99="guessing yes";;
changequote([,])dnl
                                 # Guess yes on MSVC, no on mingw.
           windows*-msvc*)       gl_cv_func_printf_sizes_c99="guessing yes" ;;
           mingw* | windows*)    AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                   ],
                                   [gl_cv_func_printf_sizes_c99="guessing yes"],
                                   [gl_cv_func_printf_sizes_c99="guessing no"])
                                 ;;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_printf_sizes_c99="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the 'w8', 'w16',
dnl 'w32', 'w64', 'wf8', 'wf16', 'wf32', 'wf64' size specifiers. (ISO C23)
dnl Result is gl_cv_func_printf_sizes_c23.

AC_DEFUN([gl_PRINTF_SIZES_C23],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_AC_HEADER_STDINT_H])
  AC_REQUIRE([gl_AC_HEADER_INTTYPES_H])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports size specifiers as in C23],
    [gl_cv_func_printf_sizes_c23],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#if HAVE_STDINT_H_WITH_UINTMAX
# include <stdint.h>
#endif
#if HAVE_INTTYPES_H_WITH_UINTMAX
# include <inttypes.h>
#endif
static char buf[100];
int main ()
{
  int result = 0;
  buf[0] = '\0';
  if (sprintf (buf, "%w8u %d", (uint8_t) 123, 33, 44, 55) < 0
      || strcmp (buf, "123 33") != 0)
    result |= 1;
  buf[0] = '\0';
  if (sprintf (buf, "%wf8u %d", (uint_fast8_t) 123, 33, 44, 55) < 0
      || strcmp (buf, "123 33") != 0)
    result |= 1;
  buf[0] = '\0';
  if (sprintf (buf, "%w16u %d", (uint16_t) 12345, 33, 44, 55) < 0
      || strcmp (buf, "12345 33") != 0)
    result |= 2;
  buf[0] = '\0';
  if (sprintf (buf, "%wf16u %d", (uint_fast16_t) 12345, 33, 44, 55) < 0
      || strcmp (buf, "12345 33") != 0)
    result |= 2;
  buf[0] = '\0';
  if (sprintf (buf, "%w32u %d", (uint32_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 4;
  buf[0] = '\0';
  if (sprintf (buf, "%wf32u %d", (uint_fast32_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 4;
#if HAVE_STDINT_H_WITH_UINTMAX || HAVE_INTTYPES_H_WITH_UINTMAX
  buf[0] = '\0';
  if (sprintf (buf, "%w64u %d", (uint64_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 8;
  buf[0] = '\0';
  if (sprintf (buf, "%wf64u %d", (uint_fast64_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    result |= 8;
#else
  result |= 8;
#endif
  return result;
}]])],
        [gl_cv_func_printf_sizes_c23=yes],
        [gl_cv_func_printf_sizes_c23=no],
        [
         case "$host_os" in
                               # Guess no on glibc systems.
           *-gnu* | gnu*)      gl_cv_func_printf_sizes_c23="guessing no";;
                               # Guess no on musl systems.
           *-musl* | midipix*) gl_cv_func_printf_sizes_c23="guessing no";;
                               # Guess no on Android.
           linux*-android*)    gl_cv_func_printf_sizes_c23="guessing no";;
                               # Guess no on native Windows.
           mingw* | windows*)  gl_cv_func_printf_sizes_c23="guessing no";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_printf_sizes_c23="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports 'long double'
dnl arguments together with the 'L' size specifier. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_long_double.

AC_DEFUN([gl_PRINTF_LONG_DOUBLE],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports 'long double' arguments],
    [gl_cv_func_printf_long_double],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[10000];
int main ()
{
  int result = 0;
  buf[0] = '\0';
  if (sprintf (buf, "%Lf %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.750000 33") != 0)
    result |= 1;
  buf[0] = '\0';
  if (sprintf (buf, "%Le %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.750000e+00 33") != 0)
    result |= 2;
  buf[0] = '\0';
  if (sprintf (buf, "%Lg %d", 1.75L, 33, 44, 55) < 0
      || strcmp (buf, "1.75 33") != 0)
    result |= 4;
  return result;
}]])],
        [gl_cv_func_printf_long_double=yes],
        [gl_cv_func_printf_long_double=no],
        [case "$host_os" in
                              # Guess no on BeOS.
           beos*)             gl_cv_func_printf_long_double="guessing no";;
                              # Guess yes on Android.
           linux*-android*)   gl_cv_func_printf_long_double="guessing yes";;
                              # Guess yes on MSVC, no on mingw.
           windows*-msvc*)    gl_cv_func_printf_long_double="guessing yes" ;;
           mingw* | windows*) AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                ],
                                [gl_cv_func_printf_long_double="guessing yes"],
                                [gl_cv_func_printf_long_double="guessing no"])
                              ;;
           *)                 gl_cv_func_printf_long_double="guessing yes";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports infinite and NaN
dnl 'double' arguments and negative zero arguments in the %f, %e, %g
dnl directives. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_infinite.

AC_DEFUN([gl_PRINTF_INFINITE],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports infinite 'double' arguments],
    [gl_cv_func_printf_infinite],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static int
strisnan (const char *string, size_t start_index, size_t end_index)
{
  if (start_index < end_index)
    {
      if (string[start_index] == '-')
        start_index++;
      if (start_index + 3 <= end_index
          && memcmp (string + start_index, "nan", 3) == 0)
        {
          start_index += 3;
          if (start_index == end_index
              || (string[start_index] == '(' && string[end_index - 1] == ')'))
            return 1;
        }
    }
  return 0;
}
static int
have_minus_zero ()
{
  static double plus_zero = 0.0;
  double minus_zero = - plus_zero;
  return memcmp (&plus_zero, &minus_zero, sizeof (double)) != 0;
}
static char buf[10000];
static double zero = 0.0;
int main ()
{
  int result = 0;
  if (sprintf (buf, "%f", 1.0 / zero) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%f", -1.0 / zero) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%f", zero / zero) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 2;
  if (sprintf (buf, "%e", 1.0 / zero) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 4;
  if (sprintf (buf, "%e", -1.0 / zero) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 4;
  if (sprintf (buf, "%e", zero / zero) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 8;
  if (sprintf (buf, "%g", 1.0 / zero) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 16;
  if (sprintf (buf, "%g", -1.0 / zero) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 16;
  if (sprintf (buf, "%g", zero / zero) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 32;
  /* This test fails on HP-UX 10.20.  */
  if (have_minus_zero ())
    if (sprintf (buf, "%g", - zero) < 0
        || strcmp (buf, "-0") != 0)
    result |= 64;
  return result;
}]])],
        [gl_cv_func_printf_infinite=yes],
        [gl_cv_func_printf_infinite=no],
        [
         case "$host_os" in
changequote(,)dnl
                                 # Guess yes on glibc systems.
           *-gnu* | gnu*)        gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on FreeBSD >= 6.
           freebsd[1-5].*)       gl_cv_func_printf_infinite="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_printf_infinite="guessing yes";;
           midnightbsd*)         gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_printf_infinite="guessing no";;
           darwin*)              gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on HP-UX >= 11.
           hpux[7-9]* | hpux10*) gl_cv_func_printf_infinite="guessing no";;
           hpux*)                gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                 gl_cv_func_printf_infinite="guessing no";;
           netbsd*)              gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on OpenBSD >= 6.0.
           openbsd[1-5].*)       gl_cv_func_printf_infinite="guessing no";;
           openbsd*)             gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess yes on BeOS.
           beos*)                gl_cv_func_printf_infinite="guessing yes";;
                                 # Guess no on Android.
           linux*-android*)      gl_cv_func_printf_infinite="guessing no";;
changequote([,])dnl
                                 # Guess yes on MSVC, no on mingw.
           windows*-msvc*)       gl_cv_func_printf_infinite="guessing yes" ;;
           mingw* | windows*)    AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                   ],
                                   [gl_cv_func_printf_infinite="guessing yes"],
                                   [gl_cv_func_printf_infinite="guessing no"])
                                 ;;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_printf_infinite="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports infinite and NaN
dnl 'long double' arguments in the %f, %e, %g directives. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_infinite_long_double.

AC_DEFUN([gl_PRINTF_INFINITE_LONG_DOUBLE],
[
  AC_REQUIRE([gl_PRINTF_LONG_DOUBLE])
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_BIGENDIAN])
  AC_REQUIRE([gl_LONG_DOUBLE_VS_DOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  dnl The user can set or unset the variable gl_printf_safe to indicate
  dnl that he wishes a safe handling of non-IEEE-754 'long double' values.
  if test -n "$gl_printf_safe"; then
    AC_DEFINE([CHECK_PRINTF_SAFE], [1],
      [Define if you wish *printf() functions that have a safe handling of
       non-IEEE-754 'long double' values.])
  fi
  case "$gl_cv_func_printf_long_double" in
    *yes)
      AC_CACHE_CHECK([whether printf supports infinite 'long double' arguments],
        [gl_cv_func_printf_infinite_long_double],
        [
          AC_RUN_IFELSE(
            [AC_LANG_SOURCE([[
]GL_NOCRASH[
#include <float.h>
#include <stdio.h>
#include <string.h>
static int
strisnan (const char *string, size_t start_index, size_t end_index)
{
  if (start_index < end_index)
    {
      if (string[start_index] == '-')
        start_index++;
      if (start_index + 3 <= end_index
          && memcmp (string + start_index, "nan", 3) == 0)
        {
          start_index += 3;
          if (start_index == end_index
              || (string[start_index] == '(' && string[end_index - 1] == ')'))
            return 1;
        }
    }
  return 0;
}
static char buf[10000];
static long double zeroL = 0.0L;
int main ()
{
  int result = 0;
  nocrash_init();
  if (sprintf (buf, "%Lf", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lf", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lf", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
  if (sprintf (buf, "%Le", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Le", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Le", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
  if (sprintf (buf, "%Lg", 1.0L / zeroL) < 0
      || (strcmp (buf, "inf") != 0 && strcmp (buf, "infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lg", -1.0L / zeroL) < 0
      || (strcmp (buf, "-inf") != 0 && strcmp (buf, "-infinity") != 0))
    result |= 1;
  if (sprintf (buf, "%Lg", zeroL / zeroL) < 0
      || !strisnan (buf, 0, strlen (buf)))
    result |= 1;
#if CHECK_PRINTF_SAFE && ((defined __ia64 && LDBL_MANT_DIG == 64) || (defined __x86_64__ || defined __amd64__) || (defined __i386 || defined __i386__ || defined _I386 || defined _M_IX86 || defined _X86_)) && !HAVE_SAME_LONG_DOUBLE_AS_DOUBLE
/* Representation of an 80-bit 'long double' as an initializer for a sequence
   of 'unsigned int' words.  */
# ifdef WORDS_BIGENDIAN
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { ((unsigned int) (exponent) << 16) | ((unsigned int) (manthi) >> 16), \
       ((unsigned int) (manthi) << 16) | ((unsigned int) (mantlo) >> 16),   \
       (unsigned int) (mantlo) << 16                                        \
     }
# else
#  define LDBL80_WORDS(exponent,manthi,mantlo) \
     { mantlo, manthi, exponent }
# endif
  { /* Quiet NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0xC3333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Le", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Lg", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
  }
  {
    /* Signalling NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x83333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Le", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
    if (sprintf (buf, "%Lg", x.value) < 0
        || !strisnan (buf, 0, strlen (buf)))
      result |= 2;
  }
  { /* Pseudo-NaN.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x40000001, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 4;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 4;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 4;
  }
  { /* Pseudo-Infinity.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0xFFFF, 0x00000000, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 8;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 8;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 8;
  }
  { /* Pseudo-Zero.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4004, 0x00000000, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 16;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 16;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 16;
  }
  { /* Unnormalized number.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x4000, 0x63333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 32;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 32;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 32;
  }
  { /* Pseudo-Denormal.  */
    static union { unsigned int word[4]; long double value; } x =
      { LDBL80_WORDS (0x0000, 0x83333333, 0x00000000) };
    if (sprintf (buf, "%Lf", x.value) <= 0)
      result |= 64;
    if (sprintf (buf, "%Le", x.value) <= 0)
      result |= 64;
    if (sprintf (buf, "%Lg", x.value) <= 0)
      result |= 64;
  }
#endif
  return result;
}]])],
            [gl_cv_func_printf_infinite_long_double=yes],
            [gl_cv_func_printf_infinite_long_double=no],
            [case "$host_cpu" in
                                     # Guess no on ia64, x86_64, i386.
               ia64 | x86_64 | i*86) gl_cv_func_printf_infinite_long_double="guessing no";;
               *)
                 case "$host_os" in
changequote(,)dnl
                                         # Guess yes on glibc systems.
                   *-gnu* | gnu*)        gl_cv_func_printf_infinite_long_double="guessing yes";;
                                         # Guess yes on musl systems.
                   *-musl* | midipix*)   gl_cv_func_printf_infinite_long_double="guessing yes";;
                                         # Guess yes on FreeBSD >= 6.
                   freebsd[1-5].*)       gl_cv_func_printf_infinite_long_double="guessing no";;
                   freebsd* | kfreebsd*) gl_cv_func_printf_infinite_long_double="guessing yes";;
                   midnightbsd*)         gl_cv_func_printf_infinite_long_double="guessing yes";;
                                         # Guess yes on HP-UX >= 11.
                   hpux[7-9]* | hpux10*) gl_cv_func_printf_infinite_long_double="guessing no";;
                   hpux*)                gl_cv_func_printf_infinite_long_double="guessing yes";;
                                         # Guess yes on OpenBSD >= 6.0.
                   openbsd[1-5].*)       gl_cv_func_printf_infinite_long_double="guessing no";;
                   openbsd*)             gl_cv_func_printf_infinite_long_double="guessing yes";;
                                         # Guess no on Android.
                   linux*-android*)      gl_cv_func_printf_infinite_long_double="guessing no";;
changequote([,])dnl
                                         # Guess yes on MSVC, no on mingw.
                   windows*-msvc*)       gl_cv_func_printf_infinite_long_double="guessing yes" ;;
                   mingw* | windows*)    AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                           ],
                                           [gl_cv_func_printf_infinite_long_double="guessing yes"],
                                           [gl_cv_func_printf_infinite_long_double="guessing no"])
                                         ;;
                                         # If we don't know, obey --enable-cross-guesses.
                   *)                    gl_cv_func_printf_infinite_long_double="$gl_cross_guess_normal";;
                 esac
                 ;;
             esac
            ])
        ])
      ;;
    *)
      gl_cv_func_printf_infinite_long_double="irrelevant"
      ;;
  esac
])

dnl Test whether the *printf family of functions supports the 'a' and 'A'
dnl conversion specifier for hexadecimal output of floating-point numbers.
dnl (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_directive_a.

AC_DEFUN([gl_PRINTF_DIRECTIVE_A],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'a' and 'A' directives],
    [gl_cv_func_printf_directive_a],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
static double zero = 0.0;
int main ()
{
  int result = 0;
  /* This fails on FreeBSD 5.2.1, Solaris 11.4.  */
  if (sprintf (buf, "%a %d", 3.1416015625, 33, 44, 55) < 0
      || (strcmp (buf, "0x1.922p+1 33") != 0
          && strcmp (buf, "0x3.244p+0 33") != 0
          && strcmp (buf, "0x6.488p-1 33") != 0
          && strcmp (buf, "0xc.91p-2 33") != 0))
    result |= 1;
  if (sprintf (buf, "%A %d", -3.1416015625, 33, 44, 55) < 0
      || (strcmp (buf, "-0X1.922P+1 33") != 0
          && strcmp (buf, "-0X3.244P+0 33") != 0
          && strcmp (buf, "-0X6.488P-1 33") != 0
          && strcmp (buf, "-0XC.91P-2 33") != 0))
    result |= 1;
  /* This catches a Mac OS X 10.5, FreeBSD 6.4, NetBSD 10.0 bug:
     it doesn't round.  */
  if (sprintf (buf, "%.2a %d", 1.51, 33, 44, 55) < 0
      || (strcmp (buf, "0x1.83p+0 33") != 0
          && strcmp (buf, "0x3.05p-1 33") != 0
          && strcmp (buf, "0x6.0ap-2 33") != 0
          && strcmp (buf, "0xc.14p-3 33") != 0))
    result |= 2;
  /* This catches a macOS 14 (Darwin 23), FreeBSD 14.0, OpenBSD 7.5, AIX 7.3,
     Solaris 11.4 bug: it doesn't round.  */
  if (sprintf (buf, "%.0a %d", 1.51, 33, 44, 55) < 0
      || (strcmp (buf, "0x2p+0 33") != 0
          && strcmp (buf, "0x3p-1 33") != 0
          && strcmp (buf, "0x6p-2 33") != 0
          && strcmp (buf, "0xcp-3 33") != 0))
    result |= 4;
  /* This catches a Mac OS X 10.5, FreeBSD 6.4 bug.  See
     <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html> */
  if (sprintf (buf, "%010a %d", 1.0 / zero, 33, 44, 55) < 0
      || buf[0] == '0')
    result |= 8;
  /* This catches a Mac OS X 10.3.9 (Darwin 7.9), FreeBSD 6.4 bug.  */
  if (sprintf (buf, "%.1a", 1.999) < 0
      || (strcmp (buf, "0x1.0p+1") != 0
          && strcmp (buf, "0x2.0p+0") != 0
          && strcmp (buf, "0x4.0p-1") != 0
          && strcmp (buf, "0x8.0p-2") != 0))
    result |= 16;
  /* This catches the same Mac OS X 10.3.9 (Darwin 7.9) bug and also a
     glibc 2.4 bug <https://sourceware.org/PR2908>
     and a FreeBSD 6.4, NetBSD 10.0 bug.  */
  if (sprintf (buf, "%.1La", 1.999L) < 0
      || (strcmp (buf, "0x1.0p+1") != 0
          && strcmp (buf, "0x2.0p+0") != 0
          && strcmp (buf, "0x4.0p-1") != 0
          && strcmp (buf, "0x8.0p-2") != 0))
    result |= 32;
  return result;
}]])],
        [gl_cv_func_printf_directive_a=yes],
        [gl_cv_func_printf_directive_a=no],
        [
         case "$host_os" in
                                 # Guess yes on glibc >= 2.5 systems.
           *-gnu* | gnu*)
             AC_EGREP_CPP([BZ2908], [
               #include <features.h>
               #ifdef __GNU_LIBRARY__
                #if ((__GLIBC__ == 2 && __GLIBC_MINOR__ >= 5) || (__GLIBC__ > 2)) && !defined __UCLIBC__
                 BZ2908
                #endif
               #endif
               ],
               [gl_cv_func_printf_directive_a="guessing yes"],
               [gl_cv_func_printf_directive_a="guessing no"])
             ;;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_printf_directive_a="guessing yes";;
                                 # Guess no on Android.
           linux*-android*)      gl_cv_func_printf_directive_a="guessing no";;
                                 # Guess no on native Windows.
           mingw* | windows*)    gl_cv_func_printf_directive_a="guessing no";;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_printf_directive_a="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the 'b' conversion
dnl specifier for binary output of integers.
dnl (ISO C23)
dnl Result is gl_cv_func_printf_directive_b.

AC_DEFUN([gl_PRINTF_DIRECTIVE_B],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'b' directive],
    [gl_cv_func_printf_directive_b],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  int result = 0;
  if (sprintf (buf, "%b %d", 12345, 33, 44, 55) < 0
      || strcmp (buf, "11000000111001 33") != 0)
    result |= 1;
  return result;
}]])],
        [gl_cv_func_printf_directive_b=yes],
        [gl_cv_func_printf_directive_b=no],
        [
         case "$host_os" in
                               # Guess yes on glibc >= 2.35 systems.
           *-gnu* | gnu*)
             AC_EGREP_CPP([Lucky], [
               #include <features.h>
               #ifdef __GNU_LIBRARY__
                #if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 35) || (__GLIBC__ > 2)
                 Lucky user
                #endif
               #endif
               ],
               [gl_cv_func_printf_directive_uppercase_b="guessing yes"],
               [gl_cv_func_printf_directive_uppercase_b="guessing no"])
             ;;
                               # Guess no on musl systems.
           *-musl* | midipix*) gl_cv_func_printf_directive_b="guessing no";;
                               # Guess no on Android.
           linux*-android*)    gl_cv_func_printf_directive_b="guessing no";;
                               # Guess no on native Windows.
           mingw* | windows*)  gl_cv_func_printf_directive_b="guessing no";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_printf_directive_b="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the 'B' conversion
dnl specifier for binary output of integers.
dnl (GNU, encouraged by ISO C23 § 7.23.6.1)
dnl Result is gl_cv_func_printf_directive_uppercase_b.

AC_DEFUN([gl_PRINTF_DIRECTIVE_UPPERCASE_B],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'B' directive],
    [gl_cv_func_printf_directive_uppercase_b],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  int result = 0;
  if (sprintf (buf, "%#B %d", 12345, 33, 44, 55) < 0
      || strcmp (buf, "0B11000000111001 33") != 0)
    result |= 1;
  return result;
}]])],
        [gl_cv_func_printf_directive_uppercase_b=yes],
        [gl_cv_func_printf_directive_uppercase_b=no],
        [
         case "$host_os" in
                               # Guess yes on glibc >= 2.35 systems.
           *-gnu* | gnu*)
             AC_EGREP_CPP([Lucky], [
               #include <features.h>
               #ifdef __GNU_LIBRARY__
                #if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 35) || (__GLIBC__ > 2)
                 Lucky user
                #endif
               #endif
               ],
               [gl_cv_func_printf_directive_uppercase_b="guessing yes"],
               [gl_cv_func_printf_directive_uppercase_b="guessing no"])
             ;;
                               # Guess no on musl systems.
           *-musl* | midipix*) gl_cv_func_printf_directive_uppercase_b="guessing no";;
                               # Guess no on Android.
           linux*-android*)    gl_cv_func_printf_directive_uppercase_b="guessing no";;
                               # Guess no on native Windows.
           mingw* | windows*)  gl_cv_func_printf_directive_uppercase_b="guessing no";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_printf_directive_uppercase_b="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the %F format
dnl directive. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_directive_f.

AC_DEFUN([gl_PRINTF_DIRECTIVE_F],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'F' directive],
    [gl_cv_func_printf_directive_f],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
static double zero = 0.0;
int main ()
{
  int result = 0;
  if (sprintf (buf, "%F %d", 1234567.0, 33, 44, 55) < 0
      || strcmp (buf, "1234567.000000 33") != 0)
    result |= 1;
  if (sprintf (buf, "%F", 1.0 / zero) < 0
      || (strcmp (buf, "INF") != 0 && strcmp (buf, "INFINITY") != 0))
    result |= 2;
  /* This catches a Cygwin 1.5.x bug.  */
  if (sprintf (buf, "%.F", 1234.0) < 0
      || strcmp (buf, "1234") != 0)
    result |= 4;
  return result;
}]])],
        [gl_cv_func_printf_directive_f=yes],
        [gl_cv_func_printf_directive_f=no],
        [
         case "$host_os" in
changequote(,)dnl
                                 # Guess yes on glibc systems.
           *-gnu* | gnu*)        gl_cv_func_printf_directive_f="guessing yes";;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_printf_directive_f="guessing yes";;
                                 # Guess yes on FreeBSD >= 6.
           freebsd[1-5].*)       gl_cv_func_printf_directive_f="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_printf_directive_f="guessing yes";;
           midnightbsd*)         gl_cv_func_printf_directive_f="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_printf_directive_f="guessing no";;
           darwin*)              gl_cv_func_printf_directive_f="guessing yes";;
                                 # Guess yes on OpenBSD >= 6.0.
           openbsd[1-5].*)       gl_cv_func_printf_directive_f="guessing no";;
           openbsd*)             gl_cv_func_printf_directive_f="guessing yes";;
                                 # Guess yes on Solaris >= 2.10.
           solaris2.[1-9][0-9]*) gl_cv_func_printf_directive_f="guessing yes";;
           solaris*)             gl_cv_func_printf_directive_f="guessing no";;
                                 # Guess no on Android.
           linux*-android*)      gl_cv_func_printf_directive_f="guessing no";;
changequote([,])dnl
                                 # Guess yes on MSVC, no on mingw.
           windows*-msvc*)       gl_cv_func_printf_directive_f="guessing yes" ;;
           mingw* | windows*)    AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                   ],
                                   [gl_cv_func_printf_directive_f="guessing yes"],
                                   [gl_cv_func_printf_directive_f="guessing no"])
                                 ;;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_printf_directive_f="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the %n format
dnl directive. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_directive_n.

AC_DEFUN([gl_PRINTF_DIRECTIVE_N],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'n' directive],
    [gl_cv_func_printf_directive_n],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <signal.h>
#include <stdio.h>
#include <string.h>
#if defined _WIN32 && !defined __CYGWIN__
# include <stdlib.h>
#else
# include <unistd.h>
#endif
#ifdef _MSC_VER
#include <crtdbg.h>
#include <inttypes.h>
/* See page about "Parameter Validation" on msdn.microsoft.com.
   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/parameter-validation>
   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/set-invalid-parameter-handler-set-thread-local-invalid-parameter-handler>  */
static void cdecl
invalid_parameter_handler (const wchar_t *expression,
                           const wchar_t *function,
                           const wchar_t *file, unsigned int line,
                           uintptr_t dummy)
{
  exit (1);
}
#endif
static void
abort_handler (int sig)
{
  (void) sig;
  _exit (1);
}
static char fmtstring[10];
static char buf[100];
int main ()
{
  int count = -1;
#ifdef _MSC_VER
  _set_invalid_parameter_handler (invalid_parameter_handler);
  /* Also avoid an Abort/Retry/Ignore dialog in debug builds.
     <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/crtsetreportmode>  */
  _CrtSetReportMode (_CRT_ASSERT, 0);
#endif
  signal (SIGABRT, abort_handler);
  /* Copy the format string.  Some systems (glibc with _FORTIFY_SOURCE=2)
     support %n in format strings in read-only memory but not in writable
     memory.  */
  strcpy (fmtstring, "%d %n");
  if (sprintf (buf, fmtstring, 123, &count, 33, 44, 55) < 0
      || strcmp (buf, "123 ") != 0
      || count != 4)
    return 1;
  return 0;
}]])],
        [gl_cv_func_printf_directive_n=yes],
        [gl_cv_func_printf_directive_n=no],
        [case "$host_os" in
                              # Guess no on glibc when _FORTIFY_SOURCE >= 2.
           *-gnu* | gnu*)     AC_COMPILE_IFELSE(
                                [AC_LANG_SOURCE(
                                   [[#if _FORTIFY_SOURCE >= 2
                                      error fail
                                     #endif
                                   ]])],
                                [gl_cv_func_printf_directive_n="guessing yes"],
                                [gl_cv_func_printf_directive_n="guessing no"])
                              ;;
                              # Guess no on Android.
           linux*-android*)   gl_cv_func_printf_directive_n="guessing no";;
                              # Guess no on native Windows.
           mingw* | windows*) gl_cv_func_printf_directive_n="guessing no";;
           *)                 gl_cv_func_printf_directive_n="guessing yes";;
         esac
        ])
    ])
])

dnl Test whether the *printf family of functions supports the %ls format
dnl directive and in particular, when a precision is specified, whether
dnl the functions stop converting the wide string argument when the number
dnl of bytes that have been produced by this conversion equals or exceeds
dnl the precision.
dnl Result is gl_cv_func_printf_directive_ls.

AC_DEFUN([gl_PRINTF_DIRECTIVE_LS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'ls' directive],
    [gl_cv_func_printf_directive_ls],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <wchar.h>
#include <string.h>
int main ()
{
  int result = 0;
  char buf[100];
  /* Test whether %ls works at all.
     This test fails on OpenBSD 4.0, IRIX 6.5, Solaris 2.6, Haiku, but not on
     Cygwin 1.5.  */
  {
    static const wchar_t wstring[] = { 'a', 'b', 'c', 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%ls", wstring) < 0
        || strcmp (buf, "abc") != 0)
      result |= 1;
  }
  /* This test fails on IRIX 6.5, Solaris 2.6, Cygwin 1.5, Haiku (with an
     assertion failure inside libc), but not on OpenBSD 4.0.  */
  {
    static const wchar_t wstring[] = { 'a', 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%ls", wstring) < 0
        || strcmp (buf, "a") != 0)
      result |= 2;
  }
  /* Test whether precisions in %ls are supported as specified in ISO C 99
     section 7.19.6.1:
       "If a precision is specified, no more than that many bytes are written
        (including shift sequences, if any), and the array shall contain a
        null wide character if, to equal the multibyte character sequence
        length given by the precision, the function would need to access a
        wide character one past the end of the array."
     This test fails on Solaris 10.  */
  {
    static const wchar_t wstring[] = { 'a', 'b', (wchar_t) 0xfdfdfdfd, 0 };
    buf[0] = '\0';
    if (sprintf (buf, "%.2ls", wstring) < 0
        || strcmp (buf, "ab") != 0)
      result |= 8;
  }
  return result;
}]])],
        [gl_cv_func_printf_directive_ls=yes],
        [gl_cv_func_printf_directive_ls=no],
        [
changequote(,)dnl
         case "$host_os" in
                              # Guess yes on OpenBSD >= 6.0.
           openbsd[1-5].*)    gl_cv_func_printf_directive_ls="guessing no";;
           openbsd*)          gl_cv_func_printf_directive_ls="guessing yes";;
           irix*)             gl_cv_func_printf_directive_ls="guessing no";;
           solaris*)          gl_cv_func_printf_directive_ls="guessing no";;
           cygwin*)           gl_cv_func_printf_directive_ls="guessing no";;
           beos* | haiku*)    gl_cv_func_printf_directive_ls="guessing no";;
                              # Guess no on Android.
           linux*-android*)   gl_cv_func_printf_directive_ls="guessing no";;
                              # Guess yes on native Windows.
           mingw* | windows*) gl_cv_func_printf_directive_ls="guessing yes";;
           *)                 gl_cv_func_printf_directive_ls="guessing yes";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports the %lc format
dnl directive and in particular, when the argument is a null wide character,
dnl whether the functions produce a NUL byte, as specified in ISO C 23
dnl after the issue GB-141 was fixed.
dnl Result is gl_cv_func_printf_directive_lc.

AC_DEFUN([gl_PRINTF_DIRECTIVE_LC],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'lc' directive correctly],
    [gl_cv_func_printf_directive_lc],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <wchar.h>
#include <string.h>
int main ()
{
  int result = 0;
  char buf[100];
  /* This test fails on musl libc 1.2.4.  */
  {
    buf[0] = '\0';
    if (sprintf (buf, "%lc%lc%lc", (wint_t) 'a', (wint_t) 0, (wint_t) 'z') < 0
        || memcmp (buf, "a\0z", 4) != 0)
      result |= 1;
  }
  return result;
}]])],
        [gl_cv_func_printf_directive_lc=yes],
        [gl_cv_func_printf_directive_lc=no],
        [
changequote(,)dnl
         case "$host_os" in
                               # Guess no on musl libc.
           *-musl* | midipix*) gl_cv_func_printf_directive_lc="guessing no";;
                               # Guess yes otherwise.
           *)                  gl_cv_func_printf_directive_lc="guessing yes";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports POSIX/XSI format
dnl strings with positions. (POSIX:2001)
dnl Result is gl_cv_func_printf_positions.

AC_DEFUN_ONCE([gl_PRINTF_POSITIONS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports POSIX/XSI format strings with positions],
    [gl_cv_func_printf_positions],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
/* The string "%2$d %1$d", with dollar characters protected from the shell's
   dollar expansion (possibly an autoconf bug).  */
static char format[] = { '%', '2', '$', 'd', ' ', '%', '1', '$', 'd', '\0' };
static char buf[100];
int main ()
{
  sprintf (buf, format, 33, 55);
  return (strcmp (buf, "55 33") != 0);
}]])],
        [gl_cv_func_printf_positions=yes],
        [gl_cv_func_printf_positions=no],
        [
changequote(,)dnl
         case "$host_os" in
           netbsd[1-3]* | netbsdelf[1-3]* | netbsdaout[1-3]* | netbsdcoff[1-3]*)
             gl_cv_func_printf_positions="guessing no";;
           beos*)
             gl_cv_func_printf_positions="guessing no";;
             # Guess yes on Android.
           linux*-android*)
             gl_cv_func_printf_positions="guessing yes";;
             # Guess no on native Windows.
           mingw* | windows* | pw*)
             gl_cv_func_printf_positions="guessing no";;
           *)
             gl_cv_func_printf_positions="guessing yes";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports POSIX/XSI format
dnl strings with the ' flag for grouping of decimal digits. (POSIX:2001)
dnl Result is gl_cv_func_printf_flag_grouping.

AC_DEFUN([gl_PRINTF_FLAG_GROUPING],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the grouping flag],
    [gl_cv_func_printf_flag_grouping],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  if (sprintf (buf, "%'d %d", 1234567, 99) < 0
      || buf[strlen (buf) - 1] != '9')
    return 1;
  return 0;
}]])],
        [gl_cv_func_printf_flag_grouping=yes],
        [gl_cv_func_printf_flag_grouping=no],
        [
changequote(,)dnl
         case "$host_os" in
           cygwin*)                 gl_cv_func_printf_flag_grouping="guessing no";;
           netbsd*)                 gl_cv_func_printf_flag_grouping="guessing no";;
                                    # Guess no on Android.
           linux*-android*)         gl_cv_func_printf_flag_grouping="guessing no";;
                                    # Guess no on native Windows.
           mingw* | windows* | pw*) gl_cv_func_printf_flag_grouping="guessing no";;
           *)                       gl_cv_func_printf_flag_grouping="guessing yes";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports POSIX/XSI format
dnl strings with the ' flag for grouping of decimal digits on integers,
dnl together with a precision.
dnl Result is gl_cv_func_printf_flag_grouping_int_precision.

AC_DEFUN([gl_PRINTF_FLAG_GROUPING_INT_PRECISION],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CACHE_CHECK([whether printf supports grouping on integers with a precision],
    [gl_cv_func_printf_flag_grouping_int_precision],
    [
      dnl Prepare a guess, used when cross-compiling or when specific locales
      dnl are not available.
      case "$host_os" in
        # Guess no on FreeBSD, NetBSD, Solaris, Cygwin, Haiku.
        freebsd* | dragonfly* | netbsd* | solaris* | cygwin* | haiku*)
          gl_cv_func_printf_flag_grouping_int_precision="guessing no";;
        *)
          gl_cv_func_printf_flag_grouping_int_precision="guessing yes";;
      esac
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <locale.h>
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  if (setlocale (LC_ALL, "fr_FR.UTF-8") != NULL
      || setlocale (LC_ALL, "fr_FR") != NULL
      || setlocale (LC_ALL, "fr_FR.ISO-8859-1") != NULL
      || setlocale (LC_ALL, "fr_FR.ISO8859-1") != NULL)
    {
      if (sprintf (buf, "%'.10d", 1000) < 0)
        return 1;
      if (strlen (buf) == 10 && strcmp (buf, "0000001000") != 0)
        /* The sprintf implementation has produced fewer than 10 digits.  */
        return 2;
      else
        return 0;
    }
  return 3;
}]])],
        [gl_cv_func_printf_flag_grouping_int_precision=yes],
        [if test $? = 2; then
           gl_cv_func_printf_flag_grouping_int_precision=no
         fi
        ],
        [:])
    ])
])

dnl Test whether the *printf family of functions supports POSIX/XSI format
dnl strings with the ' flag for grouping of decimal digits, when the thousands
dnl separator is a multibyte character (such as U+00A0 or U+202F in a UTF-8
dnl locale).
dnl Result is gl_cv_func_printf_flag_grouping_multibyte.

AC_DEFUN([gl_PRINTF_FLAG_GROUPING_MULTIBYTE],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_CACHE_CHECK([whether printf supports grouping with a multibyte separator],
    [gl_cv_func_printf_flag_grouping_multibyte],
    [
      dnl Prepare a guess, used when cross-compiling or when specific locales
      dnl are not available.
      case "$host_os" in
        # Guess no on NetBSD and Solaris 11 OpenIndiana.
        netbsd* | solaris*)
          gl_cv_func_printf_flag_grouping_multibyte="guessing no";;
        *)
          gl_cv_func_printf_flag_grouping_multibyte="guessing yes";;
      esac
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <locale.h>
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  if (setlocale (LC_ALL, "fr_FR.UTF-8") == NULL)
    return 0;
  if (sprintf (buf, "%'.0f", 1000.0) < 0)
    return 1;
  if (strlen (localeconv ()->thousands_sep) > 1)
    {
      if (strlen (buf) <= 4 + 1)
        return 2;
      else
        return 3;
    }
  return 0;
}]])],
        [:],
        [case $? in
           2) gl_cv_func_printf_flag_grouping_multibyte=no ;;
           3) gl_cv_func_printf_flag_grouping_multibyte=yes ;;
         esac
        ],
        [:])
    ])
])

dnl Test whether the *printf family of functions supports the - flag correctly.
dnl (ISO C99.) See
dnl <https://lists.gnu.org/r/bug-coreutils/2008-02/msg00035.html>
dnl Result is gl_cv_func_printf_flag_leftadjust.

AC_DEFUN([gl_PRINTF_FLAG_LEFTADJUST],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the left-adjust flag correctly],
    [gl_cv_func_printf_flag_leftadjust],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  /* Check that a '-' flag is not annihilated by a negative width.  */
  if (sprintf (buf, "a%-*sc", -3, "b") < 0
      || strcmp (buf, "ab  c") != 0)
    return 1;
  return 0;
}]])],
        [gl_cv_func_printf_flag_leftadjust=yes],
        [gl_cv_func_printf_flag_leftadjust=no],
        [
changequote(,)dnl
         case "$host_os" in
                              # Guess yes on HP-UX 11.
           hpux11*)           gl_cv_func_printf_flag_leftadjust="guessing yes";;
                              # Guess no on HP-UX 10 and older.
           hpux*)             gl_cv_func_printf_flag_leftadjust="guessing no";;
                              # Guess yes on Android.
           linux*-android*)   gl_cv_func_printf_flag_leftadjust="guessing yes";;
                              # Guess yes on native Windows.
           mingw* | windows*) gl_cv_func_printf_flag_leftadjust="guessing yes";;
                              # Guess yes otherwise.
           *)                 gl_cv_func_printf_flag_leftadjust="guessing yes";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports padding of non-finite
dnl values with the 0 flag correctly. (ISO C99 + TC1 + TC2.) See
dnl <https://lists.gnu.org/r/bug-gnulib/2007-04/msg00107.html>
dnl Result is gl_cv_func_printf_flag_zero.

AC_DEFUN([gl_PRINTF_FLAG_ZERO],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the zero flag correctly],
    [gl_cv_func_printf_flag_zero],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[100];
static double zero = 0.0;
int main ()
{
  if (sprintf (buf, "%010f", 1.0 / zero, 33, 44, 55) < 0
      || (strcmp (buf, "       inf") != 0
          && strcmp (buf, "  infinity") != 0))
    return 1;
  return 0;
}]])],
        [gl_cv_func_printf_flag_zero=yes],
        [gl_cv_func_printf_flag_zero=no],
        [
changequote(,)dnl
         case "$host_os" in
                               # Guess yes on glibc systems.
           *-gnu* | gnu*)      gl_cv_func_printf_flag_zero="guessing yes";;
                               # Guess yes on musl systems.
           *-musl* | midipix*) gl_cv_func_printf_flag_zero="guessing yes";;
                               # Guess yes on BeOS.
           beos*)              gl_cv_func_printf_flag_zero="guessing yes";;
                               # Guess no on Android.
           linux*-android*)    gl_cv_func_printf_flag_zero="guessing no";;
                               # Guess no on native Windows.
           mingw* | windows*)  gl_cv_func_printf_flag_zero="guessing no";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_printf_flag_zero="$gl_cross_guess_normal";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports the # flag with a
dnl zero precision and a zero value in the 'x' and 'X' directives correctly.
dnl ISO C and POSIX specify that for the 'd', 'i', 'b', 'o', 'u', 'x', 'X'
dnl directives: "The result of converting a zero value with a precision of
dnl zero is no characters."  But on Mac OS X 10.5, for the 'x', 'X' directives,
dnl when a # flag is present, the output is "0" instead of "".
dnl Result is gl_cv_func_printf_flag_alt_precision_zero.

AC_DEFUN([gl_PRINTF_FLAG_ALT_PRECISION_ZERO],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the alternative flag with a zero precision],
    [gl_cv_func_printf_flag_alt_precision_zero],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
static char buf[10];
int main ()
{
  int result = 0;
  if (sprintf (buf, "%#.0x %d", 0, 33, 44) > 0 + 3)
    result |= 1;
  return result;
}]])],
        [gl_cv_func_printf_flag_alt_precision_zero=yes],
        [gl_cv_func_printf_flag_alt_precision_zero=no],
        [
changequote(,)dnl
         case "$host_os" in
           # Guess no only on macOS 10..12 systems.
           darwin[0-9] | darwin[0-9].* | \
           darwin1[0-9] | darwin1[0-9].* | \
           darwin2[0-1] | darwin2[0-1].*)
                    gl_cv_func_printf_flag_alt_precision_zero="guessing no" ;;
           darwin*) gl_cv_func_printf_flag_alt_precision_zero="guessing yes" ;;
           *)       gl_cv_func_printf_flag_alt_precision_zero="guessing yes" ;;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions supports large precisions.
dnl On mingw, precisions larger than 512 are treated like 512, in integer,
dnl floating-point or pointer output. On Solaris 10/x86, precisions larger
dnl than 510 in floating-point output crash the program. On Solaris 10/SPARC,
dnl precisions larger than 510 in floating-point output yield wrong results.
dnl On AIX 7.1, precisions larger than 998 in floating-point output yield
dnl wrong results. On BeOS, precisions larger than 1044 crash the program.
dnl Result is gl_cv_func_printf_precision.

AC_DEFUN([gl_PRINTF_PRECISION],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports large precisions],
    [gl_cv_func_printf_precision],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
static char buf[5000];
int main ()
{
  int result = 0;
#ifdef __BEOS__
  /* On BeOS, this would crash and show a dialog box.  Avoid the crash.  */
  return 1;
#endif
  if (sprintf (buf, "%.4000d %d", 1, 33, 44) < 4000 + 3)
    result |= 1;
  if (sprintf (buf, "%.4000f %d", 1.0, 33, 44) < 4000 + 5)
    result |= 2;
  if (sprintf (buf, "%.511f %d", 1.0, 33, 44) < 511 + 5
      || buf[0] != '1')
    result |= 4;
  if (sprintf (buf, "%.999f %d", 1.0, 33, 44) < 999 + 5
      || buf[0] != '1')
    result |= 4;
  return result;
}]])],
        [gl_cv_func_printf_precision=yes],
        [gl_cv_func_printf_precision=no],
        [
changequote(,)dnl
         case "$host_os" in
           # Guess no only on Solaris, native Windows, and BeOS systems.
           solaris*)                gl_cv_func_printf_precision="guessing no" ;;
           mingw* | windows* | pw*) gl_cv_func_printf_precision="guessing no" ;;
           beos*)                   gl_cv_func_printf_precision="guessing no" ;;
                                    # Guess yes on Android.
           linux*-android*)         gl_cv_func_printf_precision="guessing yes" ;;
           *)                       gl_cv_func_printf_precision="guessing yes" ;;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the *printf family of functions recovers gracefully in case
dnl of an out-of-memory condition, or whether it crashes the entire program.
dnl Result is gl_cv_func_printf_enomem.

AC_DEFUN([gl_PRINTF_ENOMEM],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_MULTIARCH])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf survives out-of-memory conditions],
    [gl_cv_func_printf_enomem],
    [
      gl_cv_func_printf_enomem="guessing no"
      if test "$cross_compiling" = no; then
        if test $APPLE_UNIVERSAL_BUILD = 0; then
          AC_LANG_CONFTEST([AC_LANG_SOURCE([[
]GL_NOCRASH[
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
int main()
{
  struct rlimit limit;
  int ret;
  nocrash_init ();
  /* Some printf implementations allocate temporary space with malloc.  */
  /* On BSD systems, malloc() is limited by RLIMIT_DATA.  */
#ifdef RLIMIT_DATA
  if (getrlimit (RLIMIT_DATA, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_DATA, &limit) < 0)
    return 77;
#endif
  /* On Linux systems, malloc() is limited by RLIMIT_AS.  */
#ifdef RLIMIT_AS
  if (getrlimit (RLIMIT_AS, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_AS, &limit) < 0)
    return 77;
#endif
  /* Some printf implementations allocate temporary space on the stack.  */
#ifdef RLIMIT_STACK
  if (getrlimit (RLIMIT_STACK, &limit) < 0)
    return 77;
  if (limit.rlim_max == RLIM_INFINITY || limit.rlim_max > 5000000)
    limit.rlim_max = 5000000;
  limit.rlim_cur = limit.rlim_max;
  if (setrlimit (RLIMIT_STACK, &limit) < 0)
    return 77;
#endif
  ret = printf ("%.5000000f", 1.0);
  return !(ret == 5000002 || (ret < 0 && errno == ENOMEM));
}
          ]])])
          if AC_TRY_EVAL([ac_link]) && test -s conftest$ac_exeext; then
            (./conftest 2>&AS_MESSAGE_LOG_FD
             result=$?
             _AS_ECHO_LOG([\$? = $result])
             if test $result != 0 && test $result != 77; then result=1; fi
             exit $result
            ) >/dev/null 2>/dev/null
            case $? in
              0) gl_cv_func_printf_enomem="yes" ;;
              77) gl_cv_func_printf_enomem="guessing no" ;;
              *) gl_cv_func_printf_enomem="no" ;;
            esac
          else
            gl_cv_func_printf_enomem="guessing no"
          fi
          rm -fr conftest*
        else
          dnl A universal build on Apple Mac OS X platforms.
          dnl The result would be 'no' in 32-bit mode and 'yes' in 64-bit mode.
          dnl But we need a configuration result that is valid in both modes.
          gl_cv_func_printf_enomem="guessing no"
        fi
      fi
      if test "$gl_cv_func_printf_enomem" = "guessing no"; then
changequote(,)dnl
        case "$host_os" in
                           # Guess yes on glibc systems.
          *-gnu* | gnu*)   gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on Solaris.
          solaris*)        gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on AIX.
          aix*)            gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on HP-UX/hppa.
          hpux*)           case "$host_cpu" in
                             hppa*) gl_cv_func_printf_enomem="guessing yes";;
                             *)     gl_cv_func_printf_enomem="guessing no";;
                           esac
                           ;;
                           # Guess yes on IRIX.
          irix*)           gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on OSF/1.
          osf*)            gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on BeOS.
          beos*)           gl_cv_func_printf_enomem="guessing yes";;
                           # Guess yes on Haiku.
          haiku*)          gl_cv_func_printf_enomem="guessing yes";;
                           # Guess no on Android.
          linux*-android*) gl_cv_func_printf_enomem="guessing no";;
                           # If we don't know, obey --enable-cross-guesses.
          *)               gl_cv_func_printf_enomem="$gl_cross_guess_normal";;
        esac
changequote([,])dnl
      fi
    ])
])

dnl Test whether the snprintf function exists. (ISO C99, POSIX:2001)
dnl Result is ac_cv_func_snprintf.

AC_DEFUN([gl_SNPRINTF_PRESENCE],
[
  AC_CHECK_FUNCS_ONCE([snprintf])
])

dnl Test whether the string produced by the snprintf function is always NUL
dnl terminated. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_snprintf_truncation_c99.

AC_DEFUN_ONCE([gl_SNPRINTF_TRUNCATION_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_SNPRINTF_PRESENCE])
  AC_CACHE_CHECK([whether snprintf truncates the result as in C99],
    [gl_cv_func_snprintf_truncation_c99],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
#if HAVE_SNPRINTF
# define my_snprintf snprintf
#else
# include <stdarg.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
#endif
static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  my_snprintf (buf, 3, "%d %d", 4567, 89);
  if (memcmp (buf, "45\0DEF", 6) != 0)
    return 1;
  return 0;
}]])],
        [gl_cv_func_snprintf_truncation_c99=yes],
        [gl_cv_func_snprintf_truncation_c99=no],
        [
changequote(,)dnl
         case "$host_os" in
                                 # Guess yes on glibc systems.
           *-gnu* | gnu*)        gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on FreeBSD >= 5.
           freebsd[1-4].*)       gl_cv_func_snprintf_truncation_c99="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_snprintf_truncation_c99="guessing yes";;
           midnightbsd*)         gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_snprintf_truncation_c99="guessing no";;
           darwin*)              gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on OpenBSD >= 3.9.
           openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                                 gl_cv_func_snprintf_truncation_c99="guessing no";;
           openbsd*)             gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on Solaris >= 2.6.
           solaris2.[0-5] | solaris2.[0-5].*)
                                 gl_cv_func_snprintf_truncation_c99="guessing no";;
           solaris*)             gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on AIX >= 4.
           aix[1-3]*)            gl_cv_func_snprintf_truncation_c99="guessing no";;
           aix*)                 gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on HP-UX >= 11.
           hpux[7-9]* | hpux10*) gl_cv_func_snprintf_truncation_c99="guessing no";;
           hpux*)                gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on IRIX >= 6.5.
           irix6.5)              gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on OSF/1 >= 5.
           osf[3-4]*)            gl_cv_func_snprintf_truncation_c99="guessing no";;
           osf*)                 gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                 gl_cv_func_snprintf_truncation_c99="guessing no";;
           netbsd*)              gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on BeOS.
           beos*)                gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess yes on Android.
           linux*-android*)      gl_cv_func_snprintf_truncation_c99="guessing yes";;
                                 # Guess no on native Windows.
           mingw* | windows*)    gl_cv_func_snprintf_truncation_c99="guessing no";;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_snprintf_truncation_c99="$gl_cross_guess_normal";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the return value of the snprintf function is the number
dnl of bytes (excluding the terminating NUL) that would have been produced
dnl if the buffer had been large enough. (ISO C99, POSIX:2001)
dnl For example, this test program fails on IRIX 6.5:
dnl     ---------------------------------------------------------------------
dnl     #include <stdio.h>
dnl     int main()
dnl     {
dnl       static char buf[8];
dnl       int retval = snprintf (buf, 3, "%d", 12345);
dnl       return retval >= 0 && retval < 3;
dnl     }
dnl     ---------------------------------------------------------------------
dnl Result is gl_cv_func_snprintf_retval_c99.

AC_DEFUN_ONCE([gl_SNPRINTF_RETVAL_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_SNPRINTF_PRESENCE])
  AC_CACHE_CHECK([whether snprintf returns a byte count as in C99],
    [gl_cv_func_snprintf_retval_c99],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <string.h>
#if HAVE_SNPRINTF
# define my_snprintf snprintf
#else
# include <stdarg.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
#endif
static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  if (my_snprintf (buf, 3, "%d %d", 4567, 89) != 7)
    return 1;
  if (my_snprintf (buf, 0, "%d %d", 4567, 89) != 7)
    return 2;
  if (my_snprintf (NULL, 0, "%d %d", 4567, 89) != 7)
    return 3;
  return 0;
}]])],
        [gl_cv_func_snprintf_retval_c99=yes],
        [gl_cv_func_snprintf_retval_c99=no],
        [case "$host_os" in
changequote(,)dnl
                                 # Guess yes on glibc systems.
           *-gnu* | gnu*)        gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on FreeBSD >= 5.
           freebsd[1-4].*)       gl_cv_func_snprintf_retval_c99="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_snprintf_retval_c99="guessing yes";;
           midnightbsd*)         gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_snprintf_retval_c99="guessing no";;
           darwin*)              gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on OpenBSD >= 3.9.
           openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                                 gl_cv_func_snprintf_retval_c99="guessing no";;
           openbsd*)             gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on Solaris >= 2.10.
           solaris2.[1-9][0-9]*) gl_cv_func_printf_sizes_c99="guessing yes";;
           solaris*)             gl_cv_func_printf_sizes_c99="guessing no";;
                                 # Guess yes on AIX >= 4.
           aix[1-3]*)            gl_cv_func_snprintf_retval_c99="guessing no";;
           aix*)                 gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                 gl_cv_func_snprintf_retval_c99="guessing no";;
           netbsd*)              gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on BeOS.
           beos*)                gl_cv_func_snprintf_retval_c99="guessing yes";;
                                 # Guess yes on Android.
           linux*-android*)      gl_cv_func_snprintf_retval_c99="guessing yes";;
changequote([,])dnl
                                 # Guess yes on MSVC, no on mingw.
           windows*-msvc*)       gl_cv_func_snprintf_retval_c99="guessing yes" ;;
           mingw* | windows*)    AC_EGREP_CPP([Known], [
#ifdef _MSC_VER
 Known
#endif
                                   ],
                                   [gl_cv_func_snprintf_retval_c99="guessing yes"],
                                   [gl_cv_func_snprintf_retval_c99="guessing no"])
                                 ;;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_snprintf_retval_c99="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the snprintf function supports the %n format directive
dnl also in truncated portions of the format string. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_snprintf_directive_n.

AC_DEFUN([gl_SNPRINTF_DIRECTIVE_N],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_SNPRINTF_PRESENCE])
  AC_CACHE_CHECK([whether snprintf fully supports the 'n' directive],
    [gl_cv_func_snprintf_directive_n],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <signal.h>
#include <stdio.h>
#include <string.h>
#if defined _WIN32 && !defined __CYGWIN__
# include <stdlib.h>
#else
# include <unistd.h>
#endif
#if HAVE_SNPRINTF
# define my_snprintf snprintf
#else
# include <stdarg.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
#endif
static void
abort_handler (int sig)
{
  (void) sig;
  _exit (1);
}
static char fmtstring[10];
static char buf[100];
int main ()
{
  int count = -1;
  signal (SIGABRT, abort_handler);
  /* Copy the format string.  Some systems (glibc with _FORTIFY_SOURCE=2)
     support %n in format strings in read-only memory but not in writable
     memory.  */
  strcpy (fmtstring, "%d %n");
  my_snprintf (buf, 4, fmtstring, 12345, &count, 33, 44, 55);
  if (count != 6)
    return 1;
  return 0;
}]])],
        [gl_cv_func_snprintf_directive_n=yes],
        [gl_cv_func_snprintf_directive_n=no],
        [
         case "$host_os" in
                                 # Guess no on glibc when _FORTIFY_SOURCE >= 2.
           *-gnu* | gnu*)        AC_COMPILE_IFELSE(
                                   [AC_LANG_SOURCE(
                                      [[#if _FORTIFY_SOURCE >= 2
                                         error fail
                                        #endif
                                      ]])],
                                   [gl_cv_func_snprintf_directive_n="guessing yes"],
                                   [gl_cv_func_snprintf_directive_n="guessing no"])
                                 ;;
changequote(,)dnl
                                 # Guess yes on musl systems.
           *-musl* | midipix*)   gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on FreeBSD >= 5.
           freebsd[1-4].*)       gl_cv_func_snprintf_directive_n="guessing no";;
           freebsd* | kfreebsd*) gl_cv_func_snprintf_directive_n="guessing yes";;
           midnightbsd*)         gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)        gl_cv_func_snprintf_directive_n="guessing no";;
           darwin*)              gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on Solaris >= 2.6.
           solaris2.[0-5] | solaris2.[0-5].*)
                                 gl_cv_func_snprintf_directive_n="guessing no";;
           solaris*)             gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on AIX >= 4.
           aix[1-3]*)            gl_cv_func_snprintf_directive_n="guessing no";;
           aix*)                 gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on IRIX >= 6.5.
           irix6.5)              gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on OSF/1 >= 5.
           osf[3-4]*)            gl_cv_func_snprintf_directive_n="guessing no";;
           osf*)                 gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                 gl_cv_func_snprintf_directive_n="guessing no";;
           netbsd*)              gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess yes on BeOS.
           beos*)                gl_cv_func_snprintf_directive_n="guessing yes";;
                                 # Guess no on Android.
           linux*-android*)      gl_cv_func_snprintf_directive_n="guessing no";;
                                 # Guess no on native Windows.
           mingw* | windows*)    gl_cv_func_snprintf_directive_n="guessing no";;
                                 # If we don't know, obey --enable-cross-guesses.
           *)                    gl_cv_func_snprintf_directive_n="$gl_cross_guess_normal";;
changequote([,])dnl
         esac
        ])
    ])
])

dnl Test whether the snprintf function, when passed a size = 1, writes any
dnl output without bounds in this case, behaving like sprintf. This is the
dnl case on Linux libc5.
dnl Result is gl_cv_func_snprintf_size1.

AC_DEFUN([gl_SNPRINTF_SIZE1],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_REQUIRE([gl_SNPRINTF_PRESENCE])
  AC_CACHE_CHECK([whether snprintf respects a size of 1],
    [gl_cv_func_snprintf_size1],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#if HAVE_SNPRINTF
# define my_snprintf snprintf
#else
# include <stdarg.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
#endif
int main()
{
  static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
  my_snprintf (buf, 1, "%d", 12345);
  return buf[1] != 'E';
}]])],
        [gl_cv_func_snprintf_size1=yes],
        [gl_cv_func_snprintf_size1=no],
        [case "$host_os" in
                              # Guess yes on Android.
           linux*-android*)   gl_cv_func_snprintf_size1="guessing yes" ;;
                              # Guess yes on native Windows.
           mingw* | windows*) gl_cv_func_snprintf_size1="guessing yes" ;;
           *)                 gl_cv_func_snprintf_size1="guessing yes" ;;
         esac
        ])
    ])
])

dnl Test whether the vsnprintf function, when passed a zero size, produces no
dnl output. (ISO C99, POSIX:2001)
dnl For example, snprintf nevertheless writes a NUL byte in this case
dnl on OSF/1 5.1:
dnl     ---------------------------------------------------------------------
dnl     #include <stdio.h>
dnl     int main()
dnl     {
dnl       static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
dnl       snprintf (buf, 0, "%d", 12345);
dnl       return buf[0] != 'D';
dnl     }
dnl     ---------------------------------------------------------------------
dnl And vsnprintf writes any output without bounds in this case, behaving like
dnl vsprintf, on HP-UX 11 and OSF/1 5.1:
dnl     ---------------------------------------------------------------------
dnl     #include <stdarg.h>
dnl     #include <stdio.h>
dnl     static int my_snprintf (char *buf, int size, const char *format, ...)
dnl     {
dnl       va_list args;
dnl       int ret;
dnl       va_start (args, format);
dnl       ret = vsnprintf (buf, size, format, args);
dnl       va_end (args);
dnl       return ret;
dnl     }
dnl     int main()
dnl     {
dnl       static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
dnl       my_snprintf (buf, 0, "%d", 12345);
dnl       return buf[0] != 'D';
dnl     }
dnl     ---------------------------------------------------------------------
dnl Result is gl_cv_func_vsnprintf_zerosize_c99.

AC_DEFUN([gl_VSNPRINTF_ZEROSIZE_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether vsnprintf respects a zero size as in C99],
    [gl_cv_func_vsnprintf_zerosize_c99],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdarg.h>
#include <stdio.h>
static int my_snprintf (char *buf, int size, const char *format, ...)
{
  va_list args;
  int ret;
  va_start (args, format);
  ret = vsnprintf (buf, size, format, args);
  va_end (args);
  return ret;
}
int main()
{
  static char buf[8] = { 'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F' };
  my_snprintf (buf, 0, "%d", 12345);
  return buf[0] != 'D';
}]])],
        [gl_cv_func_vsnprintf_zerosize_c99=yes],
        [gl_cv_func_vsnprintf_zerosize_c99=no],
        [
changequote(,)dnl
         case "$host_os" in
                                    # Guess yes on glibc systems.
           *-gnu* | gnu*)           gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on musl systems.
           *-musl* | midipix*)      gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on FreeBSD >= 5.
           freebsd[1-4].*)          gl_cv_func_vsnprintf_zerosize_c99="guessing no";;
           freebsd* | kfreebsd*)    gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
           midnightbsd*)            gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on Mac OS X >= 10.3.
           darwin[1-6].*)           gl_cv_func_vsnprintf_zerosize_c99="guessing no";;
           darwin*)                 gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on Cygwin.
           cygwin*)                 gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on Solaris >= 2.6.
           solaris2.[0-5] | solaris2.[0-5].*)
                                    gl_cv_func_vsnprintf_zerosize_c99="guessing no";;
           solaris*)                gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on AIX >= 4.
           aix[1-3]*)               gl_cv_func_vsnprintf_zerosize_c99="guessing no";;
           aix*)                    gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on IRIX >= 6.5.
           irix6.5)                 gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on NetBSD >= 3.
           netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                                    gl_cv_func_vsnprintf_zerosize_c99="guessing no";;
           netbsd*)                 gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on BeOS.
           beos*)                   gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on Android.
           linux*-android*)         gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # Guess yes on native Windows.
           mingw* | windows* | pw*) gl_cv_func_vsnprintf_zerosize_c99="guessing yes";;
                                    # If we don't know, obey --enable-cross-guesses.
           *)                       gl_cv_func_vsnprintf_zerosize_c99="$gl_cross_guess_normal";;
         esac
changequote([,])dnl
        ])
    ])
])

dnl Test whether the swprintf function works correctly when it produces output
dnl that contains null wide characters.
dnl Result is gl_cv_func_swprintf_works.

AC_DEFUN([gl_SWPRINTF_WORKS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CHECK_FUNCS_ONCE([swprintf])
  AC_CACHE_CHECK([whether swprintf works],
    [gl_cv_func_swprintf_works],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#ifndef __USE_MINGW_ANSI_STDIO
# define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <wchar.h>
int main()
{
  int result = 0;
  { /* This test fails on musl libc 1.2.3, FreeBSD, NetBSD, OpenBSD, macOS, AIX.  */
    wchar_t buf[5] = { 0xBEEF, 0xBEEF, 0xBEEF, 0xBEEF, 0xBEEF };
    int ret = swprintf (buf, 4, L"%cz", '\0');
    /* Expected result:
         ret = 2, buf[0] = 0x0, buf[1] = 0x7a, buf[2] = 0x0, buf[3] = 0xbeef
       musl libc 1.2.3:
         ret = 2, buf[0] = 0x0, buf[1] = 0x0, buf[2] = 0x0, buf[3] = 0x0
         Reported at <https://www.openwall.com/lists/musl/2023/03/22/9>.
       FreeBSD 13.1, NetBSD 9.0, OpenBSD 7.2, macOS 12.5, AIX 7.2:
         ret = 2, buf[0] = 0x0, buf[1] = 0xbeef, buf[2] = 0xbeef, buf[3] = 0xbeef
     */
    if (ret < 0 || buf[1] != 'z')
      result |= 1;
  }
  { /* This test fails on mingw.  */
    wchar_t buf[2];
    int ret = swprintf (buf, 2, L"%lc", (wint_t)0);
    /* Expected: ret = 1
       mingw:    ret = 0
     */
    if (ret != 1)
      result |= 2;
  }
  return result;
}]])],
        [gl_cv_func_swprintf_works=yes],
        [gl_cv_func_swprintf_works=no],
        [case "$host_os" in
                                    # Guess yes on glibc systems.
           *-gnu* | gnu*)           gl_cv_func_swprintf_works="guessing yes";;
                                    # Guess no on musl systems.
           *-musl* | midipix*)      gl_cv_func_swprintf_works="guessing no";;
                                    # Guess no on FreeBSD, NetBSD, OpenBSD, macOS, AIX.
           freebsd* | midnightbsd* | netbsd* | openbsd* | darwin* | aix*)
                                    gl_cv_func_swprintf_works="guessing no";;
                                    # Guess no on native Windows.
           mingw* | windows* | pw*) gl_cv_func_swprintf_works="guessing no";;
                                    # If we don't know, obey --enable-cross-guesses.
           *)                       gl_cv_func_swprintf_works="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *wprintf family of functions supports the 'a' and 'A'
dnl conversion specifier for hexadecimal output of 'long double' numbers.
dnl (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_swprintf_directive_la.

AC_DEFUN([gl_SWPRINTF_DIRECTIVE_LA],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether swprintf supports the 'La' and 'LA' directives],
    [gl_cv_func_swprintf_directive_la],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <wchar.h>
static wchar_t buf[100];
int main ()
{
  int result = 0;
  /* This catches a glibc 2.15, Haiku 2022, NetBSD 10.0 bug.  */
  if (swprintf (buf, sizeof (buf) / sizeof (wchar_t),
                L"%La %d", 3.1416015625L, 33, 44, 55) < 0
      || (wcscmp (buf, L"0x1.922p+1 33") != 0
          && wcscmp (buf, L"0x3.244p+0 33") != 0
          && wcscmp (buf, L"0x6.488p-1 33") != 0
          && wcscmp (buf, L"0xc.91p-2 33") != 0))
    result |= 1;
  return result;
}]])],
        [gl_cv_func_swprintf_directive_la=yes],
        [gl_cv_func_swprintf_directive_la=no],
        [case "$host_os" in
                               # Guess yes on glibc >= 2.17 systems.
           *-gnu* | gnu*)
             AC_EGREP_CPP([Unlucky], [
               #include <features.h>
               #ifdef __GNU_LIBRARY__
                #if ((__GLIBC__ == 2 && __GLIBC_MINOR__ >= 16) || (__GLIBC__ > 2)) && !defined __UCLIBC__
                 Unlucky
                #endif
               #endif
               ],
               [gl_cv_func_swprintf_directive_la="guessing yes"],
               [gl_cv_func_swprintf_directive_la="guessing no"])
             ;;
                               # Guess yes on musl systems.
           *-musl* | midipix*) gl_cv_func_swprintf_directive_la="guessing yes";;
                               # Guess yes on Android.
           linux*-android*)    gl_cv_func_swprintf_directive_la="guessing yes";;
                               # Guess no on NetBSD.
           netbsd*)            gl_cv_func_swprintf_directive_la="guessing no";;
                               # Guess no on native Windows.
           mingw* | windows*)  gl_cv_func_swprintf_directive_la="guessing no";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_swprintf_directive_la="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl Test whether the *wprintf family of functions supports the 'lc' conversion
dnl specifier for all wide characters.
dnl (ISO C11, POSIX:2001)
dnl Result is gl_cv_func_swprintf_directive_lc.

AC_DEFUN([gl_SWPRINTF_DIRECTIVE_LC],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether swprintf supports the 'lc' directive],
    [gl_cv_func_swprintf_directive_lc],
    [
      AC_RUN_IFELSE(
        [AC_LANG_SOURCE([[
#include <stdio.h>
#include <wchar.h>
static wchar_t buf[100];
static wint_t L_invalid = (wchar_t) 0x76543210;
int main ()
{
  int result = 0;
  /* This catches a musl libc 1.2.4, Android bug.
     Reported at <https://www.openwall.com/lists/musl/2023/06/12/3>. */
  if (swprintf (buf, sizeof (buf) / sizeof (wchar_t),
                L"%lc %d", L_invalid, 33, 44, 55) < 0)
    result |= 1;
  return result;
}]])],
        [gl_cv_func_swprintf_directive_lc=yes],
        [gl_cv_func_swprintf_directive_lc=no],
        [case "$host_os" in
                               # Guess yes on glibc systems.
           *-gnu* | gnu*)      gl_cv_func_swprintf_directive_lc="guessing yes";;
                               # Guess no on musl systems.
           *-musl* | midipix*) gl_cv_func_swprintf_directive_lc="guessing no";;
                               # Guess no on Android.
           linux*-android*)    gl_cv_func_swprintf_directive_lc="guessing no";;
                               # Guess yes on native Windows.
           mingw* | windows*)  gl_cv_func_swprintf_directive_lc="guessing yes";;
                               # If we don't know, obey --enable-cross-guesses.
           *)                  gl_cv_func_swprintf_directive_lc="$gl_cross_guess_normal";;
         esac
        ])
    ])
])

dnl The results of these tests on various platforms are:
dnl
dnl 1 = gl_PRINTF_SIZES_C99
dnl 2 = gl_PRINTF_SIZES_C23
dnl 3 = gl_PRINTF_LONG_DOUBLE
dnl 4 = gl_PRINTF_INFINITE
dnl 5 = gl_PRINTF_INFINITE_LONG_DOUBLE
dnl 6 = gl_PRINTF_DIRECTIVE_A
dnl 7 = gl_PRINTF_DIRECTIVE_B
dnl 8 = gl_PRINTF_DIRECTIVE_UPPERCASE_B
dnl 9 = gl_PRINTF_DIRECTIVE_F
dnl 10 = gl_PRINTF_DIRECTIVE_N
dnl 11 = gl_PRINTF_DIRECTIVE_LS
dnl 12 = gl_PRINTF_DIRECTIVE_LC
dnl 13 = gl_PRINTF_POSITIONS
dnl 14 = gl_PRINTF_FLAG_GROUPING
dnl 15 = gl_PRINTF_FLAG_GROUPING_INT_PRECISION
dnl 16 = gl_PRINTF_FLAG_GROUPING_MULTIBYTE
dnl 17 = gl_PRINTF_FLAG_LEFTADJUST
dnl 18 = gl_PRINTF_FLAG_ZERO
dnl 19 = gl_PRINTF_FLAG_ALT_PRECISION_ZERO
dnl 20 = gl_PRINTF_PRECISION
dnl 21 = gl_PRINTF_ENOMEM
dnl 22 = gl_SNPRINTF_PRESENCE
dnl 23 = gl_SNPRINTF_TRUNCATION_C99
dnl 24 = gl_SNPRINTF_RETVAL_C99
dnl 25 = gl_SNPRINTF_DIRECTIVE_N
dnl 26 = gl_SNPRINTF_SIZE1
dnl 27 = gl_VSNPRINTF_ZEROSIZE_C99
dnl 28 = gl_SWPRINTF_WORKS
dnl 29 = gl_SWPRINTF_DIRECTIVE_LA
dnl 30 = gl_SWPRINTF_DIRECTIVE_LC
dnl
dnl 1 = checking whether printf supports size specifiers as in C99...
dnl 2 = checking whether printf supports size specifiers as in C23...
dnl 3 = checking whether printf supports 'long double' arguments...
dnl 4 = checking whether printf supports infinite 'double' arguments...
dnl 5 = checking whether printf supports infinite 'long double' arguments...
dnl 6 = checking whether printf supports the 'a' and 'A' directives...
dnl 7 = checking whether printf supports the 'b' directive...
dnl 8 = checking whether printf supports the 'B' directive...
dnl 9 = checking whether printf supports the 'F' directive...
dnl 10 = checking whether printf supports the 'n' directive...
dnl 11 = checking whether printf supports the 'ls' directive...
dnl 12 = checking whether printf supports the 'lc' directive correctly...
dnl 13 = checking whether printf supports POSIX/XSI format strings with positions...
dnl 14 = checking whether printf supports the grouping flag...
dnl 15 = checking whether printf supports grouping on integers with a precision...
dnl 16 = checking whether printf supports grouping with a multibyte separator...
dnl 17 = checking whether printf supports the left-adjust flag correctly...
dnl 18 = checking whether printf supports the zero flag correctly...
dnl 19 = checking whether printf supports the alternative flag with a zero precision...
dnl 20 = checking whether printf supports large precisions...
dnl 21 = checking whether printf survives out-of-memory conditions...
dnl 22 = checking for snprintf...
dnl 23 = checking whether snprintf truncates the result as in C99...
dnl 24 = checking whether snprintf returns a byte count as in C99...
dnl 25 = checking whether snprintf fully supports the 'n' directive...
dnl 26 = checking whether snprintf respects a size of 1...
dnl 27 = checking whether vsnprintf respects a zero size as in C99...
dnl 28 = checking whether swprintf works...
dnl 29 = checking whether swprintf supports the 'La' and 'LA' directives...
dnl 30 = checking whether swprintf supports the 'lc' directive...
dnl
dnl . = yes, # = no.
dnl
dnl                                  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
dnl   musl libc 1.2.3                .  #  .  .  .  .  #  #  .  .  .  #  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  #  .  #
dnl   glibc 2.35                     .  #  .  .  .  .  .  .  .  .  .  .  .  .  #  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .
dnl   glibc 2.5                      .  #  .  .  .  .  #  #  .  .  .  .  .  .  #  .  .  .  .  .  .  .  .  .  .  .  .  .  #  .
dnl   glibc 2.3.6                    .  #  .  .  .  #  #  #  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  #  .
dnl   FreeBSD 14.0                   .  .  .  .  .  #  .  .  .  .  .  .  .  .  #  .  .  .  .  .  #  .  .  .  .  .  .  #  .  #
dnl   FreeBSD 13.0                   .  #  .  .  .  #  #  #  .  .  .  .  .  .  #  .  .  .  .  .  #  .  .  .  .  .  .  #  .  #
dnl   FreeBSD 5.4, 6.1               .  #  .  .  .  #  #  #  .  .  .  .  .  .  .  .  .  #  ?  .  #  .  .  .  .  .  .  #  ?  ?
dnl   Mac OS X 10.13.5               .  #  .  .  #  #  #  #  .  #  .  .  .  .  ?  ?  .  .  .  .  .  .  .  .  #  .  .  #  ?  ?
dnl   Mac OS X 10.5.8                .  #  .  .  #  #  #  #  .  .  .  .  .  .  ?  ?  .  #  #  .  .  .  .  .  .  .  .  #  ?  ?
dnl   Mac OS X 10.3.9                .  #  .  .  .  #  #  #  .  .  .  .  .  .  ?  ?  .  #  #  .  #  .  .  .  .  .  .  #  ?  ?
dnl   OpenBSD 6.0, 6.7               .  #  .  .  .  #  #  #  .  .  .  .  .  .  .  .  .  .  .  .  #  .  .  .  .  .  .  #  .  #
dnl   OpenBSD 3.9, 4.0               .  #  .  #  #  #  #  #  #  .  #  .  .  #  ?  ?  .  #  ?  .  #  .  .  .  .  .  .  #  ?  ?
dnl   Cygwin 1.7.0 (2009)            .  #  .  .  #  .  #  #  .  .  ?  ?  .  .  ?  ?  .  .  ?  .  ?  .  .  .  .  .  .  ?  ?  ?
dnl   Cygwin 1.5.25 (2008)           .  #  .  .  #  #  #  #  .  .  #  ?  .  .  ?  ?  .  .  ?  .  #  .  .  .  .  .  .  ?  ?  ?
dnl   Cygwin 1.5.19 (2006)           #  #  .  .  #  #  #  #  #  .  #  ?  .  #  ?  ?  .  #  ?  #  #  .  .  .  .  .  .  ?  ?  ?
dnl   Solaris 11.4                   .  #  .  #  #  #  #  #  .  .  #  .  .  .  #  #  .  #  .  .  .  .  .  .  .  .  .  .  #  .
dnl   Solaris 11.3                   .  #  .  .  .  #  #  #  .  .  #  .  .  .  ?  ?  .  .  .  .  .  .  .  .  .  .  .  .  #  .
dnl   Solaris 11.0                   .  #  .  #  #  #  #  #  .  .  #  .  .  .  ?  ?  .  #  .  .  .  .  .  .  .  .  .  ?  ?  ?
dnl   Solaris 10                     .  #  .  #  #  #  #  #  .  .  #  .  .  .  #  #  .  #  .  #  .  .  .  .  .  .  .  .  #  .
dnl   Solaris 2.6 ... 9              #  #  .  #  #  #  #  #  #  .  #  .  .  .  ?  ?  .  #  ?  #  .  .  .  #  .  .  .  ?  ?  ?
dnl   Solaris 2.5.1                  #  #  .  #  #  #  #  #  #  .  #  .  .  .  ?  ?  .  #  ?  .  .  #  #  #  #  #  #  ?  ?  ?
dnl   AIX 7.1                        .  #  .  #  #  #  #  #  .  .  .  .  .  .  .  .  .  #  .  #  .  .  .  .  .  .  .  #  .  .
dnl   AIX 5.2                        .  #  .  #  #  #  #  #  .  .  .  .  .  .  ?  ?  .  #  ?  .  .  .  .  .  .  .  .  #  ?  ?
dnl   AIX 4.3.2, 5.1                 #  #  .  #  #  #  #  #  #  .  .  .  .  .  ?  ?  .  #  ?  .  .  .  .  #  .  .  .  #  ?  ?
dnl   HP-UX 11.31                    .  #  .  .  .  #  #  #  .  .  .  ?  .  .  ?  ?  .  #  ?  .  .  .  .  #  #  .  .  ?  ?  ?
dnl   HP-UX 11.{00,11,23}            #  #  .  .  .  #  #  #  #  .  .  ?  .  .  ?  ?  .  #  ?  .  .  .  .  #  #  .  #  ?  ?  ?
dnl   HP-UX 10.20                    #  #  .  #  .  #  #  #  #  .  ?  ?  .  .  ?  ?  #  #  ?  .  .  .  .  #  #  ?  #  ?  ?  ?
dnl   IRIX 6.5                       #  #  .  #  #  #  #  #  #  .  #  .  .  .  ?  ?  .  #  ?  .  .  .  .  #  .  .  .  #  ?  ?
dnl   OSF/1 5.1                      #  #  .  #  #  #  #  #  #  .  .  ?  .  .  ?  ?  .  #  ?  .  .  .  .  #  .  .  #  ?  ?  ?
dnl   OSF/1 4.0d                     #  #  .  #  #  #  #  #  #  .  .  ?  .  .  ?  ?  .  #  ?  .  .  #  #  #  #  #  #  ?  ?  ?
dnl   NetBSD 9.0                     .  #  .  .  .  #  #  #  .  .  .  .  .  .  #  #  .  .  .  .  .  .  .  .  .  .  .  #  .  #
dnl   NetBSD 5.0                     .  #  .  .  #  #  #  #  .  .  .  .  .  .  ?  ?  .  #  ?  .  #  .  .  .  .  .  .  #  ?  ?
dnl   NetBSD 4.0                     .  #  ?  ?  ?  ?  #  #  ?  .  ?  .  .  ?  ?  ?  ?  ?  ?  ?  ?  .  .  .  ?  ?  ?  #  ?  ?
dnl   NetBSD 3.0                     .  #  .  .  .  #  #  #  #  .  ?  .  #  #  ?  ?  ?  #  ?  .  #  .  .  .  .  .  .  #  ?  ?
dnl   Haiku                          .  #  .  .  #  #  #  #  #  .  #  ?  .  .  #  .  .  .  ?  .  ?  .  .  ?  .  .  .  .  #  .
dnl   BeOS                           #  #  #  .  #  #  #  #  #  .  ?  ?  #  .  ?  ?  ?  .  ?  #  ?  .  .  ?  .  .  .  ?  ?  ?
dnl   Android 4.3                    .  #  .  #  #  #  #  #  #  #  #  ?  .  #  ?  ?  .  #  ?  .  #  .  .  .  #  .  .  ?  ?  ?
dnl   old mingw / msvcrt             #  #  #  #  #  #  #  #  #  .  .  ?  #  #  ?  ?  .  #  ?  #  ?  .  #  #  #  .  .  #  ?  ?
dnl   MSVC 9                         #  #  #  #  #  #  #  #  #  #  .  ?  #  #  ?  ?  .  #  ?  #  ?  #  #  #  #  .  .  #  ?  ?
dnl   mingw 2009-2011                .  #  #  .  #  .  #  #  .  .  .  ?  #  #  ?  ?  .  .  ?  .  ?  .  .  .  .  .  .  #  ?  ?
dnl   mingw-w64 2011                 #  #  #  #  #  #  #  #  #  .  .  ?  #  #  ?  ?  .  #  ?  #  ?  .  #  #  #  .  .  #  ?  ?
