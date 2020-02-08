/* Change the protections of file relative to an open directory.
   Copyright (C) 2006, 2009-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

/* If the user's config.h happens to include <sys/stat.h>, let it include only
   the system's <sys/stat.h> here, so that orig_fchmodat doesn't recurse to
   rpl_fchmodat.  */
#define __need_system_sys_stat_h
#include <config.h>

/* Specification.  */
#include <sys/stat.h>
#undef __need_system_sys_stat_h

#if HAVE_FCHMODAT
static int
orig_fchmodat (int dir, char const *file, mode_t mode, int flags)
{
  return fchmodat (dir, file, mode, flags);
}
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __osf__
/* Write "sys/stat.h" here, not <sys/stat.h>, otherwise OSF/1 5.1 DTK cc
   eliminates this include because of the preliminary #include <sys/stat.h>
   above.  */
# include "sys/stat.h"
#else
# include <sys/stat.h>
#endif

#include <intprops.h>

/* Invoke chmod or lchmod on FILE, using mode MODE, in the directory
   open on descriptor FD.  If possible, do it without changing the
   working directory.  Otherwise, resort to using save_cwd/fchdir,
   then (chmod|lchmod)/restore_cwd.  If either the save_cwd or the
   restore_cwd fails, then give a diagnostic and exit nonzero.
   Note that an attempt to use a FLAG value of AT_SYMLINK_NOFOLLOW
   on a system without lchmod support causes this function to fail.  */

#if HAVE_FCHMODAT
int
fchmodat (int dir, char const *file, mode_t mode, int flags)
{
  if (flags & AT_SYMLINK_NOFOLLOW)
    {
# ifdef O_PATH
      int fd = openat (dir, file, O_PATH | O_NOFOLLOW | O_CLOEXEC);
      if (fd < 0)
        return fd;
      static char const fmt[] = "/proc/self/fd/%d";
      char buf[sizeof fmt - sizeof "%d" + INT_BUFSIZE_BOUND (int)];
      sprintf (buf, fmt, fd);
      int chmod_result = chmod (buf, mode);
      int chmod_errno = errno;
      close (fd);
      if (chmod_result == 0)
        return chmod_result;
      if (chmod_errno != ENOENT)
        {
          errno = chmod_errno;
          return chmod_result;
        }
      /* /proc is not mounted; fall back on racy implementation.  */
# endif

      struct stat st;
      int fstatat_result = fstatat (dir, file, &st, AT_SYMLINK_NOFOLLOW);
      if (fstatat_result != 0)
        return fstatat_result;
      if (S_ISLNK (st.st_mode))
        {
          errno = EOPNOTSUPP;
          return -1;
        }
      flags &= ~AT_SYMLINK_NOFOLLOW;
    }

  return orig_fchmodat (dir, file, mode, flags);
}
#else
# define AT_FUNC_NAME fchmodat
# define AT_FUNC_F1 lchmod
# define AT_FUNC_F2 chmod
# define AT_FUNC_USE_F1_COND AT_SYMLINK_NOFOLLOW
# define AT_FUNC_POST_FILE_PARAM_DECLS , mode_t mode, int flag
# define AT_FUNC_POST_FILE_ARGS        , mode
# include "at-func.c"
#endif
