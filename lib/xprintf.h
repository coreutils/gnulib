/* printf and fprintf wrappers that fail immediately for non-file-related errors
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _XPRINTF_H
#define _XPRINTF_H

/* This file uses _GL_ATTRIBUTE_FORMAT.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Like zprintf, but call error if it fails without setting stdout's
   error indicator, i.e. upon ENOMEM, EOVERFLOW, or EILSEQ errors.  */
extern off64_t xprintf (char const *restrict format, ...)
#if GNULIB_VPRINTF_POSIX
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 1, 2))
#else
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_SYSTEM, 1, 2))
#endif
     ;

/* Like vzprintf, but call error if it fails without setting stdout's
   error indicator, i.e. upon ENOMEM, EOVERFLOW, or EILSEQ errors.  */
extern off64_t xvprintf (char const *restrict format, va_list args)
#if GNULIB_VPRINTF_POSIX
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 1, 0))
#else
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_SYSTEM, 1, 0))
#endif
     ;

/* Like fzprintf, but call error if it fails without setting the stream's
   error indicator, i.e. upon ENOMEM, EOVERFLOW, or EILSEQ errors.  */
extern off64_t xfprintf (FILE *restrict stream, char const *restrict format,
                         ...)
#if GNULIB_VFPRINTF_POSIX
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 3))
#else
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_SYSTEM, 2, 3))
#endif
     ;

/* Like vfzprintf, but call error if it fails without setting the stream's
   error indicator, i.e. upon ENOMEM, EOVERFLOW, or EILSEQ errors.  */
extern off64_t xvfprintf (FILE *restrict stream, char const *restrict format,
                          va_list args)
#if GNULIB_VFPRINTF_POSIX
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 0))
#else
     _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_SYSTEM, 2, 0))
#endif
     ;


#ifdef __cplusplus
}
#endif

#endif
