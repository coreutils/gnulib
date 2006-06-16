/* read-file.c -- read file contents into a string
   Copyright (C) 2006 Free Software Foundation, Inc.
   Written by Simon Josefsson and Bruno Haible.

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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "read-file.h"

/* Get realloc, free. */
#include <stdlib.h>

/* Get errno. */
#include <errno.h>

/* Read a STREAM and return a newly allocated string with the content,
   and set *LENGTH to the length of the string.  The string is
   zero-terminated, but the terminating zero byte is not counted in
   *LENGTH.  On errors, *LENGTH is undefined, errno preserves the
   values set by system functions (if any), and NULL is returned. */
char *
fread_file (FILE * stream, size_t * length)
{
  char *buf = malloc (1);
  size_t alloc = 1;
  size_t size = 0;

  if (!buf)
    return NULL;

  while (!feof (stream))
    {
      size_t count;

      if (size + BUFSIZ + 1 > alloc)
	{
	  char *new_buf;

	  alloc += alloc / 2;
	  if (alloc < size + BUFSIZ + 1)
	    alloc = size + BUFSIZ + 1;

	  new_buf = realloc (buf, alloc);
	  if (!new_buf)
	    {
	      int save_errno = errno;
	      free (buf);
	      errno = save_errno;
	      return NULL;
	    }

	  buf = new_buf;
	}

      count = fread (buf + size, 1, alloc - size - 1, stream);
      size += count;

      if (ferror (stream))
	{
	  int save_errno = errno;
	  free (buf);
	  errno = save_errno;
	  return NULL;
	}
    }

  buf[size] = '\0';

  *length = size;

  return buf;
}

static char *
internal_read_file (const char *filename, size_t * length, const char *mode)
{
  FILE *stream = fopen (filename, mode);
  char *out;
  int save_errno;
  int rc;

  if (!stream)
    return NULL;

  out = fread_file (stream, length);

  save_errno = errno;

  if (fclose (stream) != 0)
    {
      if (out)
	{
	  save_errno = errno;
	  free (out);
	}
      errno = save_errno;
      return NULL;
    }

  return out;
}

/* Open and read the contents of FILENAME, and return a newly
   allocated string with the content, and set *LENGTH to the length of
   the string.  The string is zero-terminated, but the terminating
   zero byte is not counted in *LENGTH.  On errors, *LENGTH is
   undefined, errno preserves the values set by system functions (if
   any), and NULL is returned.  */
char *
read_file (const char *filename, size_t * length)
{
  return internal_read_file (filename, length, "r");
}

/* Open (on non-POSIX systems, in binary mode) and read the contents
   of FILENAME, and return a newly allocated string with the content,
   and set LENGTH to the length of the string.  The string is
   zero-terminated, but the terminating zero byte is not counted in
   the LENGTH variable.  On errors, *LENGTH is undefined, errno
   preserves the values set by system functions (if any), and NULL is
   returned.  */
char *
read_binary_file (const char *filename, size_t * length)
{
  return internal_read_file (filename, length, "rb");
}
