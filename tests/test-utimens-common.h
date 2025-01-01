/* Test of file timestamp modification functions.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* This file defines some prerequisites useful to utime-related tests.  */

#ifndef GL_TEST_UTIMENS_COMMON
# define GL_TEST_UTIMENS_COMMON

# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <sys/stat.h>
# include <unistd.h>

/* Gnulib modules.  */
# include "stat-time.h"
# include "timespec.h"
# include "utimecmp.h"

/* Gnulib test header.  */
# include "nap.h"

enum {
  BILLION = 1000 * 1000 * 1000,

  Y2K = 946684800, /* Jan 1, 2000, in seconds since epoch.  */

  /* Bogus positive and negative tv_nsec values closest to valid
     range, but without colliding with UTIME_NOW or UTIME_OMIT.  */
  UTIME_BOGUS_POS = BILLION + ((UTIME_NOW == BILLION || UTIME_OMIT == BILLION)
                               ? (1 + (UTIME_NOW == BILLION + 1)
                                  + (UTIME_OMIT == BILLION + 1))
                               : 0),
  UTIME_BOGUS_NEG = -1 - ((UTIME_NOW == -1 || UTIME_OMIT == -1)
                          ? (1 + (UTIME_NOW == -2) + (UTIME_OMIT == -2))
                          : 0)
};

# if defined _WIN32 && !defined __CYGWIN__
/* Skip ctime tests on native Windows, since it is either a copy of
   mtime or birth time (depending on the file system), rather than a
   properly tracked change time.  See
   <https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/stat-functions>.  */
#  define check_ctime 0
# elif (defined __APPLE__ && defined __MACH__) || defined __NetBSD__
/* On macOS, the ctime is not updated when only the st_atime changes.  */
#  define check_ctime -1
# else
#  define check_ctime 1
# endif

/* Compare two st_ctime values.  Return -1, 0 or 1, respectively
   when A's st_ctime is smaller than, equal to or greater than B's.  */
static int
ctime_compare (struct stat const *a, struct stat const *b)
{
  if (a->st_ctime < b->st_ctime)
    return -1;
  else if (b->st_ctime < a->st_ctime)
    return 1;
  else if (get_stat_ctime_ns (a) < get_stat_ctime_ns (b))
    return -1;
  else if (get_stat_ctime_ns (b) < get_stat_ctime_ns (a))
    return 1;
  else
    return 0;
}

/* Test whether FD's file access times are updated by the file system.
   FD must be readable.  Set *ST to the file's status, after any
   change to its access time due to the test.  */
static bool
checkable_atime (int fd, struct stat *st)
{
  char buf[1];
  struct stat st1, st2;

  ASSERT (fstat (fd, &st1) == 0);
  nap ();
  ASSERT (read (fd, buf, sizeof buf) == 0);
  ASSERT (fstat (fd, &st2) == 0);
  bool check_atime
#if defined __HAIKU__
  /* On Haiku, the st_atime field is always the current time.  It is as if there
     was a daemon running (as root) that constantly reads from all files on all
     disks at the same time.  See <https://dev.haiku-os.org/ticket/19038>.  */
    = false;
#else
    = (st1.st_atime != st2.st_atime
       || get_stat_atime_ns (&st1) != get_stat_atime_ns (&st2));
#endif
  if (st)
    *st = st2;
  return check_atime;
}

#endif /* GL_TEST_UTIMENS_COMMON */
