/* Test <stdcountof.h>.
   Copyright 2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdcountof.h>

#include "macros.h"

/* Whether the compiler supports _Generic.
   Test program:
     int f (int x) { return _Generic (x, char *: 2, int: 3); }
 */
#if (defined __GNUC__ && __GNUC__ + (__GNUC_MINOR__ >= 9) > 4) \
    || (defined __clang__ && __clang_major__ >= 3) \
    || (defined __SUNPRO_C && __SUNPRO_C >= 0x5150)
# define HAVE__GENERIC 1
#endif

extern int integer;
extern int unbounded[];
extern int bounded[10];
extern int multidimensional[10][20];

static void
test_func (int parameter[3])
{
  int local_bounded[20];

  (void) local_bounded;

  (void) _gl_verify_is_array (unbounded);
  (void) _gl_verify_is_array (bounded);
  (void) _gl_verify_is_array (multidimensional);

  ASSERT (countof (bounded) == 10);
  ASSERT (countof (multidimensional) == 10);
  ASSERT (countof (local_bounded) == 20);

#if 0 /* These produce compilation errors.  */
  (void) _gl_verify_is_array (integer);
  (void) _gl_verify_is_array (parameter);

  ASSERT (countof (integer) >= 0);
  ASSERT (countof (unbounded) >= 0);
#endif

  {
    extern int a, b, c;
    ASSERT (countof (((int[]) { a, b, c })) == 3);
  }

  /* Check that countof(...) is an expression of type size_t.  */
#if !defined __cplusplus && HAVE__GENERIC
  ASSERT (_Generic (countof (bounded),          size_t: 1, default: 0));
  ASSERT (_Generic (countof (multidimensional), size_t: 1, default: 0));
  ASSERT (_Generic (countof (local_bounded),    size_t: 1, default: 0));
#endif
}

int
main ()
{
  int x[3];

  test_func (x);

  return test_exit_status;
}
