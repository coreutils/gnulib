/* vasprintf and asprintf, in C locale.
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

#ifndef _C_VASPRINTF_H
#define _C_VASPRINTF_H

/* Get va_list.  */
#include <stdarg.h>

/* Get _GL_ATTRIBUTE_SPEC_PRINTF_STANDARD.  */
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* asprintf() and vasprintf(), but formatting takes place in the C locale, that
   is, the decimal point used in floating-point formatting directives is always
   '.'. */
int c_asprintf (char **resultp, const char *format, ...)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 3));
int c_vasprintf (char **resultp, const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((_GL_ATTRIBUTE_SPEC_PRINTF_STANDARD, 2, 0));

#ifdef __cplusplus
}
#endif

#endif /* _C_VASPRINTF_H */
