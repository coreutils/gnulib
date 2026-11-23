# fcntl-o.m4
# serial 13
dnl Copyright (C) 2006, 2009-2026 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl This file is offered as-is, without any warranty.

dnl Written by Paul Eggert.

AC_PREREQ([2.60])

# Test whether the flags O_DIRECTORY, O_NOATIME and O_NOFOLLOW actually work.
# Define HAVE_WORKING_O_DIRECTORY to 1 if O_DIRECTORY works, or to 0 otherwise.
# Define HAVE_WORKING_O_NOATIME to 1 if O_NOATIME works, or to 0 otherwise.
# Define HAVE_WORKING_O_NOFOLLOW to 1 if O_NOFOLLOW works, or to 0 otherwise.
AC_DEFUN([gl_FCNTL_O_FLAGS],
[
  dnl Persuade glibc <fcntl.h> to define O_NOATIME and O_NOFOLLOW.
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CHECK_HEADERS_ONCE([unistd.h])
  AC_CHECK_FUNCS_ONCE([symlink])
  AC_CACHE_CHECK([for working fcntl.h], [gl_cv_header_working_fcntl_h],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <sys/types.h>
           #include <sys/stat.h>
           #if HAVE_UNISTD_H
           # include <unistd.h>
           #else /* on Windows with MSVC */
           # include <io.h>
           # include <stdlib.h>
           # define sleep(n) _sleep ((n) * 1000)
           #endif
           #include <errno.h>
           #include <fcntl.h>
           ]GL_MDA_DEFINES[
           #ifndef O_DIRECTORY
            #define O_DIRECTORY 0
           #endif
           #ifndef O_NOATIME
            #define O_NOATIME 0
           #endif
           #ifndef O_NOFOLLOW
            #define O_NOFOLLOW 0
           #endif
           #ifndef O_SEARCH
            #define O_SEARCH O_RDONLY
           #endif
           static int const constants[] =
            {
              O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC, O_APPEND,
              O_NONBLOCK, O_SYNC, O_ACCMODE, O_RDONLY, O_RDWR, O_WRONLY
            };
          ]],
          [[
            int result = !constants;
            #if HAVE_SYMLINK
            {
              static char const sym[] = "conftest.sym";
              if (symlink ("/dev/null", sym) != 0)
                result |= 1;
              else
                {
                  int fd = open (sym, O_WRONLY | O_NOFOLLOW | O_CREAT, 0);
                  if (fd >= 0)
                    {
                      close (fd);
                      result |= 3;
                    }
                }
              if (unlink (sym) != 0 || symlink (".", sym) != 0)
                result |= 1;
              else
                {
                  int fd = open (sym, O_RDONLY | O_NOFOLLOW);
                  if (fd >= 0)
                    {
                      close (fd);
                      result |= 3;
                    }
                }
              unlink (sym);
            }
            #endif
            {
              int fd = open ("confdefs.h", O_SEARCH | O_DIRECTORY);
              if (!(fd < 0 && errno == ENOTDIR))
                result |= 4;
              if (0 <= fd)
                close (fd);
            }
            {
              static char const file[] = "confdefs.h";
              int fd = open (file, O_RDONLY | O_NOATIME);
              if (fd < 0)
                result |= 8;
              else
                {
                  struct stat st0;
                  if (fstat (fd, &st0) != 0)
                    result |= 16;
                  else
                    {
                      char c;
                      sleep (1);
                      if (read (fd, &c, 1) != 1)
                        result |= 24;
                      else
                        {
                          if (close (fd) != 0)
                            result |= 32;
                          else
                            {
                              struct stat st1;
                              if (stat (file, &st1) != 0)
                                result |= 40;
                              else
                                if (st0.st_atime != st1.st_atime)
                                  result |= 64;
                            }
                        }
                    }
                }
            }
            return result;]])],
       [gl_cv_header_working_fcntl_h=yes],
       [AS_CASE([$?],
          dnl We cannot catch exit code 1 or 2 here, because
          dnl - exit code 1 can occur through a compilation error on mingw (e.g.
          dnl   when O_NOCTTY, O_NONBLOCK, O_SYNC are not defined) or when
          dnl   result = 1, whereas
          dnl - exit code 2 can occur through a compilation error on MSVC (e.g.
          dnl   again when O_NOCTTY, O_NONBLOCK, O_SYNC are not defined) or when
          dnl   result = 2.
          [ 3], [gl_cv_header_working_fcntl_h="no (bad O_NOFOLLOW)"],
          [ 4], [gl_cv_header_working_fcntl_h="no (bad O_DIRECTORY)"],
          [ 7], [gl_cv_header_working_fcntl_h="no (bad O_NOFOLLOW, O_DIRECTORY)"],
          [64], [gl_cv_header_working_fcntl_h="no (bad O_NOATIME)"],
          [67], [gl_cv_header_working_fcntl_h="no (bad O_NOFOLLOW, O_NOATIME)"],
          [68], [gl_cv_header_working_fcntl_h="no (bad O_DIRECTORY, O_NOATIME)"],
          [71], [gl_cv_header_working_fcntl_h="no (bad O_NOFOLLOW, O_DIRECTORY, O_NOATIME)"],
          [gl_cv_header_working_fcntl_h="no"])],
       [AS_CASE([$host_os,$gl_cross_guess_normal],
          # The O_DIRECTORY test is known to fail on Mac OS X 10.4.11 (2007)
          # (see <https://bugs.gnu.org/78509#95>)
          # and to succeed on Mac OS X 10.5.8 [darwin9.8.0] (2009).
          # Guess it fails on Mac OS X 10.4.x and earlier.
          [darwin[[0-8]].*yes],
             [gl_cv_header_working_fcntl_h="guessing no (bad O_DIRECTORY)"],
          # Known to be "no" on native MS-Windows.
          [mingw* | windows*],
             [gl_cv_header_working_fcntl_h=no],
          [gl_cv_header_working_fcntl_h=$gl_cross_guess_normal])])])

  AS_CASE([$gl_cv_header_working_fcntl_h],
    [*O_DIRECTORY* | *no], [gl_val=0], [gl_val=1])
  AC_DEFINE_UNQUOTED([HAVE_WORKING_O_DIRECTORY], [$gl_val],
    [Define to 1 if O_DIRECTORY works, 0 otherwise.])

  AS_CASE([$gl_cv_header_working_fcntl_h],
    [*O_NOATIME* | *no], [gl_val=0], [gl_val=1])
  AC_DEFINE_UNQUOTED([HAVE_WORKING_O_NOATIME], [$gl_val],
    [Define to 1 if O_NOATIME works, 0 otherwise.])

  AS_CASE([$gl_cv_header_working_fcntl_h],
    [*O_NOFOLLOW* | *no], [gl_val=0], [gl_val=1])
  AC_DEFINE_UNQUOTED([HAVE_WORKING_O_NOFOLLOW], [$gl_val],
    [Define to 1 if O_NOFOLLOW works, 0 otherwise.])
])

# Define HAVE_WORKING_FSTAT_O_PATH to 1 if fstat works on O_PATH descriptors.
AC_DEFUN([gl_WORKING_FSTAT_O_PATH],
[
  AC_REQUIRE([AC_USE_SYSTEM_EXTENSIONS])

  dnl On Linux 2.6.39 through 3.5, fstat fails on O_PATH descriptors.
  dnl On these platforms fstatat with AT_EMPTY_PATH and "" similarly
  dnl fails.  Although it can be dicey to use static checks due to
  dnl the dubious practice of building on newer kernels for older ones,
  dnl do it here anyway as the buggy kernels are rare (all EOLed by 2016)
  dnl and builders for them are unlikely to use the dubious practice.
  dnl Circa 2030 we should remove the old-kernel workarounds entirely.
  AC_CACHE_CHECK([whether fstat works on O_PATH file descriptors],
    [gl_cv_working_fstat_o_path],
    [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM(
          [[#include <fcntl.h>
            #include <sys/stat.h>
            #include <unistd.h>
            #include <errno.h>]],
          [[int fd = open ("confdefs.h", O_PATH);
            struct stat st;
            char c;
            /* A kernel that ignores O_PATH opens this file for reading.  */
            return (fd < 0 || fstat (fd, &st) < 0
                    || read (fd, &c, 1) != -1 || errno != EBADF);]])],
       [gl_cv_working_fstat_o_path=yes],
       [gl_cv_working_fstat_o_path=no],
       [AC_EGREP_CPP([working_fstat_o_path], [
          #include <fcntl.h>
          #if defined __linux__ || defined __ANDROID__
          # include <linux/version.h>
          # define FSTAT_OK (KERNEL_VERSION (3, 6, 0) <= LINUX_VERSION_CODE)
          #else
          # define FSTAT_OK 1
          #endif
          #if defined O_PATH && FSTAT_OK
          working_fstat_o_path
          #endif
          ], [gl_cv_working_fstat_o_path="guessing yes"],
             [gl_cv_working_fstat_o_path="guessing no"])])])
  AS_CASE([$gl_cv_working_fstat_o_path], [*yes], [gl_val=1], [gl_val=0])
  AC_DEFINE_UNQUOTED([HAVE_WORKING_FSTAT_O_PATH], [$gl_val],
    [Define to 1 if fstat works on O_PATH file descriptors, 0 otherwise.])
])
