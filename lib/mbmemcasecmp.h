/* Compare two memory areas with possibly different lengths, case-insensitive.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.

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

#ifndef MBMEMCASECMP_H
#define MBMEMCASECMP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Compare the memory regions S1 = [s1..s1+N1-1], S2 = [s2..s2+n2-1], that
   contain character sequences, lexicographically, ignoring case.
   This function's result is locale dependent.  Unlike memcasecmp(), it works
   correctly in multibyte locales and also handles Turkish i / dotless i.
   Unlike ulc_casecmp(), it does not handle the German sharp s and the Greek
   final sigma.  Unlike memcoll() and ulc_casecoll(), it ignores collation
   order.
   Return a negative number if S1 < S2, a positive number if S1 > S2, or
   0 if S1 and S2 have the same contents.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!  */
extern int mbmemcasecmp (const char *s1, size_t n1, const char *s2, size_t n2);


#ifdef __cplusplus
}
#endif

#endif /* MBMEMCASECMP_H */
