/* Locale dependent string transformation for comparison.
   Copyright (C) 2010-2022 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef ASTRXFRM_H
#define ASTRXFRM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Variant of strxfrm() with a calling convention that reduces the number
   of strxfrm calls.  */

/* Transform the string starting at S to a string, in such a way that
   comparing S1 and S2 with strcoll() is equivalent to comparing astrxfrm(S1)
   and astrxfrm(S2) with strcmp().
   The result of this function depends on the LC_COLLATE category of the
   current locale.
   If successful: If resultbuf is not NULL and the result fits into *lengthp
   bytes, it is put in resultbuf, and resultbuf is returned.  Otherwise, a
   freshly allocated string is returned.  In both cases, *lengthp is set to the
   length of the returned string.
   Upon failure, return NULL, with errno set.  */
extern char * astrxfrm (const char *s,
                        char *restrict resultbuf, size_t *lengthp);


#ifdef __cplusplus
}
#endif

#endif /* ASTRXFRM_H */
