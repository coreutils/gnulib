/* A more POSIX-compliant chown

   Copyright (C) 1997, 2004-2007, 2009-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "issymlink.h"
#include "stat-time.h"

#ifndef CHOWN_CHANGE_TIME_BUG
# define CHOWN_CHANGE_TIME_BUG 0
#endif
#ifndef CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE
# define CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE 0
#endif
#ifndef CHOWN_MODIFIES_SYMLINK
# define CHOWN_MODIFIES_SYMLINK 0
#endif
#ifndef CHOWN_TRAILING_SLASH_BUG
# define CHOWN_TRAILING_SLASH_BUG 0
#endif

/* Gnulib target platforms lacking utimensat do not need it,
   because in practice the bug it works around does not occur.  */
#if !HAVE_UTIMENSAT
# undef utimensat
# define utimensat(fd, file, times, flag) \
    ((void) (fd), (void) (file), (void) (times), (void) (flag), \
     0)
#endif

#if !HAVE_CHOWN

/* Simple stub that always fails with ENOSYS, for mingw.  */
int
chown (_GL_UNUSED const char *file, _GL_UNUSED uid_t owner,
       _GL_UNUSED gid_t group)
{
  errno = ENOSYS;
  return -1;
}

#else /* HAVE_CHOWN */

/* Below we refer to the system's function.  */
# undef chown

/* Provide a more-closely POSIX-conforming version.  */

int
rpl_chown (const char *file, uid_t owner, gid_t group)
{
  /* In some very old platforms, the system-supplied function
     does not follow symlinks.
     If the file is a symlink, open the file (following symlinks), and
     fchown the resulting descriptor.  Although the open might fail
     due to lack of permissions, it's the best we can easily do.  */
  if (CHOWN_MODIFIES_SYMLINK && 0 < issymlink (file))
    {
      int open_flags = O_NONBLOCK | O_NOCTTY | O_CLOEXEC;
      int fd = open (file, O_RDONLY | open_flags);
      if (fd < 0
          && (errno != EACCES
              || ((fd = open (file, O_WRONLY | open_flags)) < 0))
          && (errno != EACCES || O_EXEC == O_RDONLY
              || ((fd = open (file, O_EXEC | open_flags)) < 0))
          && (errno != EACCES || O_SEARCH == O_RDONLY || O_SEARCH == O_EXEC
              || ((fd = open (file, O_SEARCH | open_flags)) < 0)))
        return fd;

      int r = fchown (fd, owner, group);
      int err = errno;
      close (fd);
      errno = err;
      return r;
    }

  struct stat st;
  gid_t no_gid = -1;
  uid_t no_uid = -1;
  bool gid_noop = group == no_gid;
  bool uid_noop = owner == no_uid;
  bool change_time_check = CHOWN_CHANGE_TIME_BUG && !(gid_noop & uid_noop);
  bool negative_one_check = (CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE
                             && (gid_noop | uid_noop));
  if (change_time_check | negative_one_check
      || (CHOWN_TRAILING_SLASH_BUG
          && file[0] && file[strlen (file) - 1] == '/'))
    {
      int r = stat (file, &st);

      /* EOVERFLOW means the file exists, which is all that the
         trailing slash check needs.  */
      if (r < 0 && (change_time_check | negative_one_check
                    || errno != EOVERFLOW))
        return r;
    }

  gid_t uid = (CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE && uid_noop
               ? st.st_uid : owner);
  gid_t gid = (CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE && gid_noop
               ? st.st_gid : group);
  int result = chown (file, uid, gid);

  /* If no change in ownership, but at least one argument was not -1,
     update ctime indirectly via a no-change update to atime and mtime.
     Do not use UTIME_NOW or UTIME_OMIT as they might run into bugs
     on some platforms.  Do not communicate any failure to the caller
     as that would be worse than communicating the ownership change.  */
  if (result == 0 && change_time_check
      && (((uid == st.st_uid) | uid_noop)
          & ((gid == st.st_gid) | gid_noop)))
    utimensat (AT_FDCWD, file,
               ((struct timespec[]) { get_stat_atime (&st),
                                      get_stat_mtime (&st) }),
               0);

  return result;
}

#endif /* HAVE_CHOWN */
