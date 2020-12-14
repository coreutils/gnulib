/* Return the canonical absolute name of a given file.
   Copyright (C) 1996-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _LIBC
/* Don't use __attribute__ __nonnull__ in this compilation unit.  Otherwise gcc
   optimizes away the name == NULL test below.  */
# define _GL_ARG_NONNULL(params)

# define _GL_USE_STDLIB_ALLOC 1
# include <libc-config.h>
#endif

/* Specification.  */
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stddef.h>

#ifdef _LIBC
# include <eloop-threshold.h>
# include <shlib-compat.h>
typedef ptrdiff_t idx_t;
# define FILE_SYSTEM_PREFIX_LEN(name) 0
# define IS_ABSOLUTE_FILE_NAME(name) ISSLASH(*(name))
# define ISSLASH(c) ((c) == '/')
# define malloca __alloca
# define freea(p) ((void) (p))
#else
# define __canonicalize_file_name canonicalize_file_name
# define __realpath realpath
# include "idx.h"
# include "pathmax.h"
# include "malloca.h"
# include "filename.h"
# if defined _WIN32 && !defined __CYGWIN__
#  define __getcwd _getcwd
# elif HAVE_GETCWD
#  if IN_RELOCWRAPPER
    /* When building the relocatable program wrapper, use the system's getcwd
       function, not the gnulib override, otherwise we would get a link error.
     */
#   undef getcwd
#  endif
#  if defined VMS && !defined getcwd
    /* We want the directory in Unix syntax, not in VMS syntax.
       The gnulib override of 'getcwd' takes 2 arguments; the original VMS
       'getcwd' takes 3 arguments.  */
#   define __getcwd(buf, max) getcwd (buf, max, 0)
#  else
#   define __getcwd getcwd
#  endif
# else
#  define __getcwd(buf, max) getwd (buf)
# endif
# define __mempcpy mempcpy
# define __pathconf pathconf
# define __rawmemchr rawmemchr
# define __readlink readlink
# ifndef MAXSYMLINKS
#  ifdef SYMLOOP_MAX
#   define MAXSYMLINKS SYMLOOP_MAX
#  else
#   define MAXSYMLINKS 20
#  endif
# endif
# define __eloop_threshold() MAXSYMLINKS
#endif

#ifndef DOUBLE_SLASH_IS_DISTINCT_ROOT
# define DOUBLE_SLASH_IS_DISTINCT_ROOT 0
#endif

#if !FUNC_REALPATH_WORKS || defined _LIBC

/* Return the canonical absolute name of file NAME.  A canonical name
   does not contain any ".", ".." components nor any repeated path
   separators ('/') or symlinks.  All path components must exist.  If
   RESOLVED is null, the result is malloc'd; otherwise, if the
   canonical name is PATH_MAX chars or more, returns null with 'errno'
   set to ENAMETOOLONG; if the name fits in fewer than PATH_MAX chars,
   returns the name in RESOLVED.  If the name cannot be resolved and
   RESOLVED is non-NULL, it contains the path of the first component
   that cannot be resolved.  If the path can be resolved, RESOLVED
   holds the same value as the value returned.  */

char *
__realpath (const char *name, char *resolved)
{
  char *rpath, *dest, *extra_buf = NULL;
  const char *start, *end, *rpath_limit;
  long int path_max;
  int num_links = 0;
  size_t prefix_len;

  if (name == NULL)
    {
      /* As per Single Unix Specification V2 we must return an error if
         either parameter is a null pointer.  We extend this to allow
         the RESOLVED parameter to be NULL in case the we are expected to
         allocate the room for the return value.  */
      __set_errno (EINVAL);
      return NULL;
    }

  if (name[0] == '\0')
    {
      /* As per Single Unix Specification V2 we must return an error if
         the name argument points to an empty string.  */
      __set_errno (ENOENT);
      return NULL;
    }

#ifdef PATH_MAX
  path_max = PATH_MAX;
#else
  path_max = __pathconf (name, _PC_PATH_MAX);
  if (path_max <= 0)
    path_max = 1024;
#endif

  if (resolved == NULL)
    {
      rpath = malloc (path_max);
      if (rpath == NULL)
        return NULL;
    }
  else
    rpath = resolved;
  rpath_limit = rpath + path_max;

  /* This is always zero for Posix hosts, but can be 2 for MS-Windows
     and MS-DOS X:/foo/bar file names.  */
  prefix_len = FILE_SYSTEM_PREFIX_LEN (name);

  if (!IS_ABSOLUTE_FILE_NAME (name))
    {
      if (!__getcwd (rpath, path_max))
        {
          rpath[0] = '\0';
          goto error;
        }
      dest = __rawmemchr (rpath, '\0');
      start = name;
      prefix_len = FILE_SYSTEM_PREFIX_LEN (rpath);
    }
  else
    {
      dest = __mempcpy (rpath, name, prefix_len);
      *dest++ = '/';
      if (DOUBLE_SLASH_IS_DISTINCT_ROOT)
        {
          if (ISSLASH (name[1]) && !ISSLASH (name[2]) && !prefix_len)
            *dest++ = '/';
          *dest = '\0';
        }
      start = name + prefix_len;
    }

  for (end = start; *start; start = end)
    {
      /* Skip sequence of multiple path-separators.  */
      while (ISSLASH (*start))
        ++start;

      /* Find end of path component.  */
      for (end = start; *end && !ISSLASH (*end); ++end)
        /* Nothing.  */;

      if (end - start == 1 && start[0] == '.')
        /* nothing */;
      else if (end - start == 2 && start[0] == '.' && start[1] == '.')
        {
          /* Back up to previous component, ignore if at root already.  */
          if (dest > rpath + prefix_len + 1)
            for (--dest; dest > rpath && !ISSLASH (dest[-1]); --dest)
              continue;
          if (DOUBLE_SLASH_IS_DISTINCT_ROOT
              && dest == rpath + 1 && !prefix_len
              && ISSLASH (*dest) && !ISSLASH (dest[1]))
            dest++;
        }
      else
        {
          size_t new_size;

          if (!ISSLASH (dest[-1]))
            *dest++ = '/';

          if (rpath_limit - dest <= end - start)
            {
              idx_t dest_offset = dest - rpath;
              char *new_rpath;

              if (resolved)
                {
                  __set_errno (ENAMETOOLONG);
                  if (dest > rpath + prefix_len + 1)
                    dest--;
                  *dest = '\0';
                  goto error;
                }
              new_size = rpath_limit - rpath;
              if (end - start + 1 > path_max)
                new_size += end - start + 1;
              else
                new_size += path_max;
              new_rpath = (char *) realloc (rpath, new_size);
              if (new_rpath == NULL)
                goto error;
              rpath = new_rpath;
              rpath_limit = rpath + new_size;

              dest = rpath + dest_offset;
            }

          dest = __mempcpy (dest, start, end - start);
          *dest = '\0';

          char linkbuf[128];
          ssize_t n = __readlink (rpath, linkbuf, sizeof linkbuf);
          if (n < 0)
            {
              if (errno != EINVAL)
                goto error;
            }
          else
            {
              char *buf;
              size_t len;

              if (++num_links > __eloop_threshold ())
                {
                  __set_errno (ELOOP);
                  goto error;
                }

              if (!extra_buf)
                {
                  extra_buf = malloca (2 * path_max);
                  if (!extra_buf)
                    goto error;
                }
              if (n < sizeof linkbuf)
                buf = linkbuf;
              else
                {
                  buf = extra_buf + path_max;
                  n = __readlink (rpath, buf, path_max - 1);
                  if (n < 0)
                    goto error;
                }
              buf[n] = '\0';

              len = strlen (end);
              if (path_max - n <= len)
                {
                  __set_errno (ENAMETOOLONG);
                  goto error;
                }

              /* Careful here, end may be a pointer into extra_buf... */
              memmove (&extra_buf[n], end, len + 1);
              name = end = memcpy (extra_buf, buf, n);

              if (IS_ABSOLUTE_FILE_NAME (buf))
                {
                  size_t pfxlen = FILE_SYSTEM_PREFIX_LEN (buf);

                  dest = __mempcpy (rpath, buf, pfxlen);
                  *dest++ = '/'; /* It's an absolute symlink */
                  if (DOUBLE_SLASH_IS_DISTINCT_ROOT)
                    {
                      if (ISSLASH (buf[1]) && !ISSLASH (buf[2]) && !pfxlen)
                        *dest++ = '/';
                      *dest = '\0';
                    }
                  /* Install the new prefix to be in effect hereafter.  */
                  prefix_len = pfxlen;
                }
              else
                {
                  /* Back up to previous component, ignore if at root
                     already: */
                  if (dest > rpath + prefix_len + 1)
                    for (--dest; dest > rpath && !ISSLASH (dest[-1]); --dest)
                      continue;
                  if (DOUBLE_SLASH_IS_DISTINCT_ROOT && dest == rpath + 1
                      && ISSLASH (*dest) && !ISSLASH (dest[1]) && !prefix_len)
                    dest++;
                }
            }
        }
    }
  if (dest > rpath + prefix_len + 1 && ISSLASH (dest[-1]))
    --dest;
  if (DOUBLE_SLASH_IS_DISTINCT_ROOT && dest == rpath + 1 && !prefix_len
      && ISSLASH (*dest) && !ISSLASH (dest[1]))
    dest++;
  *dest = '\0';

  if (extra_buf)
    freea (extra_buf);

  return rpath;

error:
  if (extra_buf)
    freea (extra_buf);
  if (resolved == NULL)
    free (rpath);
  return NULL;
}
libc_hidden_def (__realpath)
versioned_symbol (libc, __realpath, realpath, GLIBC_2_3);
#endif /* !FUNC_REALPATH_WORKS || defined _LIBC */


#if SHLIB_COMPAT(libc, GLIBC_2_0, GLIBC_2_3)
char *
attribute_compat_text_section
__old_realpath (const char *name, char *resolved)
{
  if (resolved == NULL)
    {
      __set_errno (EINVAL);
      return NULL;
    }

  return __realpath (name, resolved);
}
compat_symbol (libc, __old_realpath, realpath, GLIBC_2_0);
#endif


char *
__canonicalize_file_name (const char *name)
{
  return __realpath (name, NULL);
}
weak_alias (__canonicalize_file_name, canonicalize_file_name)
