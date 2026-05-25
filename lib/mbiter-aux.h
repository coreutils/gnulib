/* Auxiliary functions for iterating through multibyte strings.
   Copyright (C) 2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>.  */

#ifndef _MBITER_AUX_H
#define _MBITER_AUX_H 1

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Determines whether the current locale encoding is UTF-8.
   Stores the value in *CACHE, that should be pre-initialized with -1.  */
extern bool mbiter_is_utf8 (int *cache);

/* If the current locale encoding is UTF-8 and a preceding
     mbrtoc32 (&uc, S, N, &state)
   invocation returned (size_t) -1, this function returns the number of
   initial bytes that form a maximal subpart in the sense of
   https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf page 127..129.
   The result is >= 1, <= N.  */
extern size_t mbiter_utf8_maximal_subpart (const char *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* _MBITER_AUX_H */
