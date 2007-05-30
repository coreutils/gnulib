/* A GNU-like <iconv.h>.

   Copyright (C) 2007 Free Software Foundation, Inc.

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

#ifndef _GL_ICONV_H

/* The include_next requires a split double-inclusion guard.  */
#if @HAVE_INCLUDE_NEXT@
# include_next <iconv.h>
#else
# include @ABSOLUTE_ICONV_H@
#endif

#ifndef _GL_ICONV_H
#define _GL_ICONV_H

#ifdef __cplusplus
extern "C" {
#endif


#if @REPLACE_ICONV_OPEN@
/* An iconv_open wrapper that supports the IANA standardized encoding names
   ("ISO-8859-1" etc.) as far as possible.  */
# define iconv_open rpl_iconv_open
extern iconv_t iconv_open (const char *tocode, const char *fromcode);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GL_ICONV_H */
#endif /* _GL_ICONV_H */
