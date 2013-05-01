/* Assist in file system timestamp tests.
   Copyright (C) 2009-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Eric Blake <ebb9@byu.net>, 2009.  */

#ifndef GLTEST_NAP_H
# define GLTEST_NAP_H

# include <limits.h>

/* Return A - B, in ns.
   Return 0 if the true result would be negative.
   Return INT_MAX if the true result would be greater than INT_MAX.  */
static int
diff_timespec (struct timespec a, struct timespec b)
{
  time_t as = a.tv_sec;
  time_t bs = b.tv_sec;
  int ans = a.tv_nsec;
  int bns = b.tv_nsec;

  if (! (bs < as || (bs == as && bns < ans)))
    return 0;
  if (as - bs <= INT_MAX / 1000000000)
    {
      int sdiff = (as - bs) * 1000000000;
      int usdiff = ans - bns;
      if (usdiff < INT_MAX - sdiff)
        return sdiff + usdiff;
    }
  return INT_MAX;
}

static void
get_stat (int fd, struct stat *st, int do_write)
{
  if (do_write)
    ASSERT (write (fd, "\n", 1) == 1);
  ASSERT (fstat (fd, st) == 0);
}

/* Given a file whose descriptor is FD, see whether delaying by DELAY
   nanoseconds causes a change in a file's time stamp.  *ST is the
   file's status, recently gotten.  Update *ST to reflect the latest
   status gotten.  If successful, return the needed delay, in
   nanoseconds as determined by the observed time stamps; this may be
   greater than DELAY if we crossed a quantization boundary.  If
   unsuccessful, return 0.  */
static int
nap_works (int fd, int delay, struct stat *st)
{
  struct stat old_st = *st;
  struct timespec delay_spec;
  int cdiff, mdiff;
  delay_spec.tv_sec = delay / 1000000000;
  delay_spec.tv_nsec = delay % 1000000000;
  ASSERT (nanosleep (&delay_spec, 0) == 0);
  get_stat (fd, st, 1);

  /* Return the greater of the ctime and the mtime differences, or
     zero if it cannot be determined, or INT_MAX if either overflows.  */
  cdiff = diff_timespec (get_stat_ctime (st), get_stat_ctime (&old_st));
  if (cdiff != 0)
    {
      mdiff = diff_timespec (get_stat_mtime (st), get_stat_mtime (&old_st));
      if (mdiff != 0)
        return cdiff < mdiff ? mdiff : cdiff;
    }
  return 0;
}

static int
guess_delay (void)
{
  /* Try a 1-ns sleep first, for speed.  If that doesn't work, try 100
     ns, 1 microsecond, 1 ms, etc.  xfs has a quantization of about 10
     milliseconds, even though it has a granularity of 1 nanosecond,
     and NTFS has a default quantization of 15.25 milliseconds, even
     though it has a granularity of 100 nanoseconds, so 15.25 ms is a
     good quantization to try.  If that doesn't work, try 1 second.
     The worst case is 2 seconds, needed for FAT.  */
  static int const delaytab[] = {1, 1000, 1000000, 15250000, 1000000000 };
  int fd = creat (BASE "tmp", 0600);
  int i;
  int delay = 2000000000;
  struct stat st;
  ASSERT (0 <= fd);
  get_stat (fd, &st, 0);
  for (i = 0; i < sizeof delaytab / sizeof delaytab[0]; i++)
    {
      int d = nap_works (fd, delaytab[i], &st);
      if (d != 0)
        {
          delay = d;
          break;
        }
    }
  ASSERT (close (fd) == 0);
  ASSERT (unlink (BASE "tmp") == 0);
  return delay;
}

/* Sleep long enough to notice a timestamp difference on the file
   system in the current directory.  Assumes that BASE is defined,
   and requires that the test module depends on nanosleep.  */
static void
nap (void)
{
  static struct timespec delay;
  if (!delay.tv_sec && !delay.tv_nsec)
    {
      int d = guess_delay ();

      /* Multiply by 1.125 (rounding up), to avoid problems if the
         file system's clock is a bit slower than nanosleep's.
         Ceiling it at INT_MAX, though.  */
      int delta = (d >> 3) + ((d & 7) != 0);
      d = delta < INT_MAX - d ? d + delta : INT_MAX;
      delay.tv_sec = d / 1000000000;
      delay.tv_nsec = d % 1000000000;
    }
  ASSERT (nanosleep (&delay, 0) == 0);
}

#endif /* GLTEST_NAP_H */
