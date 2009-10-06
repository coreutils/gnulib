/* Create a hard link relative to open directories.
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

#include <unistd.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "areadlink.h"
#include "dirname.h"
#include "filenamecat.h"
#include "openat-priv.h"

#if HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifndef MAXSYMLINKS
# ifdef SYMLOOP_MAX
#  define MAXSYMLINKS SYMLOOP_MAX
# else
#  define MAXSYMLINKS 20
# endif
#endif

#if !HAVE_LINKAT

/* Create a link.  If FILE1 is a symlink, either create a hardlink to
   that symlink, or fake it by creating an identical symlink.  */
# if LINK_FOLLOWS_SYMLINKS == 0
#  define link_immediate link
# else
static int
link_immediate (char const *file1, char const *file2)
{
  char *target = areadlink (file1);
  if (target)
    {
      /* A symlink cannot be modified in-place.  Therefore, creating
         an identical symlink behaves like a hard link to a symlink,
         except for incorrect st_ino and st_nlink.  However, we must
         be careful of EXDEV.  */
      struct stat st1;
      struct stat st2;
      char *dir = mdir_name (file2);
      if (!dir)
        {
          free (target);
          errno = ENOMEM;
          return -1;
        }
      if (lstat (file1, &st1) == 0 && stat (dir, &st2) == 0)
        {
          if (st1.st_dev == st2.st_dev)
            {
              int result = symlink (target, file2);
              int saved_errno = errno;
              free (target);
              free (dir);
              errno = saved_errno;
              return result;
            }
          free (target);
          free (dir);
          errno = EXDEV;
          return -1;
        }
      free (target);
      free (dir);
    }
  if (errno == ENOMEM)
    return -1;
  return link (file1, file2);
}
# endif /* LINK_FOLLOWS_SYMLINKS == 0 */

/* Create a link.  If FILE1 is a symlink, create a hardlink to the
   canonicalized file.  */
# if 0 < LINK_FOLLOWS_SYMLINKS
#  define link_follow link
# else
static int
link_follow (char const *file1, char const *file2)
{
  char *name = (char *) file1;
  char *target;
  int result;
  int i = MAXSYMLINKS;

  /* Using realpath or canonicalize_file_name is too heavy-handed: we
     don't need an absolute name, and we don't need to resolve
     intermediate symlinks, just the basename of each iteration.  */
  while (i-- && (target = areadlink (name)))
    {
      if (IS_ABSOLUTE_FILE_NAME (target))
        {
          if (name != file1)
            free (name);
          name = target;
        }
      else
        {
          char *dir = mdir_name (name);
          if (name != file1)
            free (name);
          if (!dir)
            {
              free (target);
              errno = ENOMEM;
              return -1;
            }
          name = mfile_name_concat (dir, target, NULL);
          free (dir);
          free (target);
          if (!name)
            {
              errno = ENOMEM;
              return -1;
            }
        }
    }
  if (i < 0)
    {
      target = NULL;
      errno = ELOOP;
    }
  if (!target && errno != EINVAL)
    {
      if (name != file1)
        {
          int saved_errno = errno;
          free (name);
          errno = saved_errno;
        }
      return -1;
    }
  result = link (name, file2);
  if (name != file1)
    {
      int saved_errno = errno;
      free (name);
      errno = saved_errno;
    }
  return result;
}
# endif /* 0 < LINK_FOLLOWS_SYMLINKS */

/* Create a link to FILE1, in the directory open on descriptor FD1, to FILE2,
   in the directory open on descriptor FD2.  If FILE1 is a symlink, FLAG
   controls whether to dereference FILE1 first.  If possible, do it without
   changing the working directory.  Otherwise, resort to using
   save_cwd/fchdir, then rename/restore_cwd.  If either the save_cwd or
   the restore_cwd fails, then give a diagnostic and exit nonzero.  */

int
linkat (int fd1, char const *file1, int fd2, char const *file2, int flag)
{
  if (flag & ~AT_SYMLINK_FOLLOW)
    {
      errno = EINVAL;
      return -1;
    }
  return at_func2 (fd1, file1, fd2, file2,
                   flag ? link_follow : link_immediate);
}

#else /* HAVE_LINKAT */

# undef linkat

/* Read a symlink, like areadlink, but relative to FD.  */

static char *
areadlinkat (int fd, char const *filename)
{
  /* The initial buffer size for the link value.  A power of 2
     detects arithmetic overflow earlier, but is not required.  */
# define INITIAL_BUF_SIZE 1024

  /* Allocate the initial buffer on the stack.  This way, in the common
     case of a symlink of small size, we get away with a single small malloc()
     instead of a big malloc() followed by a shrinking realloc().  */
  char initial_buf[INITIAL_BUF_SIZE];

  char *buffer = initial_buf;
  size_t buf_size = sizeof (initial_buf);

  while (1)
    {
      /* Attempt to read the link into the current buffer.  */
      ssize_t link_length = readlinkat (fd, filename, buffer, buf_size);

      /* On AIX 5L v5.3 and HP-UX 11i v2 04/09, readlink returns -1
         with errno == ERANGE if the buffer is too small.  */
      if (link_length < 0 && errno != ERANGE)
        {
          if (buffer != initial_buf)
            {
              int saved_errno = errno;
              free (buffer);
              errno = saved_errno;
            }
          return NULL;
        }

      if ((size_t) link_length < buf_size)
        {
          buffer[link_length++] = '\0';

          /* Return it in a chunk of memory as small as possible.  */
          if (buffer == initial_buf)
            {
              buffer = (char *) malloc (link_length);
              if (buffer == NULL)
                /* errno is ENOMEM.  */
                return NULL;
              memcpy (buffer, initial_buf, link_length);
            }
          else
            {
              /* Shrink buffer before returning it.  */
              if ((size_t) link_length < buf_size)
                {
                  char *smaller_buffer = (char *) realloc (buffer, link_length);

                  if (smaller_buffer != NULL)
                    buffer = smaller_buffer;
                }
            }
          return buffer;
        }

      if (buffer != initial_buf)
        free (buffer);
      buf_size *= 2;
      if (SSIZE_MAX < buf_size || (SIZE_MAX / 2 < SSIZE_MAX && buf_size == 0))
        {
          errno = ENOMEM;
          return NULL;
        }
      buffer = (char *) malloc (buf_size);
      if (buffer == NULL)
        /* errno is ENOMEM.  */
        return NULL;
    }
}

/* Create a link.  If FILE1 is a symlink, create a hardlink to the
   canonicalized file.  */

static int
linkat_follow (int fd1, char const *file1, int fd2, char const *file2)
{
  char *name = (char *) file1;
  char *target;
  int result;
  int i = MAXSYMLINKS;

  /* There is no realpathat.  */
  while (i-- && (target = areadlinkat (fd1, name)))
    {
      if (IS_ABSOLUTE_FILE_NAME (target))
        {
          if (name != file1)
            free (name);
          name = target;
        }
      else
        {
          char *dir = mdir_name (name);
          if (name != file1)
            free (name);
          if (!dir)
            {
              free (target);
              errno = ENOMEM;
              return -1;
            }
          name = mfile_name_concat (dir, target, NULL);
          free (dir);
          free (target);
          if (!name)
            {
              errno = ENOMEM;
              return -1;
            }
        }
    }
  if (i < 0)
    {
      target = NULL;
      errno = ELOOP;
    }
  if (!target && errno != EINVAL)
    {
      if (name != file1)
        {
          int saved_errno = errno;
          free (name);
          errno = saved_errno;
        }
      return -1;
    }
  result = linkat (fd1, name, fd2, file2, 0);
  if (name != file1)
    {
      int saved_errno = errno;
      free (name);
      errno = saved_errno;
    }
  return result;
}


/* Like linkat, but guarantee that AT_SYMLINK_FOLLOW works even on
   older Linux kernels.  */

int
rpl_linkat (int fd1, char const *file1, int fd2, char const *file2, int flag)
{
  if (!flag)
    return linkat (fd1, file1, fd2, file2, flag);
  if (flag & ~AT_SYMLINK_FOLLOW)
    {
      errno = EINVAL;
      return -1;
    }

  /* Cache the information on whether the system call really works.  */
  {
    static int have_follow_really; /* 0 = unknown, 1 = yes, -1 = no */
    if (0 <= have_follow_really)
    {
      int result = linkat (fd1, file1, fd2, file2, flag);
      if (!(result == -1 && errno == EINVAL))
        {
          have_follow_really = 1;
          return result;
        }
      have_follow_really = -1;
    }
  }
  return linkat_follow (fd1, file1, fd2, file2);
}

#endif /* HAVE_LINKAT */
