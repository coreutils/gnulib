/* Test intprops.h.
   Copyright (C) 2011-2015 Free Software Foundation, Inc.

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

/* Tell gcc not to warn about the many (X < 0) expressions that
   the overflow macros expand to.  */
#if (__GNUC__ == 4 && 3 <= __GNUC_MINOR__) || 4 < __GNUC__
# pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#include <config.h>

#include "intprops.h"
#include "verify.h"

#include <stdbool.h>
#include <inttypes.h>

#include "macros.h"

/* Create these CONST macros as alias for the standard ones, as some
   of the generic code below assumes each binary operator has a CONST
   alternative.  */
#define INT_CONST_DIVIDE_OVERFLOW(a, b) INT_DIVIDE_OVERFLOW (a, b)
#define INT_CONST_REMAINDER_OVERFLOW(a, b) INT_REMAINDER_OVERFLOW (a, b)
#define INT_CONST_LEFT_SHIFT_OVERFLOW(a, b) INT_LEFT_SHIFT_OVERFLOW (a, b)
#define INT_CONST_DIVIDE_WRAPV(a, b) INT_DIVIDE_WRAPV (a, b)
#define INT_CONST_REMAINDER_WRAPV(a, b) INT_REMAINDER_WRAPV (a, b)
#define INT_CONST_LEFT_SHIFT_WRAPV(a, b) INT_LEFT_SHIFT_WRAPV (a, b)

/* VERIFY (X) uses a static assertion for compilers that are known to work,
   and falls back on a dynamic assertion for other compilers.
   These tests should be checkable via 'verify' rather than 'ASSERT', but
   using 'verify' would run into a bug with HP-UX 11.23 cc; see
   <http://lists.gnu.org/archive/html/bug-gnulib/2011-05/msg00401.html>.  */
#if __GNUC__ || __SUNPRO_C
# define VERIFY(x) do { verify (x); } while (0)
#else
# define VERIFY(x) ASSERT (x)
#endif

#define DONTCARE __LINE__

int
main (void)
{
  /* Use VERIFY for tests that must be integer constant expressions,
     ASSERT otherwise.  */

  /* TYPE_IS_INTEGER.  */
  ASSERT (TYPE_IS_INTEGER (bool));
  ASSERT (TYPE_IS_INTEGER (char));
  ASSERT (TYPE_IS_INTEGER (signed char));
  ASSERT (TYPE_IS_INTEGER (unsigned char));
  ASSERT (TYPE_IS_INTEGER (short int));
  ASSERT (TYPE_IS_INTEGER (unsigned short int));
  ASSERT (TYPE_IS_INTEGER (int));
  ASSERT (TYPE_IS_INTEGER (unsigned int));
  ASSERT (TYPE_IS_INTEGER (long int));
  ASSERT (TYPE_IS_INTEGER (unsigned long int));
  ASSERT (TYPE_IS_INTEGER (intmax_t));
  ASSERT (TYPE_IS_INTEGER (uintmax_t));
  ASSERT (! TYPE_IS_INTEGER (float));
  ASSERT (! TYPE_IS_INTEGER (double));
  ASSERT (! TYPE_IS_INTEGER (long double));

  /* TYPE_SIGNED.  */
  /* VERIFY (! TYPE_SIGNED (bool)); // not guaranteed by gnulib substitute */
  VERIFY (TYPE_SIGNED (signed char));
  VERIFY (! TYPE_SIGNED (unsigned char));
  VERIFY (TYPE_SIGNED (short int));
  VERIFY (! TYPE_SIGNED (unsigned short int));
  VERIFY (TYPE_SIGNED (int));
  VERIFY (! TYPE_SIGNED (unsigned int));
  VERIFY (TYPE_SIGNED (long int));
  VERIFY (! TYPE_SIGNED (unsigned long int));
  VERIFY (TYPE_SIGNED (intmax_t));
  VERIFY (! TYPE_SIGNED (uintmax_t));
  ASSERT (TYPE_SIGNED (float));
  ASSERT (TYPE_SIGNED (double));
  ASSERT (TYPE_SIGNED (long double));

  /* Integer representation.  */
  VERIFY (INT_MIN + INT_MAX < 0
          ? (TYPE_TWOS_COMPLEMENT (int)
             && ! TYPE_ONES_COMPLEMENT (int) && ! TYPE_SIGNED_MAGNITUDE (int))
          : (! TYPE_TWOS_COMPLEMENT (int)
             && (TYPE_ONES_COMPLEMENT (int) || TYPE_SIGNED_MAGNITUDE (int))));

  /* TYPE_MINIMUM, TYPE_MAXIMUM.  */
  VERIFY (TYPE_MINIMUM (char) == CHAR_MIN);
  VERIFY (TYPE_MAXIMUM (char) == CHAR_MAX);
  VERIFY (TYPE_MINIMUM (unsigned char) == 0);
  VERIFY (TYPE_MAXIMUM (unsigned char) == UCHAR_MAX);
  VERIFY (TYPE_MINIMUM (signed char) == SCHAR_MIN);
  VERIFY (TYPE_MAXIMUM (signed char) == SCHAR_MAX);
  VERIFY (TYPE_MINIMUM (short int) == SHRT_MIN);
  VERIFY (TYPE_MAXIMUM (short int) == SHRT_MAX);
  VERIFY (TYPE_MINIMUM (unsigned short int) == 0);
  VERIFY (TYPE_MAXIMUM (unsigned short int) == USHRT_MAX);
  VERIFY (TYPE_MINIMUM (int) == INT_MIN);
  VERIFY (TYPE_MAXIMUM (int) == INT_MAX);
  VERIFY (TYPE_MINIMUM (unsigned int) == 0);
  VERIFY (TYPE_MAXIMUM (unsigned int) == UINT_MAX);
  VERIFY (TYPE_MINIMUM (long int) == LONG_MIN);
  VERIFY (TYPE_MAXIMUM (long int) == LONG_MAX);
  VERIFY (TYPE_MINIMUM (unsigned long int) == 0);
  VERIFY (TYPE_MAXIMUM (unsigned long int) == ULONG_MAX);
  VERIFY (TYPE_MINIMUM (intmax_t) == INTMAX_MIN);
  VERIFY (TYPE_MAXIMUM (intmax_t) == INTMAX_MAX);
  VERIFY (TYPE_MINIMUM (uintmax_t) == 0);
  VERIFY (TYPE_MAXIMUM (uintmax_t) == UINTMAX_MAX);

  /* INT_BITS_STRLEN_BOUND.  */
  VERIFY (INT_BITS_STRLEN_BOUND (1) == 1);
  VERIFY (INT_BITS_STRLEN_BOUND (2620) == 789);

  /* INT_STRLEN_BOUND, INT_BUFSIZE_BOUND.  */
  #ifdef INT32_MAX /* POSIX guarantees int32_t; this ports to non-POSIX.  */
  VERIFY (INT_STRLEN_BOUND (int32_t) == sizeof ("-2147483648") - 1);
  VERIFY (INT_BUFSIZE_BOUND (int32_t) == sizeof ("-2147483648"));
  #endif
  #ifdef INT64_MAX
  VERIFY (INT_STRLEN_BOUND (int64_t) == sizeof ("-9223372036854775808") - 1);
  VERIFY (INT_BUFSIZE_BOUND (int64_t) == sizeof ("-9223372036854775808"));
  #endif

  /* All the INT_<op>_RANGE_OVERFLOW tests are equally valid as
     INT_<op>_OVERFLOW tests, so define a single macro to do both.
     OP is the operation, A and B its operands, T the result type,
     V the overflow flag, and VRES the result if V.  If overflow
     occurs, assumes two's complement; that's good enough for
     tests.  */
  #define CHECK_BINOP(op, opname, a, b, t, v, vres)                       \
    VERIFY (INT_##opname##_RANGE_OVERFLOW (a, b, TYPE_MINIMUM (t),        \
                                           TYPE_MAXIMUM (t))              \
            == (v));                                                      \
    ASSERT (INT_##opname##_OVERFLOW (a, b) == (v));                       \
    VERIFY (INT_CONST_##opname##_OVERFLOW (a, b) == (v));                 \
    VERIFY (((t) INT_CONST_##opname##_WRAPV (a, b)                        \
             == ((v) ? (vres) : ((a) op (b))))                            \
            || ((v) && TYPE_SIGNED (t) && !TYPE_TWOS_COMPLEMENT (t)));    \
    ASSERT (INT_##opname##_WRAPV (a, b) == INT_CONST_##opname##_WRAPV (a, b))
  #define CHECK_UNOP(op, opname, a, t, v, vres)                           \
    VERIFY (INT_##opname##_RANGE_OVERFLOW (a, TYPE_MINIMUM (t),           \
                                           TYPE_MAXIMUM (t))              \
            == (v));                                                      \
    VERIFY (INT_##opname##_OVERFLOW (a) == (v));                          \
    VERIFY ((t) INT_##opname##_WRAPV (a) == ((v) ? (vres) : (op (a)))     \
            || ((v) && TYPE_SIGNED (t) && !TYPE_TWOS_COMPLEMENT (t)))

  /* INT_<op>_RANGE_OVERFLOW, INT_<op>_OVERFLOW.  */
  VERIFY (INT_ADD_RANGE_OVERFLOW (INT_MAX, 1, INT_MIN, INT_MAX));
  VERIFY (INT_CONST_ADD_OVERFLOW (INT_MAX, 1));
  CHECK_BINOP (+, ADD, INT_MAX, 1, int, true, INT_MIN);
  CHECK_BINOP (+, ADD, INT_MAX, -1, int, false, INT_MAX - 1);
  CHECK_BINOP (+, ADD, INT_MIN, 1, int, false, INT_MIN + 1);
  CHECK_BINOP (+, ADD, INT_MIN, -1, int, true, INT_MAX);
  CHECK_BINOP (+, ADD, UINT_MAX, 1u, unsigned int, true, 0u);
  CHECK_BINOP (+, ADD, 0u, 1u, unsigned int, false, 1u);

  CHECK_BINOP (-, SUBTRACT, INT_MAX, 1, int, false, INT_MAX - 1);
  CHECK_BINOP (-, SUBTRACT, INT_MAX, -1, int, true, INT_MIN);
  CHECK_BINOP (-, SUBTRACT, INT_MIN, 1, int, true, INT_MAX);
  CHECK_BINOP (-, SUBTRACT, INT_MIN, -1, int, false, INT_MIN - -1);
  CHECK_BINOP (-, SUBTRACT, UINT_MAX, 1u, unsigned int, false, UINT_MAX - 1u);
  CHECK_BINOP (-, SUBTRACT, 0u, 1u, unsigned int, true, 0u - 1u);

  CHECK_UNOP (-, NEGATE, INT_MIN, int, TYPE_TWOS_COMPLEMENT (int), INT_MIN);
  CHECK_UNOP (-, NEGATE, 0, int, false, -0);
  CHECK_UNOP (-, NEGATE, INT_MAX, int, false, -INT_MAX);
  CHECK_UNOP (-, NEGATE, 0u, unsigned int, false, -0u);
  CHECK_UNOP (-, NEGATE, 1u, unsigned int, true, -1u);
  CHECK_UNOP (-, NEGATE, UINT_MAX, unsigned int, true, -UINT_MAX);

  CHECK_BINOP (*, MULTIPLY, INT_MAX, INT_MAX, int, true, 1);
  CHECK_BINOP (*, MULTIPLY, INT_MAX, INT_MIN, int, true, INT_MIN);
  CHECK_BINOP (*, MULTIPLY, INT_MIN, INT_MAX, int, true, INT_MIN);
  CHECK_BINOP (*, MULTIPLY, INT_MIN, INT_MIN, int, true, 0);
  CHECK_BINOP (*, MULTIPLY, -1, INT_MIN, int,
               INT_NEGATE_OVERFLOW (INT_MIN), INT_MIN);
  CHECK_BINOP (*, MULTIPLY, LONG_MIN / INT_MAX, (long int) INT_MAX,
               long int, false, LONG_MIN - LONG_MIN % INT_MAX);

  CHECK_BINOP (/, DIVIDE, INT_MIN, -1, int,
               INT_NEGATE_OVERFLOW (INT_MIN), INT_MIN);
  CHECK_BINOP (/, DIVIDE, INT_MAX, 1, int, false, INT_MAX);
  CHECK_BINOP (/, DIVIDE, (unsigned int) INT_MIN, -1u, unsigned int,
               false, INT_MIN / -1u);

  CHECK_BINOP (%, REMAINDER, INT_MIN, -1, int, INT_NEGATE_OVERFLOW (INT_MIN), 0);
  CHECK_BINOP (%, REMAINDER, INT_MAX, 1, int, false, 0);
  CHECK_BINOP (%, REMAINDER, (unsigned int) INT_MIN, -1u, unsigned int,
               false, INT_MIN % -1u);

  CHECK_BINOP (<<, LEFT_SHIFT, UINT_MAX, 1, unsigned int, true, UINT_MAX << 1);
  CHECK_BINOP (<<, LEFT_SHIFT, UINT_MAX / 2 + 1, 1, unsigned int, true,
               (UINT_MAX / 2 + 1) << 1);
  CHECK_BINOP (<<, LEFT_SHIFT, UINT_MAX / 2, 1, unsigned int, false,
               (UINT_MAX / 2) << 1);

  /* INT_<op>_OVERFLOW and INT_<op>_WRAPV with mixed types.  */
  #define CHECK_SUM(a, b, t, v, vres)                                     \
    ASSERT (INT_ADD_OVERFLOW (a, b) == (v));                              \
    ASSERT (INT_ADD_OVERFLOW (b, a) == (v));                              \
    VERIFY (INT_CONST_ADD_OVERFLOW (a, b) == (v));                        \
    VERIFY (INT_CONST_ADD_OVERFLOW (b, a) == (v));                        \
    VERIFY ((t) INT_CONST_ADD_WRAPV (a, b) == (t) INT_CONST_ADD_WRAPV (b, a)); \
    VERIFY ((t) INT_CONST_ADD_WRAPV (a, b) == ((v) ? (vres) : (a) + (b))  \
            || ((v) && TYPE_SIGNED (t) && !TYPE_TWOS_COMPLEMENT (t)));    \
    ASSERT ((t) INT_ADD_WRAPV (a, b) == (t) INT_CONST_ADD_WRAPV (a, b));   \
    ASSERT ((t) INT_ADD_WRAPV (b, a) == (t) INT_CONST_ADD_WRAPV (a, b))
  CHECK_SUM (-1, LONG_MIN, long int, true, LONG_MAX);
  CHECK_SUM (-1, UINT_MAX, unsigned int, false, DONTCARE);
  CHECK_SUM (-1L, INT_MIN, long int, INT_MIN == LONG_MIN,
             INT_MIN == LONG_MIN ? INT_MAX : DONTCARE);
  CHECK_SUM (0u, -1, unsigned int, true, 0u + -1);
  CHECK_SUM (0u, 0, unsigned int, false, DONTCARE);
  CHECK_SUM (0u, 1, unsigned int, false, DONTCARE);
  CHECK_SUM (1, LONG_MAX, long int, true, LONG_MIN);
  CHECK_SUM (1, UINT_MAX, unsigned int, true, 0u);
  CHECK_SUM (1L, INT_MAX, long int, INT_MAX == LONG_MAX,
             INT_MAX == LONG_MAX ? INT_MIN : DONTCARE);
  CHECK_SUM (1u, INT_MAX, unsigned int, INT_MAX == UINT_MAX, 1u + INT_MAX);
  CHECK_SUM (1u, INT_MIN, unsigned int, true, 1u + INT_MIN);

  VERIFY (! INT_CONST_SUBTRACT_OVERFLOW (INT_MAX, 1u));
  VERIFY (! INT_CONST_SUBTRACT_OVERFLOW (UINT_MAX, 1));
  VERIFY (! INT_CONST_SUBTRACT_OVERFLOW (0u, -1));
  VERIFY (INT_CONST_SUBTRACT_OVERFLOW (UINT_MAX, -1));
  VERIFY (INT_CONST_SUBTRACT_OVERFLOW (INT_MIN, 1u));
  VERIFY (INT_CONST_SUBTRACT_OVERFLOW (-1, 0u));

  #define CHECK_PRODUCT(a, b, t, v, vres)                                 \
    ASSERT (INT_MULTIPLY_OVERFLOW (a, b) == (v));                         \
    ASSERT (INT_MULTIPLY_OVERFLOW (b, a) == (v));                         \
    VERIFY (INT_CONST_MULTIPLY_OVERFLOW (a, b) == (v));                   \
    VERIFY (INT_CONST_MULTIPLY_OVERFLOW (b, a) == (v));                   \
    VERIFY ((t) INT_CONST_MULTIPLY_WRAPV (a, b)                           \
            == (t) INT_CONST_MULTIPLY_WRAPV (b, a));                      \
    VERIFY ((t) INT_CONST_MULTIPLY_WRAPV (a, b) == ((v) ? (vres) : (a) * (b)) \
            || ((v) && TYPE_SIGNED (t) && !TYPE_TWOS_COMPLEMENT (t)));    \
    ASSERT ((t) INT_MULTIPLY_WRAPV (a, b)                                 \
            == (t) INT_CONST_MULTIPLY_WRAPV (a, b));                      \
    ASSERT ((t) INT_MULTIPLY_WRAPV (b, a)                                 \
            == (t) INT_CONST_MULTIPLY_WRAPV (a, b))
  CHECK_PRODUCT (-1, 1u, unsigned int, true, -1 * 1u);
  CHECK_PRODUCT (-1, INT_MIN, int, INT_NEGATE_OVERFLOW (INT_MIN), INT_MIN);
  CHECK_PRODUCT (-1, UINT_MAX, unsigned int, true, -1 * UINT_MAX);
  CHECK_PRODUCT (-32768, LONG_MAX / -32768 - 1, long int, true, LONG_MIN);
  CHECK_PRODUCT (-12345, LONG_MAX / -12345, long int, false, DONTCARE);
  CHECK_PRODUCT (0, -1, int, false, DONTCARE);
  CHECK_PRODUCT (0, 0, int, false, DONTCARE);
  CHECK_PRODUCT (0, 0u, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0, 1, int, false, DONTCARE);
  CHECK_PRODUCT (0, INT_MAX, int, false, DONTCARE);
  CHECK_PRODUCT (0, INT_MIN, int, false, DONTCARE);
  CHECK_PRODUCT (0, UINT_MAX, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, -1, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, 0, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, 0u, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, 1, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, INT_MAX, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, INT_MIN, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (0u, UINT_MAX, unsigned int, false, DONTCARE);
  CHECK_PRODUCT (1, INT_MAX, int, false, DONTCARE);
  CHECK_PRODUCT (1, INT_MIN, int, false, DONTCARE);
  CHECK_PRODUCT (1, UINT_MAX, int, false, DONTCARE);
  CHECK_PRODUCT (1u, INT_MIN, unsigned int, true, 1u * INT_MIN);
  CHECK_PRODUCT (1u, INT_MAX, unsigned int, UINT_MAX < INT_MAX, 1u * INT_MAX);
  CHECK_PRODUCT (INT_MAX, UINT_MAX, unsigned int, true, INT_MAX * UINT_MAX);
  CHECK_PRODUCT (INT_MAX, ULONG_MAX, unsigned long int, true,
                 INT_MAX * ULONG_MAX);
  CHECK_PRODUCT (INT_MIN, LONG_MAX / INT_MIN - 1, long int, true, LONG_MIN);
  CHECK_PRODUCT (INT_MIN, LONG_MAX / INT_MIN, long int, false, DONTCARE);
  CHECK_PRODUCT (INT_MIN, UINT_MAX, unsigned int, true, INT_MIN * UINT_MAX);
  CHECK_PRODUCT (INT_MIN, ULONG_MAX, unsigned long int, true,
                 INT_MIN * ULONG_MAX);

  #define CHECK_QUOTIENT(a, b, t, v)                                 \
    VERIFY (INT_DIVIDE_OVERFLOW (a, b) == (v));                      \
    VERIFY ((v) || (t) INT_DIVIDE_WRAPV (a, b) == (a) / (b))

  CHECK_QUOTIENT (INT_MIN, -1L, long int,
                  TYPE_TWOS_COMPLEMENT (long int) && INT_MIN == LONG_MIN);
  CHECK_QUOTIENT (INT_MIN, UINT_MAX, unsigned int, false);
  CHECK_QUOTIENT (INTMAX_MIN, UINTMAX_MAX, uintmax_t, false);
  CHECK_QUOTIENT (INTMAX_MIN, UINT_MAX, intmax_t, false);
  CHECK_QUOTIENT (-11, 10u, unsigned int, true);
  CHECK_QUOTIENT (-10, 10u, unsigned int, true);
  CHECK_QUOTIENT (-9, 10u, unsigned int, false);
  CHECK_QUOTIENT (11u, -10, unsigned int, true);
  CHECK_QUOTIENT (10u, -10, unsigned int, true);
  CHECK_QUOTIENT (9u, -10, unsigned int, false);

  #define CHECK_REMAINDER(a, b, t, v)                             \
    VERIFY (INT_REMAINDER_OVERFLOW (a, b) == (v));                \
    VERIFY ((v) || (t) INT_REMAINDER_WRAPV (a, b) == (a) % (b))

  CHECK_REMAINDER (INT_MIN, -1L, long int,
                   TYPE_TWOS_COMPLEMENT (long int) && INT_MIN == LONG_MIN);
  CHECK_REMAINDER (-1, UINT_MAX, unsigned int, true);
  CHECK_REMAINDER ((intmax_t) -1, UINTMAX_MAX, uintmax_t, true);
  CHECK_REMAINDER (INTMAX_MIN, UINT_MAX, intmax_t,
                   (INTMAX_MAX < UINT_MAX
                    && - (unsigned int) INTMAX_MIN % UINT_MAX != 0));
  CHECK_REMAINDER (INT_MIN, ULONG_MAX, unsigned long int,
                   INT_MIN % ULONG_MAX != 1);
  CHECK_REMAINDER (1u, -1, unsigned int, false);
  CHECK_REMAINDER (37*39u, -39, unsigned int, false);
  CHECK_REMAINDER (37*39u + 1, -39, unsigned int, true);
  CHECK_REMAINDER (37*39u - 1, -39, unsigned int, true);
  CHECK_REMAINDER (LONG_MAX, -INT_MAX, long int, false);

  return 0;
}
