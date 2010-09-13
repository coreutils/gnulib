/* provide a replacement fdopendir function
   Copyright (C) 2004-2010 Free Software Foundation, Inc.

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

/* written by Jim Meyering */

#include <config.h>

#include <dirent.h>

#include <stdlib.h>
#include <unistd.h>

#if !HAVE_FDOPENDIR

# include "openat.h"
# include "openat-priv.h"
# include "save-cwd.h"

# if GNULIB_DIRENT_SAFER
#  include "dirent--.h"
# endif

static DIR *fdopendir_with_dup (int, int);
static DIR *fd_clone_opendir (int);

/* Replacement for POSIX fdopendir.

   First, try to simulate it via opendir ("/proc/self/fd/FD").  Failing
   that, simulate it by using fchdir metadata, or by doing
   save_cwd/fchdir/opendir(".")/restore_cwd.
   If either the save_cwd or the restore_cwd fails (relatively unlikely),
   then give a diagnostic and exit nonzero.

   If successful, the resulting stream is based on FD in
   implementations where streams are based on file descriptors and in
   applications where no other thread or signal handler allocates or
   frees file descriptors.  In other cases, consult dirfd on the result
   to find out whether FD is still being used.

   Otherwise, this function works just like POSIX fdopendir.

   W A R N I N G:

   Unlike other fd-related functions, this one places constraints on FD.
   If this function returns successfully, FD is under control of the
   dirent.h system, and the caller should not close or modify the state of
   FD other than by the dirent.h functions.  */
DIR *
fdopendir (int fd)
{
  return fdopendir_with_dup (fd, -1);
}

/* Like fdopendir, except that if OLDER_DUPFD is not -1, it is known
   to be a dup of FD which is less than FD - 1 and which will be
   closed by the caller and not otherwise used by the caller.  This
   function makes sure that FD is closed and all file descriptors less
   than FD are open, and then calls fd_clone_opendir on a dup of FD.
   That way, barring race conditions, fd_clone_opendir returns a
   stream whose file descriptor is FD.  */
static DIR *
fdopendir_with_dup (int fd, int older_dupfd)
{
  int dupfd = dup (fd);
  if (dupfd < 0 && errno == EMFILE)
    dupfd = older_dupfd;
  if (dupfd < 0)
    return NULL;
  else
    {
      DIR *dir;
      int saved_errno;
      if (dupfd < fd - 1 && dupfd != older_dupfd)
        {
          dir = fdopendir_with_dup (fd, dupfd);
          saved_errno = errno;
        }
      else
        {
          close (fd);
          dir = fd_clone_opendir (dupfd);
          saved_errno = errno;
          if (! dir)
            {
              int fd1 = dup (dupfd);
              if (fd1 != fd)
                openat_save_fail (fd1 < 0 ? errno : EBADF);
            }
        }

      if (dupfd != older_dupfd)
        close (dupfd);
      errno = saved_errno;
      return dir;
    }
}

/* Like fdopendir, except the result controls a clone of FD.  It is
   the caller's responsibility both to close FD and (if the result is
   not null) to closedir the result.  */
static DIR *
fd_clone_opendir (int fd)
{
  int saved_errno;
  DIR *dir;

  char buf[OPENAT_BUFFER_SIZE];
  char *proc_file = openat_proc_name (buf, fd, ".");
  if (proc_file)
    {
      dir = opendir (proc_file);
      saved_errno = errno;
    }
  else
    {
      dir = NULL;
      saved_errno = EOPNOTSUPP;
    }

  /* If the syscall fails with an expected errno value, resort to
     save_cwd/restore_cwd.  */
  if (! dir && EXPECTED_ERRNO (saved_errno))
    {
# if REPLACE_FCHDIR
      const char *name = _gl_directory_name (fd);
      if (name)
        dir = opendir (name);
      saved_errno = errno;
# else /* !REPLACE_FCHDIR */
      struct saved_cwd saved_cwd;
      if (save_cwd (&saved_cwd) != 0)
        openat_save_fail (errno);

      if (fchdir (fd) != 0)
        {
          dir = NULL;
          saved_errno = errno;
        }
      else
        {
          dir = opendir (".");
          saved_errno = errno;

          if (restore_cwd (&saved_cwd) != 0)
            openat_restore_fail (errno);
        }

      free_cwd (&saved_cwd);
# endif /* !REPLACE_FCHDIR */
    }

  if (proc_file != buf)
    free (proc_file);
  errno = saved_errno;
  return dir;
}

#else /* HAVE_FDOPENDIR */

# include <errno.h>
# include <sys/stat.h>

# undef fdopendir

/* Like fdopendir, but work around GNU/Hurd bug by validating FD.  */

DIR *
rpl_fdopendir (int fd)
{
  struct stat st;
  if (fstat (fd, &st))
    return NULL;
  if (!S_ISDIR (st.st_mode))
    {
      errno = ENOTDIR;
      return NULL;
    }
  return fdopendir (fd);
}

#endif /* HAVE_FDOPENDIR */
