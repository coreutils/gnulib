/* lt__argz.h -- internal argz interface for non-glibc systems
   Copyright (C) 2004 Free Software Foundation, Inc.
   Originally by Gary V. Vaughan  <gary@gnu.org>

   NOTE: The canonical source of this file is maintained with the
   GNU Libtool package.  Report bugs to bug-libtool@gnu.org.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#if !defined(LT__ARGZ_H)
#define LT__ARGZ_H 1

#include <stdlib.h>
#include <sys/types.h>

#if defined(LTDL)
#  include "lt__glibc.h"
#endif

#if defined(_cplusplus)
extern "C" {
#endif

error_t	argz_append	(char **pargz, size_t *pargz_len,
			 const char *buf, size_t buf_len);
error_t	argz_create_sep	(const char *str, int delim,
			 char **pargz, size_t *pargz_len);
error_t	argz_insert	(char **pargz, size_t *pargz_len,
			 char *before, const char *entry);
char *	argz_next	(char *argz, size_t argz_len, const char *entry);
void	argz_stringify	(char *argz, size_t argz_len, int sep);

#if defined(_cplusplus)
}
#endif

#endif /*!defined(LT__ARGZ_H)*/
