# printf.m4 serial 2
dnl Copyright (C) 2003, 2007 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Test whether the *printf family of functions supports the 'j', 'z', 't',
dnl 'L' size specifiers. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_sizes_c99.

AC_DEFUN([gl_PRINTF_SIZES_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gl_AC_HEADER_STDINT_H])
  AC_REQUIRE([gl_AC_HEADER_INTTYPES_H])
  AC_REQUIRE([gt_TYPE_LONGDOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports size specifiers as in C99],
    [gl_cv_func_printf_sizes_c99], 
    [
      AC_TRY_RUN([
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
#if HAVE_STDINT_H_WITH_UINTMAX || HAVE_INTTYPES_H_WITH_UINTMAX
  buf[0] = '\0';
  if (sprintf (buf, "%ju %d", (uintmax_t) 12345671, 33, 44, 55) < 0
      || strcmp (buf, "12345671 33") != 0)
    return 1;
#endif
  buf[0] = '\0';
  if (sprintf (buf, "%zu %d", (size_t) 12345672, 33, 44, 55) < 0
      || strcmp (buf, "12345672 33") != 0)
    return 1;
  buf[0] = '\0';
  if (sprintf (buf, "%tu %d", (ptrdiff_t) 12345673, 33, 44, 55) < 0
      || strcmp (buf, "12345673 33") != 0)
    return 1;
#if HAVE_LONG_DOUBLE
  buf[0] = '\0';
  if (sprintf (buf, "%Lg %d", (long double) 1.5, 33, 44, 55) < 0
      || strcmp (buf, "1.5 33") != 0)
    return 1;
#endif
  return 0;
}], [gl_cv_func_printf_sizes_c99=yes], [gl_cv_func_printf_sizes_c99=no],
      [
changequote(,)dnl
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_printf_sizes_c99="guessing yes";;
                               # Guess yes on FreeBSD >= 5.
         freebsd[1-4]*)        gl_cv_func_printf_sizes_c99="guessing no";;
         freebsd* | kfreebsd*) gl_cv_func_printf_sizes_c99="guessing yes";;
                               # Guess yes on MacOS X >= 10.3.
         darwin[1-6].*)        gl_cv_func_printf_sizes_c99="guessing no";;
         darwin*)              gl_cv_func_printf_sizes_c99="guessing yes";;
                               # Guess yes on OpenBSD >= 3.9.
         openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                               gl_cv_func_printf_sizes_c99="guessing no";;
         openbsd*)             gl_cv_func_printf_sizes_c99="guessing yes";;
                               # Guess yes on Solaris >= 2.10.
         solaris2.[0-9]*)      gl_cv_func_printf_sizes_c99="guessing no";;
         solaris*)             gl_cv_func_printf_sizes_c99="guessing yes";;
                               # Guess yes on NetBSD >= 3.
         netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                               gl_cv_func_printf_sizes_c99="guessing no";;
         netbsd*)              gl_cv_func_printf_sizes_c99="guessing yes";;
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_printf_sizes_c99="guessing no";;
       esac
changequote([,])dnl
      ])
    ])
])

dnl Test whether the *printf family of functions supports the 'a' and 'A'
dnl conversion specifier for hexadecimal output of floating-point numbers.
dnl (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_printf_directive_a.

AC_DEFUN([gl_PRINTF_DIRECTIVE_A],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([gt_TYPE_LONGDOUBLE])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports the 'a' and 'A' directives],
    [gl_cv_func_printf_directive_a], 
    [
      AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  if (sprintf (buf, "%a %d", 3.1416015625, 33, 44, 55) < 0
      || (strcmp (buf, "0x1.922p+1 33") != 0
          && strcmp (buf, "0x3.244p+0 33") != 0
          && strcmp (buf, "0x6.488p-1 33") != 0
          && strcmp (buf, "0xc.91p-2 33") != 0))
    return 1;
  if (sprintf (buf, "%A %d", -3.1416015625, 33, 44, 55) < 0
      || (strcmp (buf, "-0X1.922P+1 33") != 0
          && strcmp (buf, "-0X3.244P+0 33") != 0
          && strcmp (buf, "-0X6.488P-1 33") != 0
          && strcmp (buf, "-0XC.91P-2 33") != 0))
    return 1;
  /* This catches a MacOS X 10.3.9 (Darwin 7.9) bug.  */
  if (sprintf (buf, "%.1a", 1.999) < 0
      || (strcmp (buf, "0x1.0p+1") != 0
          && strcmp (buf, "0x2.0p+0") != 0
          && strcmp (buf, "0x4.0p-1") != 0
          && strcmp (buf, "0x8.0p-2") != 0))
    return 1;
#if HAVE_LONG_DOUBLE
  /* This catches the same MacOS X 10.3.9 (Darwin 7.9) bug and also a
     glibc 2.4 bug <http://sourceware.org/bugzilla/show_bug.cgi?id=2908>.  */
  if (sprintf (buf, "%.1La", 1.999L) < 0
      || (strcmp (buf, "0x1.0p+1") != 0
          && strcmp (buf, "0x2.0p+0") != 0
          && strcmp (buf, "0x4.0p-1") != 0
          && strcmp (buf, "0x8.0p-2") != 0))
    return 1;
#endif
  return 0;
}], [gl_cv_func_printf_directive_a=yes], [gl_cv_func_printf_directive_a=no],
      [
       case "$host_os" in
                               # Guess yes on glibc >= 2.5 systems.
         *-gnu*)
           AC_EGREP_CPP([BZ2908], [
             #include <features.h>
             #ifdef __GNU_LIBRARY__
              #if (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 5) || (__GLIBC__ > 2)
               BZ2908
              #endif
             #endif
             ],
             [gl_cv_func_printf_directive_a="guessing yes"],
             [gl_cv_func_printf_directive_a="guessing no"])
           ;;
changequote(,)dnl
                               # Guess yes on FreeBSD >= 5.
         freebsd[1-4]*)        gl_cv_func_printf_directive_a="guessing no";;
         freebsd* | kfreebsd*) gl_cv_func_printf_directive_a="guessing yes";;
                               # Guess yes on NetBSD >= 4.
         netbsd[1-3]* | netbsdelf[1-3]* | netbsdaout[1-3]* | netbsdcoff[1-3]*)
                               gl_cv_func_printf_directive_a="guessing no";;
         netbsd*)              gl_cv_func_printf_directive_a="guessing yes";;
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_printf_directive_a="guessing no";;
       esac
changequote([,])dnl
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
      AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  int count = -1;
  if (sprintf (buf, "%d %n", 123, &count, 33, 44, 55) < 0
      || strcmp (buf, "123 ") != 0
      || count != 4)
    return 1;
  return 0;
}], [gl_cv_func_printf_directive_n=yes], [gl_cv_func_printf_directive_n=no],
      [
changequote(,)dnl
       case "$host_os" in
         *)     gl_cv_func_printf_directive_n="guessing yes";;
       esac
changequote([,])dnl
      ])
    ])
])

dnl Test whether the *printf family of functions supports POSIX/XSI format
dnl strings with positions. (POSIX:2001)
dnl Result is gl_cv_func_printf_positions.

AC_DEFUN([gl_PRINTF_POSITIONS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether printf supports POSIX/XSI format strings with positions],
    [gl_cv_func_printf_positions], 
    [
      AC_TRY_RUN([
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
}], [gl_cv_func_printf_positions=yes], [gl_cv_func_printf_positions=no],
      [
changequote(,)dnl
       case "$host_os" in
         netbsd[1-3]* | netbsdelf[1-3]* | netbsdaout[1-3]* | netbsdcoff[1-3]*)
                       gl_cv_func_printf_positions="guessing no";;
         beos*)        gl_cv_func_printf_positions="guessing no";;
         mingw* | pw*) gl_cv_func_printf_positions="guessing no";;
         *)            gl_cv_func_printf_positions="guessing yes";;
       esac
changequote([,])dnl
      ])
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

AC_DEFUN([gl_SNPRINTF_TRUNCATION_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether snprintf truncates the result as in C99],
    [gl_cv_func_snprintf_truncation_c99], 
    [
      AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  snprintf (buf, 3, "%d %d", 4567, 89);
  if (memcmp (buf, "45\0DEF", 6) != 0)
    return 1;
  return 0;
}], [gl_cv_func_snprintf_truncation_c99=yes], [gl_cv_func_snprintf_truncation_c99=no],
      [
changequote(,)dnl
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_snprintf_truncation_c99="guessing yes";;
                               # Guess yes on FreeBSD >= 5.
         freebsd[1-4]*)        gl_cv_func_snprintf_truncation_c99="guessing no";;
         freebsd* | kfreebsd*) gl_cv_func_snprintf_truncation_c99="guessing yes";;
                               # Guess yes on MacOS X >= 10.3.
         darwin[1-6].*)        gl_cv_func_snprintf_truncation_c99="guessing no";;
         darwin*)              gl_cv_func_snprintf_truncation_c99="guessing yes";;
                               # Guess yes on OpenBSD >= 3.9.
         openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                               gl_cv_func_snprintf_truncation_c99="guessing no";;
         openbsd*)             gl_cv_func_snprintf_truncation_c99="guessing yes";;
                               # Guess yes on Solaris >= 2.6.
         solaris2.[0-5]*)      gl_cv_func_snprintf_truncation_c99="guessing no";;
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
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_snprintf_truncation_c99="guessing no";;
       esac
changequote([,])dnl
      ])
    ])
])

dnl Test whether the return value of the snprintf function is the number
dnl of bytes (excluding the terminating NUL) that would have been produced
dnl if the buffer had been large enough. (ISO C99, POSIX:2001)
dnl Result is gl_cv_func_snprintf_retval_c99.

AC_DEFUN([gl_SNPRINTF_RETVAL_C99],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether snprintf returns a byte count as in C99],
    [gl_cv_func_snprintf_retval_c99], 
    [
      AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  strcpy (buf, "ABCDEF");
  if (snprintf (buf, 3, "%d %d", 4567, 89) != 7)
    return 1;
  return 0;
}], [gl_cv_func_snprintf_retval_c99=yes], [gl_cv_func_snprintf_retval_c99=no],
      [
changequote(,)dnl
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on FreeBSD >= 5.
         freebsd[1-4]*)        gl_cv_func_snprintf_retval_c99="guessing no";;
         freebsd* | kfreebsd*) gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on MacOS X >= 10.3.
         darwin[1-6].*)        gl_cv_func_snprintf_retval_c99="guessing no";;
         darwin*)              gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on OpenBSD >= 3.9.
         openbsd[1-2].* | openbsd3.[0-8] | openbsd3.[0-8].*)
                               gl_cv_func_snprintf_retval_c99="guessing no";;
         openbsd*)             gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on Solaris >= 2.6.
         solaris2.[0-5]*)      gl_cv_func_snprintf_retval_c99="guessing no";;
         solaris*)             gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on AIX >= 4.
         aix[1-3]*)            gl_cv_func_snprintf_retval_c99="guessing no";;
         aix*)                 gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on NetBSD >= 3.
         netbsd[1-2]* | netbsdelf[1-2]* | netbsdaout[1-2]* | netbsdcoff[1-2]*)
                               gl_cv_func_snprintf_retval_c99="guessing no";;
         netbsd*)              gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # Guess yes on BeOS.
         beos*)                gl_cv_func_snprintf_retval_c99="guessing yes";;
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_snprintf_retval_c99="guessing no";;
       esac
changequote([,])dnl
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
  AC_CACHE_CHECK([whether snprintf fully supports the 'n' directive],
    [gl_cv_func_snprintf_directive_n], 
    [
      AC_TRY_RUN([
#include <stdio.h>
#include <string.h>
static char buf[100];
int main ()
{
  int count = -1;
  snprintf (buf, 4, "%d %n", 12345, &count, 33, 44, 55);
  if (count != 6)
    return 1;
  return 0;
}], [gl_cv_func_snprintf_directive_n=yes], [gl_cv_func_snprintf_directive_n=no],
      [
changequote(,)dnl
       case "$host_os" in
                               # Guess yes on glibc systems.
         *-gnu*)               gl_cv_func_snprintf_directive_n="guessing yes";;
                               # Guess yes on FreeBSD >= 5.
         freebsd[1-4]*)        gl_cv_func_snprintf_directive_n="guessing no";;
         freebsd* | kfreebsd*) gl_cv_func_snprintf_directive_n="guessing yes";;
                               # Guess yes on MacOS X >= 10.3.
         darwin[1-6].*)        gl_cv_func_snprintf_directive_n="guessing no";;
         darwin*)              gl_cv_func_snprintf_directive_n="guessing yes";;
                               # Guess yes on Solaris >= 2.6.
         solaris2.[0-5]*)      gl_cv_func_snprintf_directive_n="guessing no";;
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
                               # If we don't know, assume the worst.
         *)                    gl_cv_func_snprintf_directive_n="guessing no";;
       esac
changequote([,])dnl
      ])
    ])
])

dnl The results of these tests on various platforms are:
dnl
dnl 1 = gl_PRINTF_SIZES_C99
dnl 2 = gl_PRINTF_DIRECTIVE_A
dnl 3 = gl_PRINTF_DIRECTIVE_N
dnl 4 = gl_PRINTF_POSITIONS
dnl 5 = gl_SNPRINTF_PRESENCE
dnl 6 = gl_SNPRINTF_TRUNCATION_C99
dnl 7 = gl_SNPRINTF_RETVAL_C99
dnl 8 = gl_SNPRINTF_DIRECTIVE_N
dnl
dnl 1 = checking whether printf supports size specifiers as in C99...
dnl 2 = checking whether printf supports the 'a' and 'A' directives...
dnl 3 = checking whether printf supports the 'n' directive...
dnl 4 = checking whether printf supports POSIX/XSI format strings with positions...
dnl 5 = checking for snprintf...
dnl 6 = checking whether snprintf truncates the result as in C99...
dnl 7 = checking whether snprintf returns a byte count as in C99...
dnl 8 = checking whether snprintf fully supports the 'n' directive...
dnl
dnl . = yes, # = no.
dnl
dnl                                        1  2  3  4  5  6  7  8
dnl   glibc 2.5                            .  .  .  .  .  .  .  .
dnl   glibc 2.3.6                          .  #  .  .  .  .  .  .
dnl   FreeBSD 5.4, 6.1                     .  .  .  .  .  .  .  .
dnl   MacOS X 10.3.9                       .  #  .  .  .  .  .  .
dnl   OpenBSD 3.9                          .  #  .  .  .  .  .  ?
dnl   Cygwin 2007                          .  #  .  .  .  .  .  ?
dnl   Cygwin 2006                          #  #  .  .  .  .  .  ?
dnl   Solaris 10                           .  #  .  .  .  .  .  .
dnl   Solaris 2.6 ... 9                    #  #  .  .  .  .  .  .
dnl   Solaris 2.5.1                        #  #  .  .  #  #  #  #
dnl   AIX 4.3.2, 5.1                       #  #  .  .  .  .  .  .
dnl   HP-UX 11.31                          .  #  .  .  .  .  #  #
dnl   HP-UX 10.20, 11.00, 11.11, 11.23     #  #  .  .  .  .  #  #
dnl   IRIX 6.5                             #  #  .  .  .  .  #  .
dnl   OSF/1 5.1                            #  #  .  .  .  .  #  .
dnl   OSF/1 4.0d                           #  #  .  .  #  #  #  #
dnl   NetBSD 4.0                           .  .  .  .  .  .  .  ?
dnl   NetBSD 3.0                           .  #  .  #  .  .  .  .
dnl   BeOS                                 #  #  .  #  .  .  .  .
dnl   mingw                                #  #  .  #  .  #  #  #
