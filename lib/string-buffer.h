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

#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H

/* This file uses _GL_ATTRIBUTE_MALLOC, _GL_ATTRIBUTE_CAPABILITY_TYPE,
   _GL_ATTRIBUTE_ACQUIRE_CAPABILITY, _GL_ATTRIBUTE_RELEASE_CAPABILITY.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdarg.h>
#include <stdlib.h>

#include "attribute.h"

typedef char * _GL_ATTRIBUTE_CAPABILITY_TYPE ("memory resource")
        sb_heap_allocated_pointer_t;

/* A string buffer type.  */
struct string_buffer
{
  sb_heap_allocated_pointer_t data;
  size_t length;     /* used bytes, <= allocated */
  size_t allocated;  /* allocated bytes */
  bool error;        /* true if there was an error */
  char space[1024];  /* stack allocated space */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes BUFFER to the empty string.  */
extern void sb_init (struct string_buffer *buffer)
  _GL_ATTRIBUTE_ACQUIRE_CAPABILITY (buffer->data);

/* Appends the contents of STR to BUFFER.
   Returns 0, or -1 in case of out-of-memory error.  */
extern int sb_append (struct string_buffer *buffer, const char *str);

/* Appends the result of the printf-compatible FORMATSTRING with the argument
   list LIST to BUFFER.
   Returns 0, or -1 with errno set in case of error.
   Error code EOVERFLOW can only occur when a width > INT_MAX is used.
   Therefore, if the format string is valid and does not use %ls/%lc
   directives nor widths, the only possible error code is ENOMEM.  */
extern int sb_appendvf (struct string_buffer *buffer,
                        const char *formatstring, va_list list)
  #if (__GNUC__ + (__GNUC_MINOR__ >= 4) > 4) && !defined __clang__
  ATTRIBUTE_FORMAT ((__gnu_printf__, 2, 0))
  #else
  ATTRIBUTE_FORMAT ((__printf__, 2, 0))
  #endif
  ;

/* Appends the result of the printf-compatible FORMATSTRING with the following
   arguments to BUFFER.
   Returns 0, or -1 with errno set in case of error.
   Error code EOVERFLOW can only occur when a width > INT_MAX is used.
   Therefore, if the format string is valid and does not use %ls/%lc
   directives nor widths, the only possible error code is ENOMEM.  */
extern int sb_appendf (struct string_buffer *buffer,
                        const char *formatstring, ...)
  #if (__GNUC__ + (__GNUC_MINOR__ >= 4) > 4) && !defined __clang__
  ATTRIBUTE_FORMAT ((__gnu_printf__, 2, 3))
  #else
  ATTRIBUTE_FORMAT ((__printf__, 2, 3))
  #endif
  ;

/* Frees the memory held by BUFFER.  */
extern void sb_free (struct string_buffer *buffer)
  _GL_ATTRIBUTE_RELEASE_CAPABILITY (buffer->data);

/* Returns the contents of BUFFER, and frees all other memory held
   by BUFFER.  Returns NULL upon failure or if there was an error earlier.
   It is the responsibility of the caller to free() the result.  */
extern char * sb_dupfree (struct string_buffer *buffer)
  _GL_ATTRIBUTE_RELEASE_CAPABILITY (buffer->data);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_BUFFER_H */
