/* A GNU-like <stdio.h>.

   Copyright (C) 2004, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#if defined __need_FILE
/* Special invocation convention inside glibc header files.  */

#include @ABSOLUTE_STDIO_H@

#else
/* Normal invocation convention.  */
#ifndef _GL_STDIO_H
#define _GL_STDIO_H

#include @ABSOLUTE_STDIO_H@

#include <stdarg.h>
#include <stddef.h>


/* The definition of GL_LINK_WARNING is copied here.  */


#ifdef __cplusplus
extern "C" {
#endif


#if @GNULIB_SNPRINTF@
# if !@HAVE_DECL_SNPRINTF@
extern int snprintf (char *str, size_t size, const char *format, ...);
# endif
#elif defined GNULIB_POSIXCHECK
# undef snprintf
# define snprintf \
    (GL_LINK_WARNING ("snprintf is unportable - " \
                      "use gnulib module snprintf for portability"), \
     snprintf)
#endif

#if @GNULIB_VSNPRINTF@
# if @REPLACE_VSNPRINTF@
#  define vsnprintf rpl_vsnprintf
# endif
# if @REPLACE_VSNPRINTF@ || !@HAVE_DECL_VSNPRINTF@
extern int vsnprintf (char *str, size_t size, const char *format, va_list args);
# endif
#elif defined GNULIB_POSIXCHECK
# undef vsnprintf
# define vsnprintf(b,s,f,a) \
    (GL_LINK_WARNING ("vsnprintf is unportable - " \
                      "use gnulib module vsnprintf for portability"), \
     vsnprintf (b, s, f, a))
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_STDIO_H */
#endif
