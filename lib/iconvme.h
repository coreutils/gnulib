/* Recode strings between character sets, using iconv.
   Copyright (C) 2004 Free Software Foundation, Inc.
   Written by Simon Josefsson.

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

#ifndef ICONVME_H
# define ICONVME_H

extern char *iconv_string (const char *string, const char *from_code,
			   const char *to_code);

#if HAVE_ICONV
# include <iconv.h>
extern char *iconv_alloc (iconv_t cd, const char *string);
#endif

#endif /* ICONVME_H */
