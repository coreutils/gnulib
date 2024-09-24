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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

int
sb_appendvf (struct string_buffer *buffer, const char *formatstring,
             va_list list)
{
  va_list list_copy;

  /* Make a bit of room, so that the probability that the first vsnprintf() call
     succeeds is high.  */
  size_t room = buffer->allocated - buffer->length;
  if (room < 64)
    {
      if (sb_ensure_more_bytes (buffer, 64) < 0)
        {
          buffer->error = true;
          return -1;
        }
      room = buffer->allocated - buffer->length;
    }

  va_copy (list_copy, list);

  /* First vsnprintf() call.  */
  int ret = vsnprintf (buffer->data + buffer->length, room, formatstring, list);
  if (ret < 0)
    {
      /* Failed.  */
      buffer->error = true;
      ret = -1;
    }
  else
    {
      if ((size_t) ret <= room)
        {
          /* The result has fit into room bytes.  */
          buffer->length += (size_t) ret;
          ret = 0;
        }
      else
        {
          /* The result was truncated.  Make more room, for a second vsnprintf()
             call.  */
          if (sb_ensure_more_bytes (buffer, (size_t) ret) < 0)
            {
              buffer->error = true;
              ret = -1;
            }
          else
            {
              /* Second vsnprintf() call.  */
              room = buffer->allocated - buffer->length;
              ret = vsnprintf (buffer->data + buffer->length, room,
                               formatstring, list_copy);
              if (ret < 0)
                {
                  /* Failed.  */
                  buffer->error = true;
                  ret = -1;
                }
              else
                {
                  if ((size_t) ret <= room)
                    {
                      /* The result has fit into room bytes.  */
                      buffer->length += (size_t) ret;
                      ret = 0;
                    }
                  else
                    /* The return values of the vsnprintf() calls are not
                       consistent.  */
                    abort ();
                }
            }
        }
    }

  va_end (list_copy);
  return ret;
}

int
sb_appendf (struct string_buffer *buffer, const char *formatstring, ...)
{
  va_list args;

  /* Make a bit of room, so that the probability that the first vsnprintf() call
     succeeds is high.  */
  size_t room = buffer->allocated - buffer->length;
  if (room < 64)
    {
      if (sb_ensure_more_bytes (buffer, 64) < 0)
        {
          buffer->error = true;
          return -1;
        }
      room = buffer->allocated - buffer->length;
    }

  va_start (args, formatstring);

  /* First vsnprintf() call.  */
  int ret = vsnprintf (buffer->data + buffer->length, room, formatstring, args);
  if (ret < 0)
    {
      /* Failed.  */
      buffer->error = true;
      ret = -1;
    }
  else
    {
      if ((size_t) ret <= room)
        {
          /* The result has fit into room bytes.  */
          buffer->length += (size_t) ret;
          ret = 0;
        }
      else
        {
          /* The result was truncated.  Make more room, for a second vsnprintf()
             call.  */
          if (sb_ensure_more_bytes (buffer, (size_t) ret) < 0)
            {
              buffer->error = true;
              ret = -1;
            }
          else
            {
              /* Second vsnprintf() call.  */
              room = buffer->allocated - buffer->length;
              va_end (args);
              va_start (args, formatstring);
              ret = vsnprintf (buffer->data + buffer->length, room,
                               formatstring, args);
              if (ret < 0)
                {
                  /* Failed.  */
                  buffer->error = true;
                  ret = -1;
                }
              else
                {
                  if ((size_t) ret <= room)
                    {
                      /* The result has fit into room bytes.  */
                      buffer->length += (size_t) ret;
                      ret = 0;
                    }
                  else
                    /* The return values of the vsnprintf() calls are not
                       consistent.  */
                    abort ();
                }
            }
        }
    }

  va_end (args);
  return ret;
}
