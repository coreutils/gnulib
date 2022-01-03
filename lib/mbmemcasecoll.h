/* Locale-specific case-ignoring memory comparison.
   Copyright (C) 2001, 2009-2022 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2001.  */

#ifndef MBMEMCASECOLL_H
#define MBMEMCASECOLL_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Compare the memory regions S1 = [s1..s1+s1len-1], S2 = [s2..s2+s2len-1],
   that contain character sequences, using the rules of the current locale,
   ignoring case.
   HARD_LC_COLLATE is false if the LC_COLLATE category of the current locale
   is equivalent to the "C" locale.

   This function's result is locale dependent.  Unlike memcasecmp(), it works
   correctly in multibyte locales and also handles Turkish i / dotless i.
   Unlike ulc_casecmp(), it does not handle the German sharp s and the Greek
   final sigma.  Like memcoll() and ulc_casecoll(), it uses collation order.

   Return a negative number if S1 < S2, a positive number if S1 > S2, 0 if
   S1 and S2 have the same contents, or an unspecified value if there is an
   error.
   Set errno to an error number if there is an error, and to zero otherwise.

   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!  */

extern int mbmemcasecoll (const char *s1, size_t s1len,
                          const char *s2, size_t s2len,
                          bool hard_LC_COLLATE);


#ifdef __cplusplus
}
#endif

#endif /* MBMEMCASECOLL_H */
