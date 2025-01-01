/* vsnprintf in C locale.
   Copyright (C) 2012-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _C_VSNPRINTF_H
#define _C_VSNPRINTF_H

/* This file uses _GL_ATTRIBUTE_FORMAT.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

/* Get size_t, ptrdiff_t.  */
#include <stddef.h>

/* Get va_list. */
#include <stdarg.h>

/* Get _GL_ATTRIBUTE_SPEC_PRINTF_STANDARD.  */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Prints formatted output to string STR.  Similar to vsprintf, but the
   additional parameter SIZE limits how much is written into STR.
   STR may be NULL, in which case nothing will be written.
   Returns the string length of the formatted string (which may be larger
   than SIZE).  Upon failure, returns -1 with errno set.

   Failure code EOVERFLOW can only occur when a width > INT_MAX is used.
   Therefore, if the format string is valid and does not use %ls/%lc
   directives nor widths, the only possible failure code is ENOMEM.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
extern ptrdiff_t c_vsnzprintf (char *restrict str, size_t size,
                               const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 3, 0));

/* Prints formatted output to string STR.  Similar to sprintf, but the
   additional parameter SIZE limits how much is written into STR.
   STR may be NULL, in which case nothing will be written.
   Returns the string length of the formatted string (which may be larger
   than SIZE).  Upon failure, returns -1 with errno set.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
extern int c_vsnprintf (char *restrict str, size_t size,
                        const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 3, 0));

#ifdef __cplusplus
}
#endif

#endif /* _C_VSNPRINTF_H */
