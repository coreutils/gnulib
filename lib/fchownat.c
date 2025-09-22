/* A more POSIX-compliant fchownat

   This serves as replacement for a missing fchownat function,
   as well as a workaround for the fchownat bug in glibc-2.4:
    <https://lists.ubuntu.com/archives/ubuntu-users/2006-September/093218.html>
   when the buggy fchownat-with-AT_SYMLINK_NOFOLLOW operates on a symlink, it
   mistakenly affects the symlink referent, rather than the symlink itself.

   Copyright (C) 2006-2007, 2009-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* written by Jim Meyering */

#include <config.h>

/* Specification.  */
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "openat.h"
#include "stat-time.h"

#ifndef CHOWN_CHANGE_TIME_BUG
# define CHOWN_CHANGE_TIME_BUG 0
#endif
#ifndef CHOWN_TRAILING_SLASH_BUG
# define CHOWN_TRAILING_SLASH_BUG 0
#endif
#ifndef FCHOWNAT_EMPTY_FILENAME_BUG
# define FCHOWNAT_EMPTY_FILENAME_BUG 0
#endif

/* Gnulib target platforms lacking utimensat do not need it,
   because in practice the bug it works around does not occur.  */
#if !HAVE_UTIMENSAT
# undef utimensat
# define utimensat(fd, file, times, flag) \
    ((void) (fd), (void) (file), (void) (times), (void) (flag), \
     0)
#endif

#if !HAVE_FCHOWNAT

/* Replacement for Solaris' function by the same name.
   Invoke chown or lchown on file, FILE, using OWNER and GROUP, in the
   directory open on descriptor FD.  If FLAG is AT_SYMLINK_NOFOLLOW, then
   use lchown, otherwise, use chown.  If possible, do it without changing
   the working directory.  Otherwise, resort to using save_cwd/fchdir,
   then (chown|lchown)/restore_cwd.  If either the save_cwd or the
   restore_cwd fails, then give a diagnostic and exit nonzero.  */

# define AT_FUNC_NAME fchownat
# define AT_FUNC_F1 lchown
# define AT_FUNC_F2 chown
# define AT_FUNC_USE_F1_COND AT_SYMLINK_NOFOLLOW
# define AT_FUNC_POST_FILE_PARAM_DECLS , uid_t owner, gid_t group, int flag
# define AT_FUNC_POST_FILE_ARGS        , owner, group
# include "at-func.c"
# undef AT_FUNC_NAME
# undef AT_FUNC_F1
# undef AT_FUNC_F2
# undef AT_FUNC_USE_F1_COND
# undef AT_FUNC_POST_FILE_PARAM_DECLS
# undef AT_FUNC_POST_FILE_ARGS

#else /* HAVE_FCHOWNAT */

# undef fchownat

# if FCHOWNAT_NOFOLLOW_BUG

/* Failure to handle AT_SYMLINK_NOFOLLOW requires the /proc/self/fd or
   fchdir workaround to call lchown for lchownat, but there is no need
   to penalize chownat.  */
static int
local_lchownat (int fd, char const *file, uid_t owner, gid_t group);

#  define AT_FUNC_NAME local_lchownat
#  define AT_FUNC_F1 lchown
#  define AT_FUNC_POST_FILE_PARAM_DECLS , uid_t owner, gid_t group
#  define AT_FUNC_POST_FILE_ARGS        , owner, group
#  include "at-func.c"
#  undef AT_FUNC_NAME
#  undef AT_FUNC_F1
#  undef AT_FUNC_POST_FILE_PARAM_DECLS
#  undef AT_FUNC_POST_FILE_ARGS

# endif

/* Work around bugs with trailing slash, using the same workarounds as
   chown and lchown.  */

int
rpl_fchownat (int fd, char const *file, uid_t owner, gid_t group, int flag)
{
  /* No need to worry about CHOWN_FAILS_TO_HONOR_ID_OF_NEGATIVE_ONE
     or CHOWN_MODIFIES_SYMLINK, as no known fchownat implementations
     have these bugs.  */

  if (FCHOWNAT_EMPTY_FILENAME_BUG && file[0] == '\0')
    {
      errno = ENOENT;
      return -1;
    }

  bool trailing_slash_check = (CHOWN_TRAILING_SLASH_BUG
                               && file[0] && file[strlen (file) - 1] == '/');
  if (trailing_slash_check)
    flag &= ~AT_SYMLINK_NOFOLLOW;

# if FCHOWNAT_NOFOLLOW_BUG
  if (flag == AT_SYMLINK_NOFOLLOW)
    return local_lchownat (fd, file, owner, group);
# endif

  struct stat st;
  gid_t no_gid = -1;
  uid_t no_uid = -1;
  bool gid_noop = group == no_gid;
  bool uid_noop = owner == no_uid;
  bool change_time_check = CHOWN_CHANGE_TIME_BUG && !(gid_noop & uid_noop);

  if (change_time_check | trailing_slash_check)
    {
      int r = fstatat (fd, file, &st, flag);

      /* EOVERFLOW means the file exists, which is all that the
         trailing slash check needs.  */
      if (r < 0 && (change_time_check || errno != EOVERFLOW))
        return r;
    }

  int result = fchownat (fd, file, owner, group, flag);

  /* If no change in ownership, but at least one argument was not -1,
     update ctime indirectly via a no-change update to atime and mtime.
     Do not use UTIME_NOW or UTIME_OMIT as they might run into bugs
     on some platforms.  Do not communicate any failure to the caller
     as that would be worse than communicating the ownership change.  */
  if (result == 0 && change_time_check
      && (((owner == st.st_uid) | uid_noop)
          & ((group == st.st_gid) | gid_noop)))
    utimensat (fd, file,
               ((struct timespec[]) { get_stat_atime (&st),
                                      get_stat_mtime (&st) }),
               flag);

  return result;
}

#endif /* HAVE_FCHOWNAT */
