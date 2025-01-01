/* vasprintf and asprintf, in C locale.
   Copyright (C) 2002-2004, 2006-2025 Free Software Foundation, Inc.

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

#ifndef _C_VASPRINTF_H
#define _C_VASPRINTF_H

/* This file uses _GL_ATTRIBUTE_FORMAT, _GL_ATTRIBUTE_NODISCARD.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

/* Get va_list.  */
#include <stdarg.h>

/* Get ptrdiff_t.  */
#include <stddef.h>

/* Get _GL_ATTRIBUTE_SPEC_PRINTF_STANDARD.  */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Prints formatted output to a string dynamically allocated with malloc().
   If the memory allocation succeeds, it stores the address of the string in
   *RESULT and returns the number of resulting bytes, excluding the trailing
   NUL.  Upon memory allocation error, or some other error, it returns -1
   with errno set.

   Failure code EOVERFLOW can only occur when a width > INT_MAX is used.
   Therefore, if the format string is valid and does not use %ls/%lc
   directives nor widths, the only possible failure code is ENOMEM.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
_GL_ATTRIBUTE_NODISCARD
ptrdiff_t c_aszprintf (char **resultp, const char *format, ...)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 3));
_GL_ATTRIBUTE_NODISCARD
ptrdiff_t c_vaszprintf (char **resultp, const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 0));

/* Prints formatted output to a string dynamically allocated with malloc().
   If the memory allocation succeeds, it stores the address of the string in
   *RESULT and returns the number of resulting bytes, excluding the trailing
   NUL.  Upon memory allocation error, or some other error, it returns -1.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
_GL_ATTRIBUTE_NODISCARD
int c_asprintf (char **resultp, const char *format, ...)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 3));
_GL_ATTRIBUTE_NODISCARD
int c_vasprintf (char **resultp, const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 0));

#ifdef __cplusplus
}
#endif

#endif /* _C_VASPRINTF_H */
