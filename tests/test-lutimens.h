/* Test of file timestamp modification functions.
   Copyright (C) 2009 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This file assumes that BASE and ASSERT are already defined.  */

#ifndef GL_TEST_UTIMENS
# define GL_TEST_UTIMENS

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "stat-time.h"
#include "timespec.h"
#include "utimecmp.h"

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

#endif /* GL_TEST_UTIMENS */

/* This function is designed to test both lutimens(a,b) and
   utimensat(AT_FDCWD,a,b,AT_SYMLINK_NOFOLLOW).  FUNC is the function
   to test.  If PRINT, warn before skipping tests with status 77.  */
static int
test_lutimens (int (*func) (char const *, struct timespec const *), bool print)
{
  int result;
  struct stat st1;
  struct stat st2;
  bool atime_supported = true;

  if (symlink ("nowhere", BASE "link"))
    {
      if (print)
        fputs ("skipping test: symlinks not supported on this file system\n",
               stderr);
      return 77;
    }
  errno = 0;
  result = func (BASE "link", NULL);
  if (result == -1 && errno == ENOSYS)
    {
      ASSERT (unlink (BASE "link") == 0);
      if (print)
        fputs ("skipping test: "
               "setting symlink time not supported on this file system\n",
               stderr);
      return 77;
    }
  ASSERT (!result);
  ASSERT (lstat (BASE "link", &st1) == 0);
#if HAVE_USLEEP
  /* On Cygwin, the mere act of lstat changes symlink atime, even
     though POSIX says that only readlink is allowed to do that.
     Sleeping for one millisecond is enough to expose this.  Platforms
     without usleep either don't have symlinks, or are immune.  */
  usleep (1000);
#endif
  ASSERT (lstat (BASE "link", &st2) == 0);
  if (st1.st_atime != st2.st_atime
      || get_stat_atime_ns (&st1) != get_stat_atime_ns (&st2))
     atime_supported = false;

  /* Invalid arguments.  */
  errno = 0;
  ASSERT (func ("no_such", NULL) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func ("", NULL) == -1);
  ASSERT (errno == ENOENT);
  {
    struct timespec ts[2] = { { Y2K, UTIME_BOGUS_POS }, { Y2K, 0 } };
    errno = 0;
    ASSERT (func (BASE "link", ts) == -1);
    ASSERT (errno == EINVAL);
  }
  {
    struct timespec ts[2] = { { Y2K, 0 }, { Y2K, UTIME_BOGUS_NEG } };
    errno = 0;
    ASSERT (func (BASE "link", ts) == -1);
    ASSERT (errno == EINVAL);
  }
  ASSERT (lstat (BASE "link", &st2) == 0);
  if (atime_supported)
    {
      ASSERT (st1.st_atime == st2.st_atime);
      ASSERT (get_stat_atime_ns (&st1) == get_stat_atime_ns (&st2));
    }
  ASSERT (utimecmp (BASE "link", &st1, &st2, 0) == 0);

  /* Set both times.  */
  {
    struct timespec ts[2] = { { Y2K, BILLION / 2 - 1 }, { Y2K, BILLION - 1 } };
    ASSERT (func (BASE "link", ts) == 0);
    ASSERT (lstat (BASE "link", &st2) == 0);
    if (atime_supported)
      {
        ASSERT (st2.st_atime == Y2K);
        ASSERT (0 <= get_stat_atime_ns (&st2));
        ASSERT (get_stat_atime_ns (&st2) < BILLION / 2);
      }
    ASSERT (st2.st_mtime == Y2K);
    ASSERT (0 <= get_stat_mtime_ns (&st2));
    ASSERT (get_stat_mtime_ns (&st2) < BILLION);
  }

  /* Play with UTIME_OMIT, UTIME_NOW.  */
  {
    struct timespec ts[2] = { { BILLION, UTIME_OMIT }, { 0, UTIME_NOW } };
    ASSERT (func (BASE "link", ts) == 0);
    ASSERT (lstat (BASE "link", &st2) == 0);
    if (atime_supported)
      {
        ASSERT (st2.st_atime == Y2K);
        ASSERT (0 <= get_stat_atime_ns (&st2));
        ASSERT (get_stat_atime_ns (&st2) < BILLION / 2);
      }
    ASSERT (utimecmp (BASE "link", &st1, &st2, 0) <= 0);
  }

  /* Cleanup.  */
  ASSERT (unlink (BASE "link") == 0);
  return 0;
}
