/* Unicode character case mappings.
   Copyright (C) 2002, 2009 Free Software Foundation, Inc.

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

#ifndef _UNICASE_H
#define _UNICASE_H

#include "unitypes.h"

/* Get size_t.  */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/* Character case mappings.
   These mappings are locale and context independent.
   WARNING! These functions are not sufficient for languages such as German.
   Better use the functions below that treat an entire string at once and are
   language aware.  */

/* Return the uppercase mapping of a Unicode character.  */
extern ucs4_t
       uc_toupper (ucs4_t uc);

/* Return the lowercase mapping of a Unicode character.  */
extern ucs4_t
       uc_tolower (ucs4_t uc);

/* Return the titlecase mapping of a Unicode character.  */
extern ucs4_t
       uc_totitle (ucs4_t uc);

/* ========================================================================= */

/* String case mappings.  */

/* These functions are locale dependent.  The iso639_language argument
   identifies the language (e.g. "tr" for Turkish).  NULL means to use
   locale independent case mappings.  */

/* Return the ISO 639 language code of the current locale.
   Return "" if it is unknown, or in the "C" locale.  */
extern const char *
       uc_locale_language (void);

/* Return the uppercase mapping of a string.  */
extern uint8_t *
       u8_toupper (const uint8_t *s, size_t n, const char *iso639_language, uint8_t *resultbuf, size_t *lengthp);
extern uint16_t *
       u16_toupper (const uint16_t *s, size_t n, const char *iso639_language, uint16_t *resultbuf, size_t *lengthp);
extern uint32_t *
       u32_toupper (const uint32_t *s, size_t n, const char *iso639_language, uint32_t *resultbuf, size_t *lengthp);

/* Return the lowercase mapping of a string.  */
extern uint8_t *
       u8_tolower (const uint8_t *s, size_t n, const char *iso639_language, uint8_t *resultbuf, size_t *lengthp);
extern uint16_t *
       u16_tolower (const uint16_t *s, size_t n, const char *iso639_language, uint16_t *resultbuf, size_t *lengthp);
extern uint32_t *
       u32_tolower (const uint32_t *s, size_t n, const char *iso639_language, uint32_t *resultbuf, size_t *lengthp);

/* Return the titlecase mapping of a string.  */
extern uint8_t *
       u8_totitle (const uint8_t *s, size_t n, const char *iso639_language, uint8_t *resultbuf, size_t *lengthp);
extern uint16_t *
       u16_totitle (const uint16_t *s, size_t n, const char *iso639_language, uint16_t *resultbuf, size_t *lengthp);
extern uint32_t *
       u32_totitle (const uint32_t *s, size_t n, const char *iso639_language, uint32_t *resultbuf, size_t *lengthp);

/* Return the case folded string.  */
extern uint8_t *
       u8_casefold (const uint8_t *s, size_t n, uint8_t *resultbuf, size_t *lengthp);
extern uint16_t *
       u16_casefold (const uint16_t *s, size_t n, uint16_t *resultbuf, size_t *lengthp);
extern uint32_t *
       u32_casefold (const uint32_t *s, size_t n, uint32_t *resultbuf, size_t *lengthp);

/* Compare S1 and S2, ignoring case.
   Return -1 if S1 < S2, 0 if S1 = S2, 1 if S1 > S2.  */
extern int
       u8_casecmp (const uint8_t *s1, size_t n1, const uint8_t *s2, size_t n2);
extern int
       u16_casecmp (const uint16_t *s1, size_t n1, const uint16_t *s2, size_t n2);
extern int
       u32_casecmp (const uint32_t *s1, size_t n1, const uint32_t *s2, size_t n2);

/* Compare S1 and S2 using the collation rules of the current locale,
   ignoring case.
   Return -1 if S1 < S2, 0 if S1 = S2, 1 if S1 > S2.
   Upon failure, set errno and return any value.  */
extern int
       u8_casecoll (const uint8_t *s1, size_t n1, const uint8_t *s2, size_t n2);
extern int
       u16_casecoll (const uint16_t *s1, size_t n1, const uint16_t *s2, size_t n2);
extern int
       u32_casecoll (const uint32_t *s1, size_t n1, const uint32_t *s2, size_t n2);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif

#endif /* _UNICASE_H */
