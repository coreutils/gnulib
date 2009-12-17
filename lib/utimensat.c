/* Set the access and modification time of a file relative to directory fd.
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

/* written by Eric Blake */

#include <config.h>

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>

#include "stat-time.h"
#include "timespec.h"
#include "utimens.h"

#if HAVE_UTIMENSAT

# undef utimensat

/* If we have a native utimensat, but are compiling this file, then
   utimensat was defined to rpl_utimensat by our replacement
   sys/stat.h.  We assume the native version might fail with ENOSYS,
   or succeed without properly affecting ctime (as is the case when
   using newer glibc but older Linux kernel).  In this scenario,
   rpl_utimensat checks whether the native version is usable, and
   local_utimensat provides the fallback manipulation.  */

static int local_utimensat (int, char const *, struct timespec const[2], int);
# define AT_FUNC_NAME local_utimensat

/* Like utimensat, but work around native bugs.  */

int
rpl_utimensat (int fd, char const *file, struct timespec const times[2],
               int flag)
{
  /* See comments in utimens.c for details.  */
  static int utimensat_works_really; /* 0 = unknown, 1 = yes, -1 = no.  */
  static int utimensat_ctime_really; /* 0 = unknown, 1 = yes, -1 = no.  */
  if (0 <= utimensat_works_really)
    {
      int result;
      struct stat st1;
      struct stat st2;
      struct timespec ts[2];
      /* Linux kernel 2.6.32 has a bug where mtime of UTIME_OMIT fails
         to change ctime.  */
      if (utimensat_ctime_really <= 0 && times
          && times[0].tv_nsec != UTIME_OMIT && times[1].tv_nsec == UTIME_OMIT)
        {
          if (fstatat (fd, file, &st1, flag))
            return -1;
          if (utimensat_ctime_really < 0)
            {
              ts[0] = times[0];
              ts[1] = get_stat_mtime (&st1);
              times = ts;
            }
        }
      result = utimensat (fd, file, times, flag);
      /* Linux kernel 2.6.25 has a bug where it returns EINVAL for
         UTIME_NOW or UTIME_OMIT with non-zero tv_sec, which
         local_utimensat works around.  Meanwhile, EINVAL for a bad
         flag is indeterminate whether the native utimensat works, but
         local_utimensat will also reject it.  */
      if (result == -1 && errno == EINVAL && (flag & ~AT_SYMLINK_NOFOLLOW))
        return result;
      if (result == 0 || (errno != ENOSYS && errno != EINVAL))
        {
          utimensat_works_really = 1;
          if (result == 0 && utimensat_ctime_really == 0 && times
              && times[0].tv_nsec != UTIME_OMIT
              && times[1].tv_nsec == UTIME_OMIT)
            {
              /* Perform a followup [l]stat.  See detect_ctime_bug in
                 utimens.c for more details.  */
              struct timespec now;
              if (fstatat (fd, file, &st2, flag))
                return -1;
              if (st1.st_ctime != st2.st_ctime
                  || get_stat_ctime_ns (&st1) != get_stat_ctime_ns (&st2))
                {
                  utimensat_ctime_really = 1;
                  return result;
                }
              if (times[0].tv_nsec == UTIME_NOW)
                now = get_stat_atime (&st2);
              else
                gettime (&now);
              if (now.tv_sec < st2.st_ctime
                  || 2 < now.tv_sec - st2.st_ctime
                  || (get_stat_ctime_ns (&st2)
                      && now.tv_sec - st2.st_ctime < 2
                      && (20000000 < (1000000000 * (now.tv_sec - st2.st_ctime)
                                      + now.tv_nsec
                                      - get_stat_ctime_ns (&st2)))))
                utimensat_ctime_really = -1;
              ts[0] = times[0];
              ts[1] = get_stat_mtime (&st2);
              result = utimensat (fd, file, ts, flag);
            }
          return result;
        }
    }
  /* No point in trying openat/futimens, since on Linux, futimens is
     implemented with the same syscall as utimensat.  Only avoid the
     native utimensat due to an ENOSYS failure; an EINVAL error was
     data-dependent, and the next caller may pass valid data.  */
  if (0 <= utimensat_works_really && errno == ENOSYS)
    utimensat_works_really = -1;
  return local_utimensat (fd, file, times, flag);
}

#else /* !HAVE_UTIMENSAT */

# define AT_FUNC_NAME utimensat

#endif /* !HAVE_UTIMENSAT */

/* Set the access and modification time stamps of FILE to be
   TIMESPEC[0] and TIMESPEC[1], respectively; relative to directory
   FD.  If flag is AT_SYMLINK_NOFOLLOW, change the times of a symlink,
   or fail with ENOSYS if not possible.  If TIMESPEC is null, set the
   time stamps to the current time.  If possible, do it without
   changing the working directory.  Otherwise, resort to using
   save_cwd/fchdir, then utimens/restore_cwd.  If either the save_cwd
   or the restore_cwd fails, then give a diagnostic and exit nonzero.
   Return 0 on success, -1 (setting errno) on failure.  */

/* AT_FUNC_NAME is now utimensat or local_utimensat.  */
#define AT_FUNC_F1 lutimens
#define AT_FUNC_F2 utimens
#define AT_FUNC_USE_F1_COND AT_SYMLINK_NOFOLLOW
#define AT_FUNC_POST_FILE_PARAM_DECLS , struct timespec const ts[2], int flag
#define AT_FUNC_POST_FILE_ARGS        , ts
#include "at-func.c"
#undef AT_FUNC_NAME
#undef AT_FUNC_F1
#undef AT_FUNC_F2
#undef AT_FUNC_USE_F1_COND
#undef AT_FUNC_POST_FILE_PARAM_DECLS
#undef AT_FUNC_POST_FILE_ARGS
