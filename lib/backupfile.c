/* backupfile.c -- make Emacs style backup file names

   Copyright (C) 1990-2006, 2009-2020 Free Software Foundation, Inc.

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

/* Written by Paul Eggert and David MacKenzie.
   Some algorithms adapted from GNU Emacs.  */

#include <config.h>

#include "backup-internal.h"

#include "dirname.h"
#include "opendirat.h"
#include "renameatu.h"
#include "xalloc-oversized.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef FALLTHROUGH
# if __GNUC__ < 7
#  define FALLTHROUGH ((void) 0)
# else
#  define FALLTHROUGH __attribute__ ((__fallthrough__))
# endif
#endif

#ifndef _D_EXACT_NAMLEN
# define _D_EXACT_NAMLEN(dp) strlen ((dp)->d_name)
#endif

#if ! (HAVE_PATHCONF && defined _PC_NAME_MAX)
# define pathconf(file, option) (errno = -1)
#endif

#ifndef _POSIX_NAME_MAX
# define _POSIX_NAME_MAX 14
#endif

#if defined _XOPEN_NAME_MAX
# define NAME_MAX_MINIMUM _XOPEN_NAME_MAX
#else
# define NAME_MAX_MINIMUM _POSIX_NAME_MAX
#endif

#ifndef HAVE_DOS_FILE_NAMES
# define HAVE_DOS_FILE_NAMES 0
#endif
#ifndef HAVE_LONG_FILE_NAMES
# define HAVE_LONG_FILE_NAMES 0
#endif

/* ISDIGIT differs from isdigit, as follows:
   - Its arg may be any int or unsigned int; it need not be an unsigned char
     or EOF.
   - It's typically faster.
   POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
   ISDIGIT unless it's important to use the locale's definition
   of "digit" even when the host does not conform to POSIX.  */
#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

/* The extension added to file names to produce a simple (as opposed
   to numbered) backup file name. */
char const *simple_backup_suffix = NULL;

/* Set SIMPLE_BACKUP_SUFFIX to S, or to a default specified by the
   environment if S is null.  If S or the environment does not specify
   a valid backup suffix, use "~".  */
void
set_simple_backup_suffix (char const *s)
{
  if (!s)
    s = getenv ("SIMPLE_BACKUP_SUFFIX");
  simple_backup_suffix = s && *s && s == last_component (s) ? s : "~";
}

/* If FILE (which was of length FILELEN before an extension was
   appended to it) is too long, replace the extension with the single
   char E.  If the result is still too long, remove the char just
   before E.

   If DIR_FD is nonnegative, it is a file descriptor for FILE's parent.
   *NAME_MAX is either 0, or the cached result of a previous call for
   FILE's parent's _PC_NAME_MAX.  */

static void
check_extension (char *file, size_t filelen, char e,
                 int dir_fd, size_t *base_max)
{
  char *base = last_component (file);
  size_t baselen = base_len (base);
  size_t baselen_max = HAVE_LONG_FILE_NAMES ? 255 : NAME_MAX_MINIMUM;

  if (HAVE_DOS_FILE_NAMES || NAME_MAX_MINIMUM < baselen)
    {
      /* The new base name is long enough to require a pathconf check.  */
      if (*base_max == 0)
        {
          long name_max;
          if (dir_fd < 0)
            {
              /* Temporarily modify the buffer into its parent
                 directory name, invoke pathconf on the directory, and
                 then restore the buffer.  */
              char tmp[sizeof "."];
              memcpy (tmp, base, sizeof ".");
              strcpy (base, ".");
              errno = 0;
              name_max = pathconf (file, _PC_NAME_MAX);
              name_max -= !errno;
              memcpy (base, tmp, sizeof ".");
            }
          else
            {
              errno = 0;
              name_max = fpathconf (dir_fd, _PC_NAME_MAX);
              name_max -= !errno;
            }

          *base_max = (0 <= name_max && name_max <= SIZE_MAX ? name_max
                       : name_max < -1 ? NAME_MAX_MINIMUM : SIZE_MAX);
        }

      baselen_max = *base_max;
    }

  if (HAVE_DOS_FILE_NAMES && baselen_max <= 12)
    {
      /* Live within DOS's 8.3 limit.  */
      char *dot = strchr (base, '.');
      if (!dot)
        baselen_max = 8;
      else
        {
          char const *second_dot = strchr (dot + 1, '.');
          baselen_max = (second_dot
                         ? second_dot - base
                         : dot + 1 - base + 3);
        }
    }

  if (baselen_max < baselen)
    {
      baselen = file + filelen - base;
      if (baselen_max <= baselen)
        baselen = baselen_max - 1;
      base[baselen] = e;
      base[baselen + 1] = '\0';
    }
}

/* Returned values for NUMBERED_BACKUP.  */

enum numbered_backup_result
  {
    /* The new backup name is the same length as an existing backup
       name, so it's valid for that directory.  */
    BACKUP_IS_SAME_LENGTH,

    /* Some backup names already exist, but the returned name is longer
       than any of them, and its length should be checked.  */
    BACKUP_IS_LONGER,

    /* There are no existing backup names.  The new name's length
       should be checked.  */
    BACKUP_IS_NEW,

    /* Memory allocation failure.  */
    BACKUP_NOMEM
  };

/* Relative to DIR_FD, *BUFFER contains a file name.
   Store into *BUFFER the next backup name for the named file,
   with a version number greater than all the
   existing numbered backups.  Reallocate *BUFFER as necessary; its
   initial allocated size is BUFFER_SIZE, which must be at least 4
   bytes longer than the file name to make room for the initially
   appended ".~1".  FILELEN is the length of the original file name.
   BASE_OFFSET is the offset of the basename in *BUFFER.
   The returned value indicates what kind of backup was found.  If an
   I/O or other read error occurs, use the highest backup number that
   was found.

   *DIRPP is the destination directory.  If *DIRPP is null, open the
   destination directory and store the resulting stream into *DIRPP
   and its file descriptor into *PNEW_FD without closing the stream.  */

static enum numbered_backup_result
numbered_backup (int dir_fd, char **buffer, size_t buffer_size, size_t filelen,
                 ptrdiff_t base_offset, DIR **dirpp, int *pnew_fd)
{
  enum numbered_backup_result result = BACKUP_IS_NEW;
  DIR *dirp = *dirpp;
  struct dirent *dp;
  char *buf = *buffer;
  size_t versionlenmax = 1;
  char *base = buf + base_offset;
  size_t baselen = base_len (base);

  if (dirp)
    rewinddir (dirp);
  else
    {
      /* Temporarily modify the buffer into its parent directory name,
         open the directory, and then restore the buffer.  */
      char tmp[sizeof "."];
      memcpy (tmp, base, sizeof ".");
      strcpy (base, ".");
      dirp = opendirat (dir_fd, buf, 0, pnew_fd);
      if (!dirp && errno == ENOMEM)
        result = BACKUP_NOMEM;
      memcpy (base, tmp, sizeof ".");
      strcpy (base + baselen, ".~1~");
      if (!dirp)
        return result;
      *dirpp = dirp;
    }

  while ((dp = readdir (dirp)) != NULL)
    {
      char const *p;
      char *q;
      bool all_9s;
      size_t versionlen;

      if (_D_EXACT_NAMLEN (dp) < baselen + 4)
        continue;

      if (memcmp (buf + base_offset, dp->d_name, baselen + 2) != 0)
        continue;

      p = dp->d_name + baselen + 2;

      /* Check whether this file has a version number and if so,
         whether it is larger.  Use string operations rather than
         integer arithmetic, to avoid problems with integer overflow.  */

      if (! ('1' <= *p && *p <= '9'))
        continue;
      all_9s = (*p == '9');
      for (versionlen = 1; ISDIGIT (p[versionlen]); versionlen++)
        all_9s &= (p[versionlen] == '9');

      if (! (p[versionlen] == '~' && !p[versionlen + 1]
             && (versionlenmax < versionlen
                 || (versionlenmax == versionlen
                     && memcmp (buf + filelen + 2, p, versionlen) <= 0))))
        continue;

      /* This entry has the largest version number seen so far.
         Append this highest numbered extension to the file name,
         prepending '0' to the number if it is all 9s.  */

      versionlenmax = all_9s + versionlen;
      result = (all_9s ? BACKUP_IS_LONGER : BACKUP_IS_SAME_LENGTH);
      size_t new_buffer_size = filelen + 2 + versionlenmax + 2;
      if (buffer_size < new_buffer_size)
        {
          if (! xalloc_oversized (new_buffer_size, 2))
            new_buffer_size *= 2;
          char *new_buf = realloc (buf, new_buffer_size);
          if (!new_buf)
            {
              *buffer = buf;
              return BACKUP_NOMEM;
            }
          buf = new_buf;
          buffer_size = new_buffer_size;
        }
      q = buf + filelen;
      *q++ = '.';
      *q++ = '~';
      *q = '0';
      q += all_9s;
      memcpy (q, p, versionlen + 2);

      /* Add 1 to the version number.  */

      q += versionlen;
      while (*--q == '9')
        *q = '0';
      ++*q;
    }

  *buffer = buf;
  return result;
}

/* Relative to DIR_FD, return the name of the new backup file for the
   existing file FILE, allocated with malloc.
   If RENAME, also rename FILE to the new name.
   On failure, return NULL and set errno.
   Do not call this function if backup_type == no_backups.  */

char *
backupfile_internal (int dir_fd, char const *file,
                     enum backup_type backup_type, bool rename)
{
  ptrdiff_t base_offset = last_component (file) - file;
  size_t filelen = base_offset + strlen (file + base_offset);

  if (! simple_backup_suffix)
    set_simple_backup_suffix (NULL);

  /* Allow room for simple or ".~N~" backups.  The guess must be at
     least sizeof ".~1~", but otherwise will be adjusted as needed.  */
  size_t simple_backup_suffix_size = strlen (simple_backup_suffix) + 1;
  size_t backup_suffix_size_guess = simple_backup_suffix_size;
  enum { GUESS = sizeof ".~12345~" };
  if (backup_suffix_size_guess < GUESS)
    backup_suffix_size_guess = GUESS;

  ssize_t ssize = filelen + backup_suffix_size_guess + 1;
  char *s = malloc (ssize);
  if (!s)
    return s;

  DIR *dirp = NULL;
  int sdir = -1;
  size_t base_max = 0;
  while (true)
    {
      memcpy (s, file, filelen + 1);

      if (backup_type == simple_backups)
        memcpy (s + filelen, simple_backup_suffix, simple_backup_suffix_size);
      else
        switch (numbered_backup (dir_fd, &s, ssize, filelen, base_offset,
                                 &dirp, &sdir))
          {
          case BACKUP_IS_SAME_LENGTH:
            break;

          case BACKUP_IS_NEW:
            if (backup_type == numbered_existing_backups)
              {
                backup_type = simple_backups;
                memcpy (s + filelen, simple_backup_suffix,
                        simple_backup_suffix_size);
              }
            FALLTHROUGH;
          case BACKUP_IS_LONGER:
            check_extension (s, filelen, '~', sdir, &base_max);
            break;

          case BACKUP_NOMEM:
            if (dirp)
              closedir (dirp);
            free (s);
            errno = ENOMEM;
            return NULL;
          }

      if (! rename)
        break;

      if (sdir < 0)
        {
          sdir = AT_FDCWD;
          base_offset = 0;
        }
      unsigned flags = backup_type == simple_backups ? 0 : RENAME_NOREPLACE;
      if (renameatu (AT_FDCWD, file, sdir, s + base_offset, flags) == 0)
        break;
      int e = errno;
      if (e != EEXIST)
        {
          if (dirp)
            closedir (dirp);
          free (s);
          errno = e;
          return NULL;
        }
    }

  if (dirp)
    closedir (dirp);
  return s;
}
