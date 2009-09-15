/* Work around platform bugs in stat.
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
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#undef stat

/* For now, mingw is the only known platform where stat(".") and
   stat("./") give different results.  Mingw stat has other bugs (such
   as st_ino always being 0 on success) which this wrapper does not
   work around.  But at least this implementation provides the ability
   to emulate fchdir correctly.  */

int
rpl_stat (char const *name, struct stat *st)
{
  int result = stat (name, st);
  if (result == -1 && errno == ENOENT)
    {
      /* Due to mingw's oddities, there are some directories (like
         c:\) where stat() only succeeds with a trailing slash, and
         other directories (like c:\windows) where stat() only
         succeeds without a trailing slash.  But we want the two to be
         synonymous, since chdir() manages either style.  Likewise, Mingw also
         reports ENOENT for names longer than PATH_MAX, when we want
         ENAMETOOLONG, and for stat("file/"), when we want ENOTDIR.
         Fortunately, mingw PATH_MAX is small enough for stack
         allocation.  */
      char fixed_name[PATH_MAX + 1] = {0};
      size_t len = strlen (name);
      bool check_dir = false;
      if (PATH_MAX <= len)
        errno = ENAMETOOLONG;
      else if (len)
        {
          strcpy (fixed_name, name);
          if (ISSLASH (fixed_name[len - 1]))
            {
              check_dir = true;
              while (len && ISSLASH (fixed_name[len - 1]))
                fixed_name[--len] = '\0';
              if (!len)
                fixed_name[0] = '/';
            }
          else
            fixed_name[len++] = '/';
          result = stat (fixed_name, st);
          if (result == 0 && check_dir && !S_ISDIR (st->st_mode))
            {
              result = -1;
              errno = ENOTDIR;
            }
        }
    }
  return result;
}
