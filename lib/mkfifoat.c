/* Create a named fifo relative to an open directory.
   Copyright (C) 2009-2016 Free Software Foundation, Inc.

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

#if !HAVE_MKFIFO

# include <errno.h>

/* Mingw lacks mkfifo, so this wrapper is trivial.  */

int
mkfifoat (int fd _GL_UNUSED, char const *path _GL_UNUSED,
          mode_t mode _GL_UNUSED)
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

#endif /* HAVE_MKFIFO */
