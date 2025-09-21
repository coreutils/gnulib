/* A more POSIX-compliant lchown

   Copyright (C) 1998-1999, 2002, 2004, 2006-2007, 2009-2025 Free Software
   Foundation, Inc.

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

#if !HAVE_LCHOWN

/* If the system chown does not follow symlinks, we don't want it
   replaced by gnulib's chown, which does follow symlinks.  */
# if CHOWN_MODIFIES_SYMLINK /* native Windows and some very old platforms */
#  undef chown
# endif

/* Work just like chown, except when FILE is a symbolic link.
   In that case, set errno to EOPNOTSUPP and return -1.
   But if autoconf tests determined that chown modifies
   symlinks, then just call chown.  */

int
lchown (_GL_UNUSED char const *file, _GL_UNUSED uid_t owner,
        _GL_UNUSED gid_t group)
{
# if HAVE_CHOWN
  if (!CHOWN_MODIFIES_SYMLINK && 0 < issymlink (file))
    {
      errno = EOPNOTSUPP;
      return -1;
    }

  return chown (file, owner, group);

# else /* !HAVE_CHOWN */
  errno = ENOSYS;
  return -1;
# endif
}

#else /* HAVE_LCHOWN */

/* Below we refer to the system's function.  */
# undef lchown

/* Provide a more-closely POSIX-conforming version.  */

int
rpl_lchown (const char *file, uid_t owner, gid_t group)
{
  struct stat st;
  gid_t no_gid = -1;
  uid_t no_uid = -1;
  bool gid_noop = group == no_gid;
  bool uid_noop = owner == no_uid;
  bool change_time_check = CHOWN_CHANGE_TIME_BUG && !(gid_noop & uid_noop);

  if (change_time_check
      || (CHOWN_TRAILING_SLASH_BUG
          && file[0] && file[strlen (file) - 1] == '/'))
    {
      bool file_is_symlink = false;
      int r = lstat (file, &st);
      if (0 <= r)
        file_is_symlink = !!S_ISLNK (st.st_mode);
      else if (errno != EOVERFLOW)
        return r;
      else
        {
          int s = issymlink (file);
          if (s < 0)
            return s;
          if (0 < s)
            {
              errno = EOVERFLOW;
              return -1;
            }
          /* FILE exists and is not a symbolic link; ST is unset.
             Rely on Gnulib chown to work around platform chown bugs.  */
        }

      if (!file_is_symlink)
        return chown (file, owner, group);
    }

  int result = lchown (file, owner, group);

  /* If no change in ownership, but at least one argument was not -1,
     update ctime indirectly via a no-change update to atime and mtime.
     Do not use UTIME_NOW or UTIME_OMIT as they might run into bugs
     on some platforms.  Do not communicate any failure to the caller
     as that would be worse than communicating the ownership change.  */
  if (result == 0 && change_time_check
      && (((owner == st.st_uid) | uid_noop)
          & ((group == st.st_gid) | gid_noop)))
    utimensat (AT_FDCWD, file,
               ((struct timespec[]) { get_stat_atime (&st),
                                      get_stat_mtime (&st) }),
               AT_SYMLINK_NOFOLLOW);

  return result;
}

#endif /* HAVE_LCHOWN */
