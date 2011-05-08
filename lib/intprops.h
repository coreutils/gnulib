/* intprops.h -- properties of integer types

   Copyright (C) 2001-2005, 2009-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert.  */

#ifndef _GL_INTPROPS_H
#define _GL_INTPROPS_H

#include <limits.h>

/* The extra casts in the following macros work around compiler bugs,
   e.g., in Cray C 5.0.3.0.  */

/* True if the arithmetic type T is an integer type.  bool counts as
   an integer.  */
#define TYPE_IS_INTEGER(t) ((t) 1.5 == 1)

/* True if negative values of the signed integer type T use two's
   complement, ones' complement, or signed magnitude representation,
   respectively.  Much GNU code assumes two's complement, but some
   people like to be portable to all possible C hosts.  */
#define TYPE_TWOS_COMPLEMENT(t) ((t) ~ (t) 0 == (t) -1)
#define TYPE_ONES_COMPLEMENT(t) ((t) ~ (t) 0 == 0)
#define TYPE_SIGNED_MAGNITUDE(t) ((t) ~ (t) 0 < (t) -1)

/* True if the arithmetic type T is signed.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

/* The maximum and minimum values for the integer type T.  These
   macros have undefined behavior if T is signed and has padding bits.
   If this is a problem for you, please let us know how to fix it for
   your host.  */
#define TYPE_MINIMUM(t)                                                 \
  ((t) (! TYPE_SIGNED (t)                                               \
        ? (t) 0                                                         \
        : TYPE_SIGNED_MAGNITUDE (t)                                     \
        ? ~ (t) 0                                                       \
        : ~ TYPE_MAXIMUM (t)))
#define TYPE_MAXIMUM(t)                                                 \
  ((t) (! TYPE_SIGNED (t)                                               \
        ? (t) -1                                                        \
        : ((((t) 1 << (sizeof (t) * CHAR_BIT - 2)) - 1) * 2 + 1)))

/* Return 1 if the __typeof__ keyword works.  This could be done by
   'configure', but for now it's easier to do it by hand.  */
#if 2 <= __GNUC__ || 0x5110 <= __SUNPRO_C
# define _GL_HAVE___TYPEOF__ 1
#else
# define _GL_HAVE___TYPEOF__ 0
#endif

/* Return 1 if the integer type or expression T might be signed.  Return 0
   if it is definitely unsigned.  This macro does not evaluate its argument,
   and expands to an integer constant expression.  */
#if _GL_HAVE___TYPEOF__
# define _GL_SIGNED_TYPE_OR_EXPR(t) TYPE_SIGNED (__typeof__ (t))
#else
# define _GL_SIGNED_TYPE_OR_EXPR(t) 1
#endif

/* Bound on length of the string representing an unsigned integer
   value representable in B bits.  log10 (2.0) < 146/485.  The
   smallest value of B where this bound is not tight is 2621.  */
#define INT_BITS_STRLEN_BOUND(b) (((b) * 146 + 484) / 485)

/* Bound on length of the string representing an integer type or expression T.
   Subtract 1 for the sign bit if T is signed, and then add 1 more for
   a minus sign if needed.

   Because _GL_SIGNED_TYPE_OR_EXPR sometimes returns 0 when its argument is
   signed, this macro may overestimate the true bound by one byte when
   applied to unsigned types of size 2, 4, 16, ... bytes.  */
#define INT_STRLEN_BOUND(t)                                     \
  (INT_BITS_STRLEN_BOUND (sizeof (t) * CHAR_BIT                 \
                          - _GL_SIGNED_TYPE_OR_EXPR (t))        \
   + _GL_SIGNED_TYPE_OR_EXPR (t))

/* Bound on buffer size needed to represent an integer type or expression T,
   including the terminating null.  */
#define INT_BUFSIZE_BOUND(t) (INT_STRLEN_BOUND (t) + 1)

#endif /* _GL_INTPROPS_H */
