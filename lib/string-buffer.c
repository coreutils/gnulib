/* A buffer that accumulates a string by piecewise concatenation.
   Copyright (C) 2021-2024 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2021.  */

#include <config.h>

/* Specification.  */
#include "string-buffer.h"

/* Undocumented.  */
extern int sb_ensure_more_bytes (struct string_buffer *buffer,
                                 size_t increment);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The warnings about memory resource 'buffer->data' in this file are not
   relevant.  Silence them.  */
#if __clang_major__ >= 3
# pragma clang diagnostic ignored "-Wthread-safety"
#endif

void
sb_init (struct string_buffer *buffer)
{
  buffer->data = buffer->space;
  buffer->length = 0;
  buffer->allocated = sizeof (buffer->space);
  buffer->error = false;
}

/* Ensures that INCREMENT bytes are available beyond the current used length
   of BUFFER.
   Returns 0, or -1 in case of out-of-memory error.  */
int
sb_ensure_more_bytes (struct string_buffer *buffer, size_t increment)
{
  size_t incremented_length = buffer->length + increment;
  if (incremented_length < increment)
    /* Overflow.  */
    return -1;

  if (buffer->allocated < incremented_length)
    {
      size_t new_allocated = 2 * buffer->allocated;
      if (new_allocated < buffer->allocated)
        /* Overflow.  */
        return -1;
      if (new_allocated < incremented_length)
        new_allocated = incremented_length;

      char *new_data;
      if (buffer->data == buffer->space)
        {
          new_data = (char *) malloc (new_allocated);
          if (new_data == NULL)
            /* Out-of-memory.  */
            return -1;
          memcpy (new_data, buffer->data, buffer->length);
        }
      else
        {
          new_data = (char *) realloc (buffer->data, new_allocated);
          if (new_data == NULL)
            /* Out-of-memory.  */
            return -1;
        }
      buffer->data = new_data;
      buffer->allocated = new_allocated;
    }
  return 0;
}

int
sb_append (struct string_buffer *buffer, const char *str)
{
  size_t len = strlen (str);
  if (sb_ensure_more_bytes (buffer, len) < 0)
    {
      buffer->error = true;
      return -1;
    }
  memcpy (buffer->data + buffer->length, str, len);
  buffer->length += len;
  return 0;
}

void
sb_free (struct string_buffer *buffer)
{
  if (buffer->data != buffer->space)
    free (buffer->data);
}

/* Returns the contents of BUFFER, and frees all other memory held
   by BUFFER.  Returns NULL upon failure or if there was an error earlier.  */
char *
sb_dupfree (struct string_buffer *buffer)
{
  if (buffer->error)
    goto fail;

  if (sb_ensure_more_bytes (buffer, 1) < 0)
    goto fail;
  buffer->data[buffer->length] = '\0';
  buffer->length++;

  if (buffer->data == buffer->space)
    {
      char *copy = (char *) malloc (buffer->length);
      if (copy == NULL)
        goto fail;
      memcpy (copy, buffer->data, buffer->length);
      return copy;
    }
  else
    {
      /* Shrink the string before returning it.  */
      char *contents = buffer->data;
      if (buffer->length < buffer->allocated)
        {
          contents = realloc (contents, buffer->length);
          if (contents == NULL)
            goto fail;
        }
      return contents;
    }

 fail:
  sb_free (buffer);
  return NULL;
}
