/* Grapheme cluster breaks in Unicode strings.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@cs.stanford.edu>, 2010.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef _UNIGBRK_H
#define _UNIGBRK_H

/* Get bool.  */
#include <stdbool.h>

/* Get size_t. */
#include <stddef.h>

#include "unitypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/* Property defined in Unicode Standard Annex #29, section "Grapheme Cluster
   Boundaries"
   <https://unicode.org/reports/tr29/#Grapheme_Cluster_Boundaries> */

/* Possible values of the Grapheme_Cluster_Break property.
   This enumeration may be extended in the future.  */
enum
{
  GBP_OTHER        = 0,
  GBP_CR           = 1,
  GBP_LF           = 2,
  GBP_CONTROL      = 3,
  GBP_EXTEND       = 4,
  GBP_PREPEND      = 5,
  GBP_SPACINGMARK  = 6,
  GBP_L            = 7,
  GBP_V            = 8,
  GBP_T            = 9,
  GBP_LV           = 10,
  GBP_LVT          = 11,
  GBP_RI           = 12,
  GBP_ZWJ          = 13,
  GBP_EB           = 14, /* obsolete */
  GBP_EM           = 15, /* obsolete */
  GBP_GAZ          = 16, /* obsolete */
  GBP_EBG          = 17  /* obsolete */
};

/* Return the Grapheme_Cluster_Break property of a Unicode character. */
extern int
       uc_graphemeclusterbreak_property (ucs4_t uc)
       _UC_ATTRIBUTE_CONST;

/* ========================================================================= */

/* Grapheme cluster breaks.  */

/* Returns true if there is a grapheme cluster boundary between Unicode code
   points A and B.  A "grapheme cluster" is an approximation to a
   user-perceived character, which sometimes corresponds to multiple code
   points.  For example, an English letter followed by an acute accent can be
   expressed as two consecutive Unicode code points, but it is perceived by the
   user as only a single character and therefore constitutes a single grapheme
   cluster.

   Implements extended (not legacy) grapheme cluster rules, because UAX #29
   indicates that they are preferred.

   Note: This function does not work right with syllables in Indic scripts or
   emojis, because it does not look at the characters before A and after B.

   Use A == 0 or B == 0 to indicate start of text or end of text,
   respectively. */
extern bool
       uc_is_grapheme_break (ucs4_t a, ucs4_t b)
       _UC_ATTRIBUTE_CONST;

/* Returns the start of the next grapheme cluster following S, or NULL if the
   end of the string has been reached.
   Note: These functions do not work right with syllables in Indic scripts or
   emojis, because they do not consider the characters before S. */
extern const uint8_t *
       u8_grapheme_next (const uint8_t *s, const uint8_t *end)
       _UC_ATTRIBUTE_PURE;
extern const uint16_t *
       u16_grapheme_next (const uint16_t *s, const uint16_t *end)
       _UC_ATTRIBUTE_PURE;
extern const uint32_t *
       u32_grapheme_next (const uint32_t *s, const uint32_t *end)
       _UC_ATTRIBUTE_PURE;
#ifndef _LIBUNISTRING_NO_CONST_GENERICS
# ifdef __cplusplus
}
# endif
/* Don't silently convert a 'const uintN_t *' to a 'uintN_t *'.  Programmers
   want compiler warnings for 'const' related mistakes.  */
# ifdef __cplusplus
template <typename T>
  T * u8_grapheme_next_template (T* s, const uint8_t *end);
template <>
  inline uint8_t * u8_grapheme_next_template (uint8_t *s, const uint8_t *end)
  { return const_cast<uint8_t *>(u8_grapheme_next (s, end)); }
template <>
  inline const uint8_t * u8_grapheme_next_template (const uint8_t *s, const uint8_t *end)
  { return u8_grapheme_next (s, end); }
#  undef u8_grapheme_next
#  define u8_grapheme_next u8_grapheme_next_template
# elif !defined u8_grapheme_next
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u8_grapheme_next(s,end) \
      _Generic ((s), \
                uint8_t *: (uint8_t *) u8_grapheme_next ((s), (end)), \
                default  :             u8_grapheme_next ((s), (end)))
#  endif
# endif
# ifdef __cplusplus
template <typename T>
  T * u16_grapheme_next_template (T* s, const uint16_t *end);
template <>
  inline uint16_t * u16_grapheme_next_template (uint16_t *s, const uint16_t *end)
  { return const_cast<uint16_t *>(u16_grapheme_next (s, end)); }
template <>
  inline const uint16_t * u16_grapheme_next_template (const uint16_t *s, const uint16_t *end)
  { return u16_grapheme_next (s, end); }
#  undef u16_grapheme_next
#  define u16_grapheme_next u16_grapheme_next_template
# elif !defined u16_grapheme_next
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u16_grapheme_next(s,end) \
      _Generic ((s), \
                uint16_t *: (uint16_t *) u16_grapheme_next ((s), (end)), \
                default   :              u16_grapheme_next ((s), (end)))
#  endif
# endif
# ifdef __cplusplus
template <typename T>
  T * u32_grapheme_next_template (T* s, const uint32_t *end);
template <>
  inline uint32_t * u32_grapheme_next_template (uint32_t *s, const uint32_t *end)
  { return const_cast<uint32_t *>(u32_grapheme_next (s, end)); }
template <>
  inline const uint32_t * u32_grapheme_next_template (const uint32_t *s, const uint32_t *end)
  { return u32_grapheme_next (s, end); }
#  undef u32_grapheme_next
#  define u32_grapheme_next u32_grapheme_next_template
# elif !defined u32_grapheme_next
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u32_grapheme_next(s,end) \
      _Generic ((s), \
                uint32_t *: (uint32_t *) u32_grapheme_next ((s), (end)), \
                default   :              u32_grapheme_next ((s), (end)))
#  endif
# endif
# ifdef __cplusplus
extern "C" {
# endif
#endif

/* Returns the start of the previous grapheme cluster before S, or NULL if the
   start of the string has been reached.
   Note: These functions do not work right with syllables in Indic scripts or
   emojis, because they do not consider the characters at or after S. */
extern const uint8_t *
       u8_grapheme_prev (const uint8_t *s, const uint8_t *start)
       _UC_ATTRIBUTE_PURE;
extern const uint16_t *
       u16_grapheme_prev (const uint16_t *s, const uint16_t *start)
       _UC_ATTRIBUTE_PURE;
extern const uint32_t *
       u32_grapheme_prev (const uint32_t *s, const uint32_t *start)
       _UC_ATTRIBUTE_PURE;
#ifndef _LIBUNISTRING_NO_CONST_GENERICS
# ifdef __cplusplus
}
# endif
/* Don't silently convert a 'const uintN_t *' to a 'uintN_t *'.  Programmers
   want compiler warnings for 'const' related mistakes.  */
# ifdef __cplusplus
template <typename T>
  T * u8_grapheme_prev_template (T* s, const uint8_t *start);
template <>
  inline uint8_t * u8_grapheme_prev_template (uint8_t *s, const uint8_t *start)
  { return const_cast<uint8_t *>(u8_grapheme_prev (s, start)); }
template <>
  inline const uint8_t * u8_grapheme_prev_template (const uint8_t *s, const uint8_t *start)
  { return u8_grapheme_prev (s, start); }
#  undef u8_grapheme_prev
#  define u8_grapheme_prev u8_grapheme_prev_template
# elif !defined u8_grapheme_prev
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u8_grapheme_prev(s,start) \
      _Generic ((s), \
                uint8_t *: (uint8_t *) u8_grapheme_prev ((s), (start)), \
                default  :             u8_grapheme_prev ((s), (start)))
#  endif
# endif
# ifdef __cplusplus
template <typename T>
  T * u16_grapheme_prev_template (T* s, const uint16_t *start);
template <>
  inline uint16_t * u16_grapheme_prev_template (uint16_t *s, const uint16_t *start)
  { return const_cast<uint16_t *>(u16_grapheme_prev (s, start)); }
template <>
  inline const uint16_t * u16_grapheme_prev_template (const uint16_t *s, const uint16_t *start)
  { return u16_grapheme_prev (s, start); }
#  undef u16_grapheme_prev
#  define u16_grapheme_prev u16_grapheme_prev_template
# elif !defined u16_grapheme_prev
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u16_grapheme_prev(s,start) \
      _Generic ((s), \
                uint16_t *: (uint16_t *) u16_grapheme_prev ((s), (start)), \
                default   :              u16_grapheme_prev ((s), (start)))
#  endif
# endif
# ifdef __cplusplus
template <typename T>
  T * u32_grapheme_prev_template (T* s, const uint32_t *start);
template <>
  inline uint32_t * u32_grapheme_prev_template (uint32_t *s, const uint32_t *start)
  { return const_cast<uint32_t *>(u32_grapheme_prev (s, start)); }
template <>
  inline const uint32_t * u32_grapheme_prev_template (const uint32_t *s, const uint32_t *start)
  { return u32_grapheme_prev (s, start); }
#  undef u32_grapheme_prev
#  define u32_grapheme_prev u32_grapheme_prev_template
# elif !defined u32_grapheme_prev
#  if ((__GNUC__ + (__GNUC_MINOR__ >= 9) > 4) || (__clang_major__ >= 3) \
       || defined __ICC  || defined __TINYC__ \
       || (__STDC_VERSION__ >= 201112L && !(defined __GNUC__ || defined __clang__)))
#   define u32_grapheme_prev(s,start) \
      _Generic ((s), \
                uint32_t *: (uint32_t *) u32_grapheme_prev ((s), (start)), \
                default   :              u32_grapheme_prev ((s), (start)))
#  endif
# endif
# ifdef __cplusplus
extern "C" {
# endif
#endif

/* Determine the grapheme cluster boundaries in S, and store the result at
   p[0..n-1].  p[i] = 1 means that a new grapheme cluster begins at s[i].  p[i]
   = 0 means that s[i-1] and s[i] are part of the same grapheme cluster.  p[0]
   will always be 1.
 */
extern void
       u8_grapheme_breaks (const uint8_t *s, size_t n, char *p);
extern void
       u16_grapheme_breaks (const uint16_t *s, size_t n, char *p);
extern void
       u32_grapheme_breaks (const uint32_t *s, size_t n, char *p);
extern void
       ulc_grapheme_breaks (const char *s, size_t n, char *p);
extern void
       uc_grapheme_breaks (const ucs4_t *s, size_t n, char *p);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif


#endif /* _UNIGBRK_H */
