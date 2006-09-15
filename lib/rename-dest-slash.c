/* A rename wrapper to make tools like mv -- that would normally rely
   on the underlying rename syscall -- work more consistently.
   On at least NetBSD 1.6, `rename ("dir", "B/")' fails when B doesn't
   exist, whereas it succeeds on Linux-2.6.x and Solaris 10.  This wrapper
   provides an interface for systems like the former so that the tools
   (namely mv) relying on the rename syscall have more consistent
   semantics.

   Copyright (C) 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* written by Jim Meyering */

#include <config.h>
#undef rename

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dirname.h"
#include "xalloc.h"

static inline bool
has_trailing_slash (char const *file, size_t len)
{
  /* Don't count "/" as having a trailing slash.  */
  if (len <= 1)
    return false;

  char last = file[len - 1];
  return ISSLASH (last);
}

/* This is a rename wrapper for systems where the rename syscall
   works differently than desired when SRC is a directory and DST does
   not exist but is specified with a trailing slash.  On NetBSD 6.1,
   rename fails in that case.  On Linux and Solaris systems, it succeeds.
   This wrapper makes it succeed on NetBSD by running the originally
   requested rename, and if it fails due to the above scenario, calling
   it again with DST's trailing slashes removed.  */
int
rpl_rename_dest_slash (char const *src, char const *dst)
{
  size_t d_len;
  int ret_val = rename (src, dst);
  if (ret_val == 0 || errno != ENOENT)
    return ret_val;

  {
    /* Fail now, unless SRC is a directory.  */
    struct stat sb;
    if (lstat (src, &sb) != 0 || ! S_ISDIR (sb.st_mode))
      return ret_val;
  }

  /* Don't call rename again if there are no trailing slashes.  */
  d_len = strlen (dst);
  if ( ! has_trailing_slash (dst, d_len))
    return ret_val;

  {
    char *dst_temp;
    dst_temp = xmemdup (dst, d_len + 1);
    strip_trailing_slashes (dst_temp);

    ret_val = rename (src, dst_temp);
    free (dst_temp);
  }

  return ret_val;
}
