/* A buffer that accumulates a string by piecewise concatenation.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.

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

#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "attribute.h"

/* A string buffer type.  */
struct string_buffer
{
  char *data;
  size_t length;     /* used bytes, <= allocated */
  size_t allocated;  /* allocated bytes */
  bool error;        /* true if there was an error */
  char space[1024];  /* stack allocated space */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes BUFFER to the empty string.  */
extern void sb_init (struct string_buffer *buffer);

/* Appends the contents of STR to BUFFER.
   Returns 0, or -1 in case of out-of-memory error.  */
extern int sb_append (struct string_buffer *buffer, const char *str);

/* Appends the result of the printf-compatible FORMATSTRING with the argument
   list LIST to BUFFER.
   Returns 0, or -1 in case of error.  */
extern int sb_appendvf (struct string_buffer *buffer,
                        const char *formatstring, va_list list)
  #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
  ATTRIBUTE_FORMAT ((__gnu_printf__, 2, 0))
  #else
  ATTRIBUTE_FORMAT ((__printf__, 2, 0))
  #endif
  ;

/* Appends the result of the printf-compatible FORMATSTRING with the following
   arguments to BUFFER.
   Returns 0, or -1 in case of error.  */
extern int sb_appendf (struct string_buffer *buffer,
                        const char *formatstring, ...)
  #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
  ATTRIBUTE_FORMAT ((__gnu_printf__, 2, 3))
  #else
  ATTRIBUTE_FORMAT ((__printf__, 2, 3))
  #endif
  ;

/* Frees the memory held by BUFFER.  */
extern void sb_free (struct string_buffer *buffer);

/* Returns the contents of BUFFER, and frees all other memory held
   by BUFFER.  Returns NULL upon failure or if there was an error earlier.
   It is the responsibility of the caller to free() the result.  */
extern char * sb_dupfree (struct string_buffer *buffer)
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE;

#ifdef __cplusplus
}
#endif

#endif /* _STRING_BUFFER_H */
