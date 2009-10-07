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

/* This function is designed to test both utimens(a,b) and
   utimensat(AT_FDCWD,a,b,0).  FUNC is the function to test.  */
static int
test_utimens (int (*func) (char const *, struct timespec const *))
{
  struct stat st1;
  struct stat st2;

  ASSERT (close (creat (BASE "file", 0600)) == 0);
  /* If utimens truncates to less resolution than the file system
     supports, then time can appear to go backwards between now and
     the follow-up utimens(file,NULL).  Use UTIMECMP_TRUNCATE_SOURCE
     to compensate, with st1 as the source.  */
  ASSERT (stat (BASE "file", &st1) == 0);

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
    ASSERT (func (BASE "file", ts) == -1);
    ASSERT (errno == EINVAL);
  }
  {
    struct timespec ts[2] = { { Y2K, 0 }, { Y2K, UTIME_BOGUS_NEG } };
    errno = 0;
    ASSERT (func (BASE "file", ts) == -1);
    ASSERT (errno == EINVAL);
  }
  ASSERT (stat (BASE "file", &st2) == 0);
  ASSERT (st1.st_atime == st2.st_atime);
  ASSERT (get_stat_atime_ns (&st1) == get_stat_atime_ns (&st2));
  ASSERT (utimecmp (BASE "file", &st1, &st2, 0) == 0);

  /* Set both times.  */
  {
    struct timespec ts[2] = { { Y2K, BILLION / 2 - 1 }, { Y2K, BILLION - 1 } };
    ASSERT (func (BASE "file", ts) == 0);
    ASSERT (stat (BASE "file", &st2) == 0);
    ASSERT (st2.st_atime == Y2K);
    ASSERT (0 <= get_stat_atime_ns (&st2));
    ASSERT (get_stat_atime_ns (&st2) < BILLION / 2);
    ASSERT (st2.st_mtime == Y2K);
    ASSERT (0 <= get_stat_mtime_ns (&st2));
    ASSERT (get_stat_mtime_ns (&st2) < BILLION);
  }

  /* Play with UTIME_OMIT, UTIME_NOW.  */
  {
    struct timespec ts[2] = { { BILLION, UTIME_OMIT }, { 0, UTIME_NOW } };
    ASSERT (func (BASE "file", ts) == 0);
    ASSERT (stat (BASE "file", &st2) == 0);
    ASSERT (st2.st_atime == Y2K);
    ASSERT (0 <= get_stat_atime_ns (&st2));
    ASSERT (get_stat_atime_ns (&st2) < BILLION / 2);
    /* See comment above about this utimecmp call.  */
    ASSERT (0 <= utimecmp (BASE "file", &st2, &st1, UTIMECMP_TRUNCATE_SOURCE));
  }

  /* Cleanup.  */
  ASSERT (unlink (BASE "file") == 0);
  return 0;
}
