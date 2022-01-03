/* vasprintf and asprintf, with out-of-memory checking, in C locale.
   Copyright (C) 2002-2004, 2006-2022 Free Software Foundation, Inc.

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

#ifndef _C_XVASPRINTF_H
#define _C_XVASPRINTF_H

/* Get va_list.  */
#include <stdarg.h>

/* Get _GL_ATTRIBUTE_SPEC_PRINTF_STANDARD.  */
#include <stdio.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Write formatted output to a string dynamically allocated with malloc(),
   and return it.  Upon [ENOMEM] memory allocation error, call xalloc_die.
   On some other error
     - [EOVERFLOW] resulting string length is > INT_MAX,
     - [EINVAL] invalid format string,
     - [EILSEQ] error during conversion between wide and multibyte characters,
   return NULL.

   Formatting takes place in the C locale, that is, the decimal point
   used in floating-point formatting directives is always '.'. */
extern char *c_xasprintf (const char *format, ...)
  _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 1, 2))
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE;
extern char *c_xvasprintf (const char *format, va_list args)
  _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 1, 0))
  _GL_ATTRIBUTE_MALLOC _GL_ATTRIBUTE_DEALLOC_FREE;

#ifdef __cplusplus
}
#endif

#endif /* _C_XVASPRINTF_H */
