/* Tests of stat.
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

/* This file is designed to test both stat(n,buf) and
   fstatat(AT_FDCWD,n,buf,0).  FUNC is the function to test.  Assumes
   that BASE and ASSERT are already defined, and that appropriate
   headers are already included.  */

static int
test_stat_func (int (*func) (char const *, struct stat *))
{
  struct stat st1;
  struct stat st2;
  char cwd[PATH_MAX];

  ASSERT (getcwd (cwd, PATH_MAX) == cwd);
  ASSERT (func (".", &st1) == 0);
  ASSERT (func ("./", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (func (cwd, &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));
  ASSERT (func ("/", &st1) == 0);
  ASSERT (func ("///", &st2) == 0);
  ASSERT (SAME_INODE (st1, st2));

  errno = 0;
  ASSERT (func ("", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func ("nosuch", &st1) == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func ("nosuch/", &st1) == -1);
  ASSERT (errno == ENOENT);

  ASSERT (close (creat (BASE "file", 0600)) == 0);
  ASSERT (func (BASE "file", &st1) == 0);
  errno = 0;
  ASSERT (func (BASE "file/", &st1) == -1);
  ASSERT (errno == ENOTDIR);
  ASSERT (unlink (BASE "file") == 0);

  return 0;
}
