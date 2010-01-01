/* Locale dependent memory area transformation for comparison.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef MEMXFRM_H
#define MEMXFRM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Generalization of strxfrm() to strings with embedded NUL bytes.  */

/* Transform the memory area [S..S+N-1] to a memory area, in such a way that
   comparing (S1,N1) and (S2,N2) with memcoll() is equivalent to comparing
   memxfrm(S1,N1) and memxfrm(S2,N2) with memcmp2().
   The byte S[N] may be temporarily overwritten by this function, but will be
   restored before this function returns.
   The result of this function depends on the LC_COLLATE category of the
   current locale.
   If successful: If resultbuf is not NULL and the result fits into *lengthp
   bytes, it is put in resultbuf, and resultbuf is returned.  Otherwise, a
   freshly allocated string is returned.  In both cases, *lengthp is set to the
   length of the returned string.
   Upon failure, return NULL, with errno set.  */
extern char * memxfrm (char *s, size_t n, char *resultbuf, size_t *lengthp);


#ifdef __cplusplus
}
#endif

#endif /* MEMXFRM_H */
