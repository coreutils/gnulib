/* Rename a file relative to open directories.
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

#include <stdio.h>

#include "openat-priv.h"

/* Rename FILE1, in the directory open on descriptor FD1, to FILE2, in
   the directory open on descriptor FD2.  If possible, do it without
   changing the working directory.  Otherwise, resort to using
   save_cwd/fchdir, then rename/restore_cwd.  If either the save_cwd or
   the restore_cwd fails, then give a diagnostic and exit nonzero.  */

int
renameat (int fd1, char const *file1, int fd2, char const *file2)
{
  return at_func2 (fd1, file1, fd2, file2, rename);
}
