/* vasprintf and asprintf, in C locale.
   Copyright (C) 2002-2004, 2006-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef _C_VASPRINTF_H
#define _C_VASPRINTF_H

/* Get va_list.  */
#include <stdarg.h>

/* The __attribute__ feature is available in gcc versions 2.5 and later.
   The __-protected variants of the attributes 'format' and 'printf' are
   accepted by gcc versions 2.6.4 (effectively 2.7) and later.
   We enable _GL_ATTRIBUTE_FORMAT only if these are supported too, because
   gnulib and libintl do '#define printf __printf__' when they override
   the 'printf' function.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_ATTRIBUTE_FORMAT(spec) __attribute__ ((__format__ spec))
#else
# define _GL_ATTRIBUTE_FORMAT(spec) /* empty */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* asprintf() and vasprintf(), but formatting takes place in the C locale, that
   is, the decimal point used in floating-point formatting directives is always
   '.'. */
int c_asprintf (char **resultp, const char *format, ...)
       _GL_ATTRIBUTE_FORMAT ((__printf__, 2, 3));
int c_vasprintf (char **resultp, const char *format, va_list args)
       _GL_ATTRIBUTE_FORMAT ((__printf__, 2, 0));

#ifdef __cplusplus
}
#endif

#endif /* _C_VASPRINTF_H */
