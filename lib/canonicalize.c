/* Return the canonical absolute name of a given file.
   Copyright (C) 1996-2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "canonicalize.h"

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <scratch_buffer.h>

#include "attribute.h"
#include "file-set.h"
#include "idx.h"
#include "hash-triple.h"
#include "xalloc.h"
#include "filename.h"

#ifndef DOUBLE_SLASH_IS_DISTINCT_ROOT
# define DOUBLE_SLASH_IS_DISTINCT_ROOT 0
#endif

#if ISSLASH ('\\')
# define SLASHES "/\\"
#else
# define SLASHES "/"
#endif

#if !((HAVE_CANONICALIZE_FILE_NAME && FUNC_REALPATH_WORKS)      \
      || GNULIB_CANONICALIZE_LGPL)
/* Return the canonical absolute name of file NAME.  A canonical name
   does not contain any ".", ".." components nor any repeated file name
   separators ('/') or symlinks.  All components must exist.
   The result is malloc'd.  */

char *
canonicalize_file_name (const char *name)
{
  return canonicalize_filename_mode (name, CAN_EXISTING);
}
#endif /* !HAVE_CANONICALIZE_FILE_NAME */

static bool
multiple_bits_set (canonicalize_mode_t i)
{
  return (i & (i - 1)) != 0;
}

/* Return true if we've already seen the triple, <FILENAME, dev, ino>.
   If *HT is not initialized, initialize it.  */
static bool
seen_triple (Hash_table **ht, char const *filename, struct stat const *st)
{
  if (*ht == NULL)
    {
      idx_t initial_capacity = 7;
      *ht = hash_initialize (initial_capacity,
                            NULL,
                            triple_hash,
                            triple_compare_ino_str,
                            triple_free);
      if (*ht == NULL)
        xalloc_die ();
    }

  if (seen_file (*ht, filename, st))
    return true;

  record_file (*ht, filename, st);
  return false;
}


/* Act like canonicalize_filename_mode (see below), with an additional argument
   rname_buf that can be used as temporary storage.

   If GCC_LINT is defined, do not inline this function with GCC 10.1
   and later, to avoid creating a pointer to the stack that GCC
   -Wreturn-local-addr incorrectly complains about.  See:
   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93644
   Although the noinline attribute can hurt performance a bit, no better way
   to pacify GCC is known; even an explicit #pragma does not pacify GCC.
   When the GCC bug is fixed this workaround should be limited to the
   broken GCC versions.  */
#if _GL_GNUC_PREREQ (10, 1)
# if defined GCC_LINT || defined lint
__attribute__ ((__noinline__))
# elif __OPTIMIZE__ && !__NO_INLINE__
#  warning "GCC might issue a bogus -Wreturn-local-addr warning here."
#  warning "See <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93644>."
# endif
#endif
static char *
canonicalize_filename_mode_stk (const char *name, canonicalize_mode_t can_mode,
                                struct scratch_buffer *rname_buf)
{
  char *dest;
  char const *start;
  char const *end;
  Hash_table *ht = NULL;
  bool logical = (can_mode & CAN_NOLINKS) != 0;
  int num_links = 0;

  canonicalize_mode_t can_exist = can_mode & CAN_MODE_MASK;
  if (multiple_bits_set (can_exist))
    {
      errno = EINVAL;
      return NULL;
    }

  if (name == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  if (name[0] == '\0')
    {
      errno = ENOENT;
      return NULL;
    }

  struct scratch_buffer extra_buffer, link_buffer;
  scratch_buffer_init (&extra_buffer);
  scratch_buffer_init (&link_buffer);
  scratch_buffer_init (rname_buf);
  char *rname_on_stack = rname_buf->data;
  char *rname = rname_on_stack;
  bool end_in_extra_buffer = false;
  bool failed = true;

  /* This is always zero for Posix hosts, but can be 2 for MS-Windows
     and MS-DOS X:/foo/bar file names.  */
  idx_t prefix_len = FILE_SYSTEM_PREFIX_LEN (name);

  if (!IS_ABSOLUTE_FILE_NAME (name))
    {
      while (!getcwd (rname, rname_buf->length))
        {
          switch (errno)
            {
            case ERANGE:
              if (scratch_buffer_grow (rname_buf))
                break;
              FALLTHROUGH;
            case ENOMEM:
              xalloc_die ();

            default:
              dest = rname;
              goto error;
            }
          rname = rname_buf->data;
        }
      dest = rawmemchr (rname, '\0');
      start = name;
      prefix_len = FILE_SYSTEM_PREFIX_LEN (rname);
    }
  else
    {
      dest = mempcpy (rname, name, prefix_len);
      *dest++ = '/';
      if (DOUBLE_SLASH_IS_DISTINCT_ROOT)
        {
          if (prefix_len == 0 /* implies ISSLASH (name[0]) */
              && ISSLASH (name[1]) && !ISSLASH (name[2]))
            {
              *dest++ = '/';
#if defined _WIN32 && !defined __CYGWIN__
              /* For UNC file names '\\server\path\to\file', extend the prefix
                 to include the server: '\\server\'.  */
              {
                idx_t i;
                for (i = 2; name[i] != '\0' && !ISSLASH (name[i]); )
                  i++;
                if (name[i] != '\0' /* implies ISSLASH (name[i]) */
                    && i + 1 < rname_buf->length)
                  {
                    prefix_len = i;
                    memcpy (dest, name + 2, i - 2 + 1);
                    dest += i - 2 + 1;
                  }
                else
                  {
                    /* Either name = '\\server'; this is an invalid file name.
                       Or name = '\\server\...' and server is more than
                       rname_buf->length - 4 bytes long.  In either
                       case, stop the UNC processing.  */
                  }
              }
#endif
            }
          *dest = '\0';
        }
      start = name + prefix_len;
    }

  for ( ; *start; start = end)
    {
      /* Skip sequence of multiple file name separators.  */
      while (ISSLASH (*start))
        ++start;

      /* Find end of component.  */
      for (end = start; *end && !ISSLASH (*end); ++end)
        /* Nothing.  */;

      /* Length of this file name component; it can be zero if a file
         name ends in '/'.  */
      idx_t startlen = end - start;

      if (startlen == 1 && start[0] == '.')
        /* nothing */;
      else if (startlen == 2 && start[0] == '.' && start[1] == '.')
        {
          /* Back up to previous component, ignore if at root already.  */
          if (dest > rname + prefix_len + 1)
            for (--dest; dest > rname && !ISSLASH (dest[-1]); --dest)
              continue;
          if (DOUBLE_SLASH_IS_DISTINCT_ROOT
              && dest == rname + 1 && !prefix_len
              && ISSLASH (*dest) && !ISSLASH (dest[1]))
            dest++;
        }
      else
        {
          if (!ISSLASH (dest[-1]))
            *dest++ = '/';

          while (rname + rname_buf->length - dest <= startlen)
            {
              idx_t dest_offset = dest - rname;
              if (!scratch_buffer_grow_preserve (rname_buf))
                xalloc_die ();
              rname = rname_buf->data;
              dest = rname + dest_offset;
            }

          dest = mempcpy (dest, start, startlen);
          *dest = '\0';

          /* If STARTLEN == 0, RNAME ends in '/'; use stat rather than
             readlink, because readlink might fail with EINVAL without
             checking whether RNAME sans '/' is valid.  */
          char discard;
          struct stat st;
          char *buf = NULL;
          ssize_t n;
          if (!logical && startlen != 0)
            {
              while (true)
                {
                  buf = link_buffer.data;
                  idx_t bufsize = link_buffer.length;
                  n = readlink (rname, buf, bufsize - 1);
                  if (n < bufsize - 1)
                    break;
                  if (!scratch_buffer_grow (&link_buffer))
                    xalloc_die ();
                }
              if (n < 0)
                buf = NULL;
            }
          if (buf)
            {
              /* A physical traversal and RNAME is a symbolic link.  */

              if (num_links < 20)
                num_links++;
              else if (*start)
                {
                  /* Enough symlinks have been seen that it is time to
                     worry about being in a symlink cycle.
                     Get the device and inode of the parent directory, as
                     pre-2017 POSIX says this info is not reliable for
                     symlinks.  */
                  dest[- startlen] = '\0';
                  if (stat (*rname ? rname : ".", &st) != 0)
                    goto error;
                  dest[- startlen] = *start;

                  /* Detect loops.  We cannot use the cycle-check module here,
                     since it's possible to encounter the same parent
                     directory more than once in a given traversal.  However,
                     encountering the same (parentdir, START) pair twice does
                     indicate a loop.  */
                  if (seen_triple (&ht, start, &st))
                    {
                      if (can_exist == CAN_MISSING)
                        continue;
                      errno = ELOOP;
                      goto error;
                    }
                }

              buf[n] = '\0';

              char *extra_buf = extra_buffer.data;
              idx_t end_idx;
              if (end_in_extra_buffer)
                end_idx = end - extra_buf;
              idx_t len = strlen (end);
              while (extra_buffer.length <= len + n)
                {
                  if (!scratch_buffer_grow_preserve (&extra_buffer))
                    xalloc_die ();
                  extra_buf = extra_buffer.data;
                }
              if (end_in_extra_buffer)
                end = extra_buf + end_idx;

              /* Careful here, end may be a pointer into extra_buf... */
              memmove (&extra_buf[n], end, len + 1);
              name = end = memcpy (extra_buf, buf, n);
              end_in_extra_buffer = true;

              if (IS_ABSOLUTE_FILE_NAME (buf))
                {
                  idx_t pfxlen = FILE_SYSTEM_PREFIX_LEN (buf);

                  dest = mempcpy (rname, buf, pfxlen);
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
                  if (dest > rname + prefix_len + 1)
                    for (--dest; dest > rname && !ISSLASH (dest[-1]); --dest)
                      continue;
                  if (DOUBLE_SLASH_IS_DISTINCT_ROOT && dest == rname + 1
                      && ISSLASH (*dest) && !ISSLASH (dest[1]) && !prefix_len)
                    dest++;
                }
            }
          else if (! (can_exist == CAN_MISSING
                      || (startlen == 0
                          ? stat (rname, &st) == 0 || errno == EOVERFLOW
                          : ((logical && 0 <= readlink (rname, &discard, 1))
                             || errno == EINVAL))
                      || (can_exist == CAN_ALL_BUT_LAST
                          && errno == ENOENT
                          && !end[strspn (end, SLASHES)])))
            goto error;
        }
    }
  if (dest > rname + prefix_len + 1 && ISSLASH (dest[-1]))
    --dest;
  if (DOUBLE_SLASH_IS_DISTINCT_ROOT && dest == rname + 1 && !prefix_len
      && ISSLASH (*dest) && !ISSLASH (dest[1]))
    dest++;
  failed = false;

error:
  *dest++ = '\0';
  if (ht)
    hash_free (ht);
  scratch_buffer_free (&extra_buffer);
  scratch_buffer_free (&link_buffer);

  if (failed)
    {
      scratch_buffer_free (rname_buf);
      return NULL;
    }

  idx_t rname_size = dest - rname;
  if (rname == rname_on_stack)
    return xmemdup (rname, rname_size);
  char *result = realloc (rname, rname_size);
  return result != NULL ? result : rname;
}

/* Return the canonical absolute name of file NAME, while treating
   missing elements according to CAN_MODE.  A canonical name
   does not contain any ".", ".." components nor any repeated file name
   separators ('/') or, depending on other CAN_MODE flags, symlinks.
   Whether components must exist or not depends on canonicalize mode.
   The result is malloc'd.  */

char *
canonicalize_filename_mode (const char *name, canonicalize_mode_t can_mode)
{
  /* If GCC -Wreturn-local-addr warns about this buffer, the warning
     is bogus; see canonicalize_filename_mode_stk.  */
  struct scratch_buffer rname_buffer;
  return canonicalize_filename_mode_stk (name, can_mode, &rname_buffer);
}
