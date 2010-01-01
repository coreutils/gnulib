/* Test of rename() function.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

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

/* This file is designed to test both rename(a,b) and
   renameat(AT_FDCWD,a,AT_FDCWD,b).  FUNC is the function to test.
   Assumes that BASE and ASSERT are already defined, and that
   appropriate headers are already included.  If PRINT, warn before
   skipping symlink tests with status 77.  */

static int
test_rename (int (*func) (char const *, char const *), bool print)
{
  /* Setup.  */
  struct stat st;
  int fd = creat (BASE "file", 0600);
  ASSERT (0 <= fd);
  ASSERT (write (fd, "hi", 2) == 2);
  ASSERT (close (fd) == 0);
  ASSERT (mkdir (BASE "dir", 0700) == 0);

  /* Obvious errors.  */

  errno = 0; /* Missing source.  */
  ASSERT (func (BASE "missing", BASE "missing") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "missing/", BASE "missing") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "missing", BASE "missing/") == -1);
  ASSERT (errno == ENOENT);
  errno = 0; /* Empty operand.  */
  ASSERT (func ("", BASE "missing") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "file", "") == -1);
  ASSERT (errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "", "") == -1);
  ASSERT (errno == ENOENT);

  /* Files.  */
  errno = 0; /* Trailing slash.  */
  ASSERT (func (BASE "file", BASE "file2/") == -1);
  ASSERT (errno == ENOENT || errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "file/", BASE "file2") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (stat (BASE "file2", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (func (BASE "file", BASE "file2") == 0); /* Simple rename.  */
  errno = 0;
  ASSERT (stat (BASE "file", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "file2", &st) == 0);
  ASSERT (st.st_size == 2);
  ASSERT (close (creat (BASE "file", 0600)) == 0); /* Overwrite.  */
  errno = 0;
  ASSERT (func (BASE "file2", BASE "file/") == -1);
  ASSERT (errno == ENOTDIR);
  ASSERT (func (BASE "file2", BASE "file") == 0);
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "file", &st) == 0);
  ASSERT (st.st_size == 2);
  errno = 0;
  ASSERT (stat (BASE "file2", &st) == -1);
  ASSERT (errno == ENOENT);

  /* Directories.  */
  ASSERT (func (BASE "dir", BASE "dir2/") == 0); /* Simple rename.  */
  errno = 0;
  ASSERT (stat (BASE "dir", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (stat (BASE "dir2", &st) == 0);
  ASSERT (func (BASE "dir2/", BASE "dir") == 0);
  ASSERT (stat (BASE "dir", &st) == 0);
  errno = 0;
  ASSERT (stat (BASE "dir2", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (func (BASE "dir", BASE "dir2") == 0);
  errno = 0;
  ASSERT (stat (BASE "dir", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (stat (BASE "dir2", &st) == 0);
  ASSERT (mkdir (BASE "dir", 0700) == 0); /* Empty onto empty.  */
  ASSERT (func (BASE "dir2", BASE "dir") == 0);
  ASSERT (mkdir (BASE "dir2", 0700) == 0);
  ASSERT (func (BASE "dir2", BASE "dir/") == 0);
  ASSERT (mkdir (BASE "dir2", 0700) == 0);
  ASSERT (func (BASE "dir2/", BASE "dir") == 0);
  ASSERT (mkdir (BASE "dir2", 0700) == 0);
  ASSERT (close (creat (BASE "dir/file", 0600)) == 0); /* Empty onto full.  */
  errno = 0;
  ASSERT (func (BASE "dir2", BASE "dir") == -1);
  ASSERT (errno == EEXIST || errno == ENOTEMPTY);
  errno = 0;
  ASSERT (func (BASE "dir2/", BASE "dir") == -1);
  ASSERT (errno == EEXIST || errno == ENOTEMPTY);
  errno = 0;
  ASSERT (func (BASE "dir2", BASE "dir/") == -1);
  ASSERT (errno == EEXIST || errno == ENOTEMPTY);
  ASSERT (func (BASE "dir", BASE "dir2") == 0); /* Full onto empty.  */
  errno = 0;
  ASSERT (stat (BASE "dir", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (stat (BASE "dir2/file", &st) == 0);
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  ASSERT (func (BASE "dir2/", BASE "dir") == 0);
  ASSERT (stat (BASE "dir/file", &st) == 0);
  errno = 0;
  ASSERT (stat (BASE "dir2", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (mkdir (BASE "dir2", 0700) == 0);
  ASSERT (func (BASE "dir", BASE "dir2/") == 0);
  errno = 0;
  ASSERT (stat (BASE "dir", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (stat (BASE "dir2/file", &st) == 0);
  ASSERT (unlink (BASE "dir2/file") == 0);
  errno = 0; /* Reject trailing dot.  */
  ASSERT (func (BASE "dir2", BASE "dir/.") == -1);
  ASSERT (errno == EINVAL || errno == ENOENT);
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  errno = 0;
  ASSERT (func (BASE "dir2", BASE "dir/.") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY || errno == EISDIR
          || errno == ENOTEMPTY);
  errno = 0;
  ASSERT (func (BASE "dir2/.", BASE "dir") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY);
  ASSERT (rmdir (BASE "dir") == 0);
  errno = 0;
  ASSERT (func (BASE "dir2", BASE "dir/.//") == -1);
  ASSERT (errno == EINVAL || errno == ENOENT);
  ASSERT (mkdir (BASE "dir", 0700) == 0);
  errno = 0;
  ASSERT (func (BASE "dir2", BASE "dir/.//") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY || errno == EISDIR
          || errno == ENOTEMPTY);
  errno = 0;
  ASSERT (func (BASE "dir2/.//", BASE "dir") == -1);
  ASSERT (errno == EINVAL || errno == EBUSY);
  ASSERT (rmdir (BASE "dir2") == 0);
  errno = 0; /* Move into subdir.  */
  ASSERT (func (BASE "dir", BASE "dir/sub") == -1);
  ASSERT (errno == EINVAL || errno == EACCES);
  errno = 0;
  ASSERT (stat (BASE "dir/sub", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (mkdir (BASE "dir/sub", 0700) == 0);
  errno = 0;
  ASSERT (func (BASE "dir", BASE "dir/sub") == -1);
  ASSERT (errno == EINVAL);
  ASSERT (stat (BASE "dir/sub", &st) == 0);
  ASSERT (rmdir (BASE "dir/sub") == 0);

  /* Mixing file and directory.  */
  errno = 0; /* File onto dir.  */
  ASSERT (func (BASE "file", BASE "dir") == -1);
  ASSERT (errno == EISDIR || errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "file", BASE "dir/") == -1);
  ASSERT (errno == EISDIR || errno == ENOTDIR);
  errno = 0; /* Dir onto file.  */
  ASSERT (func (BASE "dir", BASE "file") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "dir/", BASE "file") == -1);
  ASSERT (errno == ENOTDIR);

  /* Hard links.  */
  ASSERT (func (BASE "file", BASE "file") == 0); /* File onto self.  */
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "file", &st) == 0);
  ASSERT (st.st_size == 2);
  ASSERT (func (BASE "dir", BASE "dir") == 0); /* Empty dir onto self.  */
  ASSERT (stat (BASE "dir", &st) == 0);
  ASSERT (close (creat (BASE "dir/file", 0600)) == 0);
  ASSERT (func (BASE "dir", BASE "dir") == 0); /* Full dir onto self.  */
  ASSERT (unlink (BASE "dir/file") == 0);
  {
    /*  Not all file systems support link.  Mingw doesn't have
        reliable st_nlink on hard links, but our implementation does
        fail with EPERM on poor file systems, and we can detect the
        inferior stat() via st_ino.  Cygwin 1.5.x copies rather than
        links files on those file systems, but there, st_nlink and
        st_ino are reliable.  */
    int ret = link (BASE "file", BASE "file2");
    if (!ret)
      {
        memset (&st, 0, sizeof st);
        ASSERT (stat (BASE "file2", &st) == 0);
        if (st.st_ino && st.st_nlink != 2)
          {
            ASSERT (unlink (BASE "file2") == 0);
            errno = EPERM;
            ret = -1;
          }
      }
    if (ret == -1)
      {
        /* If the device does not support hard links, errno is
           EPERM on Linux, EOPNOTSUPP on FreeBSD.  */
        switch (errno)
          {
          case EPERM:
          case EOPNOTSUPP:
            if (print)
              fputs ("skipping test: "
                     "hard links not supported on this file system\n",
                     stderr);
            ASSERT (unlink (BASE "file") == 0);
            ASSERT (rmdir (BASE "dir") == 0);
            return 77;
          default:
            perror ("link");
            return 1;
          }
      }
    ASSERT (ret == 0);
  }
  ASSERT (func (BASE "file", BASE "file2") == 0); /* File onto hard link.  */
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "file", &st) == 0);
  ASSERT (st.st_size == 2);
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "file2", &st) == 0);
  ASSERT (st.st_size == 2);
  ASSERT (unlink (BASE "file2") == 0);

  /* Symlinks.  */
  if (symlink (BASE "file", BASE "link1"))
    {
      if (print)
        fputs ("skipping test: symlinks not supported on this file system\n",
               stderr);
      ASSERT (unlink (BASE "file") == 0);
      ASSERT (rmdir (BASE "dir") == 0);
      return 77;
    }
  ASSERT (func (BASE "link1", BASE "link2") == 0); /* Simple rename.  */
  ASSERT (stat (BASE "file", &st) == 0);
  errno = 0;
  ASSERT (lstat (BASE "link1", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link2", &st) == 0);
  ASSERT (S_ISLNK (st.st_mode));
  ASSERT (symlink (BASE "nowhere", BASE "link1") == 0); /* Overwrite.  */
  ASSERT (func (BASE "link2", BASE "link1") == 0);
  memset (&st, 0, sizeof st);
  ASSERT (stat (BASE "link1", &st) == 0);
  ASSERT (st.st_size == 2);
  errno = 0;
  ASSERT (lstat (BASE "link2", &st) == -1);
  ASSERT (errno == ENOENT);
  ASSERT (symlink (BASE "link2", BASE "link2") == 0); /* Symlink loop.  */
  ASSERT (func (BASE "link2", BASE "link2") == 0);
  errno = 0;
  ASSERT (func (BASE "link2/", BASE "link2") == -1);
  ASSERT (errno == ELOOP || errno == ENOTDIR);
  ASSERT (func (BASE "link2", BASE "link3") == 0);
  ASSERT (unlink (BASE "link3") == 0);
  ASSERT (symlink (BASE "nowhere", BASE "link2") == 0); /* Dangling link.  */
  ASSERT (func (BASE "link2", BASE "link3") == 0);
  errno = 0;
  ASSERT (lstat (BASE "link2", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link3", &st) == 0);
  errno = 0; /* Trailing slash on dangling.  */
  ASSERT (func (BASE "link3/", BASE "link2") == -1);
  ASSERT (errno == ENOENT || errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "link3", BASE "link2/") == -1);
  ASSERT (errno == ENOENT || errno == ENOTDIR);
  errno = 0;
  ASSERT (lstat (BASE "link2", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link3", &st) == 0);
  errno = 0; /* Trailing slash on link to file.  */
  ASSERT (func (BASE "link1/", BASE "link2") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "link1", BASE "link3/") == -1);
  ASSERT (errno == ENOENT || errno == ENOTDIR);

  /* Mixing symlink and file.  */
  ASSERT (close (creat (BASE "file2", 0600)) == 0); /* File onto link.  */
  ASSERT (func (BASE "file2", BASE "link3") == 0);
  errno = 0;
  ASSERT (stat (BASE "file2", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link3", &st) == 0);
  ASSERT (S_ISREG (st.st_mode));
  ASSERT (unlink (BASE "link3") == 0);
  ASSERT (symlink (BASE "nowhere", BASE "link2") == 0); /* Link onto file.  */
  ASSERT (close (creat (BASE "file2", 0600)) == 0);
  ASSERT (func (BASE "link2", BASE "file2") == 0);
  errno = 0;
  ASSERT (lstat (BASE "link2", &st) == -1);
  ASSERT (errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "file2", &st) == 0);
  ASSERT (S_ISLNK (st.st_mode));
  ASSERT (unlink (BASE "file2") == 0);
  errno = 0; /* Trailing slash.  */
  ASSERT (func (BASE "file/", BASE "link1") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "file", BASE "link1/") == -1);
  ASSERT (errno == ENOTDIR || errno == ENOENT);
  errno = 0;
  ASSERT (func (BASE "link1/", BASE "file") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "link1", BASE "file/") == -1);
  ASSERT (errno == ENOTDIR || errno == ENOENT);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "file", &st) == 0);
  ASSERT (S_ISREG (st.st_mode));
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link1", &st) == 0);
  ASSERT (S_ISLNK (st.st_mode));

  /* Mixing symlink and directory.  */
  errno = 0; /* Directory onto link.  */
  ASSERT (func (BASE "dir", BASE "link1") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "dir/", BASE "link1") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "dir", BASE "link1/") == -1);
  ASSERT (errno == ENOTDIR);
  errno = 0; /* Link onto directory.  */
  ASSERT (func (BASE "link1", BASE "dir") == -1);
  ASSERT (errno == EISDIR || errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "link1", BASE "dir/") == -1);
  ASSERT (errno == EISDIR || errno == ENOTDIR);
  errno = 0;
  ASSERT (func (BASE "link1/", BASE "dir") == -1);
  ASSERT (errno == ENOTDIR);
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "link1", &st) == 0);
  ASSERT (S_ISLNK (st.st_mode));
  memset (&st, 0, sizeof st);
  ASSERT (lstat (BASE "dir", &st) == 0);
  ASSERT (S_ISDIR (st.st_mode));

  /* POSIX requires rename("link-to-dir/","other") to rename "dir" and
     leave "link-to-dir" dangling, but GNU rejects this.  POSIX
     requires rename("dir","dangling/") to create the directory so
     that "dangling/" now resolves, but GNU rejects this.  While we
     prefer GNU behavior, we don't enforce it.  However, we do test
     that the system either follows POSIX in both cases, or follows
     GNU.  */
  {
    int result;
    ASSERT (symlink (BASE "dir2", BASE "link2") == 0);
    errno = 0;
    result = func (BASE "dir", BASE "link2/");
    if (result == 0)
      {
        /* POSIX.  */
        errno = 0;
        ASSERT (lstat (BASE "dir", &st) == -1);
        ASSERT (errno == ENOENT);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "dir2", &st) == 0);
        ASSERT (S_ISDIR (st.st_mode));
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "link2", &st) == 0);
        ASSERT (S_ISLNK (st.st_mode));
        ASSERT (func (BASE "link2/", BASE "dir") == 0);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "dir", &st) == 0);
        ASSERT (S_ISDIR (st.st_mode));
        errno = 0;
        ASSERT (lstat (BASE "dir2", &st) == -1);
        ASSERT (errno == ENOENT);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "link2", &st) == 0);
        ASSERT (S_ISLNK (st.st_mode));
      }
    else
      {
        /* GNU.  */
        ASSERT (result == -1);
        ASSERT (errno == ENOTDIR);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "dir", &st) == 0);
        ASSERT (S_ISDIR (st.st_mode));
        errno = 0;
        ASSERT (lstat (BASE "dir2", &st) == -1);
        ASSERT (errno == ENOENT);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "link2", &st) == 0);
        ASSERT (S_ISLNK (st.st_mode));
        ASSERT (unlink (BASE "link2") == 0);
        ASSERT (symlink (BASE "dir", BASE "link2") == 0);
        errno = 0; /* OpenBSD notices that link2/ and dir are the same.  */
        result = func (BASE "link2/", BASE "dir");
        if (result) /* GNU/Linux rejects attempts to use link2/.  */
          {
            ASSERT (result == -1);
            ASSERT (errno == ENOTDIR);
          }
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "dir", &st) == 0);
        ASSERT (S_ISDIR (st.st_mode));
        errno = 0;
        ASSERT (lstat (BASE "dir2", &st) == -1);
        ASSERT (errno == ENOENT);
        memset (&st, 0, sizeof st);
        ASSERT (lstat (BASE "link2", &st) == 0);
        ASSERT (S_ISLNK (st.st_mode));
      }
  }

  /* Clean up.  */
  ASSERT (unlink (BASE "file") == 0);
  ASSERT (rmdir (BASE "dir") == 0);
  ASSERT (unlink (BASE "link1") == 0);
  ASSERT (unlink (BASE "link2") == 0);

  return 0;
}
