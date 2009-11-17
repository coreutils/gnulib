/* Tests of lchown.
   Copyright (C) 2009 Free Software Foundation, Inc.

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

#ifndef TEST_CHOWN_NAP
/* Sleep long enough to notice a timestamp difference on the file
   system in the current directory.  */
static void
nap (void)
{
  static long delay;
  if (!delay)
    {
      /* Initialize only once, by sleeping for 20 milliseconds (needed
         since xfs has a quantization of about 10 milliseconds, even
         though it has a granularity of 1 nanosecond, and since NTFS
         has a default quantization of 15.25 milliseconds, even though
         it has a granularity of 100 nanoseconds).  If the seconds
         differ, repeat the test one more time (in case we crossed a
         quantization boundary on a file system with 1 second
         resolution).  If we can't observe a difference in only the
         nanoseconds, then fall back to 1 second if the time is odd,
         and 2 seconds (needed for FAT) if time is even.  */
      struct stat st1;
      struct stat st2;
      ASSERT (close (creat (BASE "tmp", 0600)) == 0);
      ASSERT (stat (BASE "tmp", &st1) == 0);
      ASSERT (unlink (BASE "tmp") == 0);
      delay = 20000;
      usleep (delay);
      ASSERT (close (creat (BASE "tmp", 0600)) == 0);
      ASSERT (stat (BASE "tmp", &st2) == 0);
      ASSERT (unlink (BASE "tmp") == 0);
      if (st1.st_mtime != st2.st_mtime)
        {
          /* Seconds differ, give it one more shot.  */
          st1 = st2;
          usleep (delay);
          ASSERT (close (creat (BASE "tmp", 0600)) == 0);
          ASSERT (stat (BASE "tmp", &st2) == 0);
          ASSERT (unlink (BASE "tmp") == 0);
        }
      if (! (st1.st_mtime == st2.st_mtime
             && get_stat_mtime_ns (&st1) < get_stat_mtime_ns (&st2)))
        delay = (st1.st_mtime & 1) ? 1000000 : 2000000;
    }
  usleep (delay);
}
#endif /* !TEST_CHOWN_NAP */

#if !HAVE_GETEGID
# define getegid() (-1)
#endif

#ifndef HAVE_LCHMOD
# define HAVE_LCHMOD 0
#endif

#ifndef CHOWN_CHANGE_TIME_BUG
# define CHOWN_CHANGE_TIME_BUG 0
#endif

/* This file is designed to test lchown(n,o,g) and
   chownat(AT_FDCWD,n,o,g,AT_SYMLINK_NOFOLLOW).  FUNC is the function
   to test.  Assumes that BASE and ASSERT are already defined, and
   that appropriate headers are already included.  If PRINT, warn
   before skipping symlink tests with status 77.  */

static int
test_lchown (int (*func) (char const *, uid_t, gid_t), bool print)
{
  struct stat st1;
  struct stat st2;
  gid_t *gids = NULL;
  int gids_count;
  int result;

  /* Solaris 8 is interesting - if the current process belongs to
     multiple groups, the current directory is owned by a a group that
     the current process belongs to but different than getegid(), and
     the current directory does not have the S_ISGID bit, then regular
     files created in the directory belong to the directory's group,
     but symlinks belong to the current effective group id.  If
     S_ISGID is set, then both files and symlinks belong to the
     directory's group.  However, it is possible to run the testsuite
     from within a directory owned by a group we don't belong to, in
     which case all things that we create belong to the current
     effective gid.  So, work around the issues by creating a
     subdirectory (we are guaranteed that the subdirectory will be
     owned by one of our current groups), change ownership of that
     directory to the current effective gid (which will thus succeed),
     then create all other files within that directory (eliminating
     questions on whether inheritance or current id triumphs, since
     the two methods resolve to the same gid).  */
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  ASSERT (stat (BASE "dir", &st1) == 0);

  /* Filter out mingw, which has no concept of groups.  */
  result = func (BASE "dir", st1.st_uid, getegid ());
  if (result == -1 && errno == ENOSYS)
    {
      ASSERT (rmdir (BASE "dir") == 0);
      if (print)
        fputs ("skipping test: no support for ownership\n", stderr);
      return 77;
    }
  ASSERT (result == 0);

  ASSERT (close (creat (BASE "dir/file", 0600)) == 0);
  ASSERT (stat (BASE "dir/file", &st1) == 0);
  ASSERT (st1.st_uid != -1);
  ASSERT (st1.st_gid != -1);
  ASSERT (st1.st_gid == getegid ());

  /* Sanity check of error cases.  */
  errno = 0;
  ASSERT (func ("", -1, -1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func ("no_such", -1, -1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func ("no_such/", -1, -1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "dir/file/", -1, -1) == -1);
  ASSERT (errno == ENOTDIR);

  /* Check that -1 does not alter ownership.  */
  ASSERT (func (BASE "dir/file", -1, st1.st_gid) == 0);
  ASSERT (func (BASE "dir/file", st1.st_uid, -1) == 0);
  ASSERT (stat (BASE "dir/file", &st2) == 0);
  ASSERT (st1.st_uid == st2.st_uid);
  ASSERT (st1.st_gid == st2.st_gid);

  /* Even if the values aren't changing, ctime is required to change
     if at least one argument is not -1.  */
  nap ();
  ASSERT (func (BASE "dir/file", st1.st_uid, st1.st_gid) == 0);
  ASSERT (stat (BASE "dir/file", &st2) == 0);
  ASSERT (st1.st_ctime < st2.st_ctime
          || (st1.st_ctime == st2.st_ctime
              && get_stat_ctime_ns (&st1) < get_stat_ctime_ns (&st2)));

  /* Test symlink behavior.  */
  if (symlink ("link", BASE "dir/link2"))
    {
      ASSERT (unlink (BASE "dir/file") == 0);
      ASSERT (rmdir (BASE "dir") == 0);
      if (print)
        fputs ("skipping test: symlinks not supported on this file system\n",
               stderr);
      return 77;
    }
  result = func (BASE "dir/link2", -1, -1);
  if (result == -1 && errno == ENOSYS)
    {
      ASSERT (unlink (BASE "dir/file") == 0);
      ASSERT (unlink (BASE "dir/link2") == 0);
      ASSERT (rmdir (BASE "dir") == 0);
      if (print)
        fputs ("skipping test: symlink ownership not supported\n", stderr);
      return 77;
    }
  ASSERT (result == 0);
  errno = 0;
  ASSERT (func (BASE "dir/link2/", st1.st_uid, st1.st_gid) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (symlink ("file", BASE "dir/link") == 0);
  ASSERT (mkdir (BASE "dir/sub", 0700) == 0);
  ASSERT (symlink ("sub", BASE "dir/link3") == 0);

  /* For non-privileged users, lchown can only portably succeed at
     changing group ownership of a file we own.  If we belong to at
     least two groups, then verifying the correct change is simple.
     But if we belong to only one group, then we fall back on the
     other observable effect of lchown: the ctime must be updated.
     Be careful of duplicates returned by getgroups.  */
  gids_count = mgetgroups (NULL, -1, &gids);
  if (2 <= gids_count && gids[0] == gids[1] && 2 < gids_count--)
    gids[1] = gids[2];
  if (1 < gids_count || (gids_count == 1 && gids[0] != st1.st_gid))
    {
      if (gids[0] == st1.st_gid)
        {
          ASSERT (1 < gids_count);
          ASSERT (gids[0] != gids[1]);
          gids[0] = gids[1];
        }
      ASSERT (gids[0] != st1.st_gid);
      ASSERT (gids[0] != -1);
      ASSERT (lstat (BASE "dir/link", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/link2", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);

      errno = 0;
      ASSERT (func (BASE "dir/link2/", -1, gids[0]) == -1);
      ASSERT (errno == ENOTDIR);
      ASSERT (stat (BASE "dir/file", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/link", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/link2", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);

      ASSERT (func (BASE "dir/link2", -1, gids[0]) == 0);
      ASSERT (stat (BASE "dir/file", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/link", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/link2", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (gids[0] == st2.st_gid);

      /* Trailing slash follows through to directory.  */
      ASSERT (lstat (BASE "dir/link3", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/sub", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);

      ASSERT (func (BASE "dir/link3/", -1, gids[0]) == 0);
      ASSERT (lstat (BASE "dir/link3", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (st1.st_gid == st2.st_gid);
      ASSERT (lstat (BASE "dir/sub", &st2) == 0);
      ASSERT (st1.st_uid == st2.st_uid);
      ASSERT (gids[0] == st2.st_gid);
    }
  else if (!CHOWN_CHANGE_TIME_BUG || HAVE_LCHMOD)
    {
      /* If we don't have lchmod, and lchown fails to change ctime,
         then we can't test this part of lchown.  */
      struct stat l1;
      struct stat l2;
      ASSERT (stat (BASE "dir/file", &st1) == 0);
      ASSERT (lstat (BASE "dir/link", &l1) == 0);
      ASSERT (lstat (BASE "dir/link2", &l2) == 0);

      nap ();
      errno = 0;
      ASSERT (func (BASE "dir/link2/", -1, st1.st_gid) == -1);
      ASSERT (errno == ENOTDIR);
      ASSERT (stat (BASE "dir/file", &st2) == 0);
      ASSERT (st1.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&st1) == get_stat_ctime_ns (&st2));
      ASSERT (lstat (BASE "dir/link", &st2) == 0);
      ASSERT (l1.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&l1) == get_stat_ctime_ns (&st2));
      ASSERT (lstat (BASE "dir/link2", &st2) == 0);
      ASSERT (l2.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&l2) == get_stat_ctime_ns (&st2));

      ASSERT (func (BASE "dir/link2", -1, gids[0]) == 0);
      ASSERT (stat (BASE "dir/file", &st2) == 0);
      ASSERT (st1.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&st1) == get_stat_ctime_ns (&st2));
      ASSERT (lstat (BASE "dir/link", &st2) == 0);
      ASSERT (l1.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&l1) == get_stat_ctime_ns (&st2));
      ASSERT (lstat (BASE "dir/link2", &st2) == 0);
      ASSERT (l2.st_ctime < st2.st_ctime
              || (l2.st_ctime == st2.st_ctime
                  && get_stat_ctime_ns (&l2) < get_stat_ctime_ns (&st2)));

      /* Trailing slash follows through to directory.  */
      ASSERT (lstat (BASE "dir/sub", &st1) == 0);
      ASSERT (lstat (BASE "dir/link3", &l1) == 0);
      nap ();
      ASSERT (func (BASE "dir/link3/", -1, gids[0]) == 0);
      ASSERT (lstat (BASE "dir/link3", &st2) == 0);
      ASSERT (l1.st_ctime == st2.st_ctime);
      ASSERT (get_stat_ctime_ns (&l1) == get_stat_ctime_ns (&st2));
      ASSERT (lstat (BASE "dir/sub", &st2) == 0);
      ASSERT (st1.st_ctime < st2.st_ctime
              || (st1.st_ctime == st2.st_ctime
                  && get_stat_ctime_ns (&st1) < get_stat_ctime_ns (&st2)));
    }

  /* Cleanup.  */
  free (gids);
  ASSERT (unlink (BASE "dir/file") == 0);
  ASSERT (unlink (BASE "dir/link") == 0);
  ASSERT (unlink (BASE "dir/link2") == 0);
  ASSERT (unlink (BASE "dir/link3") == 0);
  ASSERT (rmdir (BASE "dir/sub") == 0);
  ASSERT (rmdir (BASE "dir") == 0);
  return 0;
}
