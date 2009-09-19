/* Create a named fifo relative to an open directory.
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

#ifndef HAVE_MKFIFO
# define HAVE_MKFIFO 0
#endif
#ifndef HAVE_MKNOD
# define HAVE_MKNOD 0
#endif

/* For now, all known systems either have both mkfifo and mknod, or
   neither.  If this is not true, we can implement the portable
   aspects of one using the other (POSIX only requires mknod to create
   fifos; all other uses of mknod are for root users and outside the
   realm of POSIX).  */
#if HAVE_MKNOD != HAVE_MKFIFO
# error Please report this message and your system to bug-gnulib@gnu.org.
#endif

#if !HAVE_MKFIFO
/* Mingw lacks mkfifo and mknod, so this wrapper is trivial.  */

# include <errno.h>

int
mkfifoat (int fd _UNUSED_PARAMETER_, char const *path _UNUSED_PARAMETER_,
	  mode_t mode _UNUSED_PARAMETER_)
{
  errno = ENOSYS;
  return -1;
}

int
mknodat (int fd _UNUSED_PARAMETER_, char const *path _UNUSED_PARAMETER_,
	 mode_t mode _UNUSED_PARAMETER_, dev_t dev _UNUSED_PARAMETER_)
{
  errno = ENOSYS;
  return -1;
}

#else /* HAVE_MKFIFO */

/* Create a named fifo FILE relative to directory FD, with access
   permissions in MODE.  If possible, do it without changing the
   working directory.  Otherwise, resort to using save_cwd/fchdir,
   then mkfifo/restore_cwd.  If either the save_cwd or the restore_cwd
   fails, then give a diagnostic and exit nonzero.  */

# define AT_FUNC_NAME mkfifoat
# define AT_FUNC_F1 mkfifo
# define AT_FUNC_POST_FILE_PARAM_DECLS , mode_t mode
# define AT_FUNC_POST_FILE_ARGS        , mode
# include "at-func.c"
# undef AT_FUNC_NAME
# undef AT_FUNC_F1
# undef AT_FUNC_POST_FILE_PARAM_DECLS
# undef AT_FUNC_POST_FILE_ARGS

/* Create a file system node FILE relative to directory FD, with
   access permissions and file type in MODE, and device type in DEV.
   Usually, non-root applications can only create named fifos, with
   DEV set to 0.  If possible, create the node without changing the
   working directory.  Otherwise, resort to using save_cwd/fchdir,
   then mknod/restore_cwd.  If either the save_cwd or the restore_cwd
   fails, then give a diagnostic and exit nonzero.  */

# define AT_FUNC_NAME mknodat
# define AT_FUNC_F1 mknod
# define AT_FUNC_POST_FILE_PARAM_DECLS , mode_t mode, dev_t dev
# define AT_FUNC_POST_FILE_ARGS        , mode, dev
# include "at-func.c"
# undef AT_FUNC_NAME
# undef AT_FUNC_F1
# undef AT_FUNC_POST_FILE_PARAM_DECLS
# undef AT_FUNC_POST_FILE_ARGS

#endif /* HAVE_MKFIFO */
