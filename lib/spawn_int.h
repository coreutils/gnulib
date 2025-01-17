/* Copyright (C) 2000, 2008-2025 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

#include <sys/types.h>

/* Data structure to contain the action information.  */
struct __spawn_action
{
  enum
  {
    spawn_do_close,
    spawn_do_dup2,
    spawn_do_open,
    spawn_do_chdir,
    spawn_do_fchdir
  } tag;

  union
  {
    struct
    {
      int fd;
    } close_action;
    struct
    {
      int fd;
      int newfd;
    } dup2_action;
    struct
    {
      int fd;
      char *path;
      int oflag;
      mode_t mode;
    } open_action;
    struct
    {
      char *path;
    } chdir_action;
    struct
    {
      int fd;
    } fchdir_action;
  } action;
};

#if !_LIBC
# define __posix_spawn_file_actions_realloc gl_posix_spawn_file_actions_realloc
#endif
extern int __posix_spawn_file_actions_realloc (posix_spawn_file_actions_t *
                                               file_actions);

#if !_LIBC
# define __spawni gl_posix_spawn_internal
#endif
extern int __spawni (pid_t *pid, const char *path,
                     const posix_spawn_file_actions_t *file_actions,
                     const posix_spawnattr_t *attrp, const char *const argv[],
                     const char *const envp[], int use_path);
