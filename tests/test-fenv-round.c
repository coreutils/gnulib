/* Test of floating-point rounding direction.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

/* Specification.  */
#include <fenv.h>

#include "macros.h"

/* Check the FE_TOWARDZERO rounding direction.  */
_GL_UNUSED static void
test_towardzero ()
{
  {
    float volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
  {
    double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
#if !(((defined __FreeBSD__ || defined __OpenBSD__ || defined __ANDROID__) && defined __aarch64__) || (defined __NetBSD__ && defined __sparcv9 /* sparc64 */) || (defined __OpenBSD__ && defined __mips64))
  {
    long double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
#endif
}

/* Check the FE_UPWARD rounding direction.  */
_GL_UNUSED static void
test_upward ()
{
  {
    float volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
  {
    double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
#if !(((defined __FreeBSD__ || defined __OpenBSD__ || defined __ANDROID__) && defined __aarch64__) || (defined __NetBSD__ && defined __sparcv9 /* sparc64 */) || (defined __OpenBSD__ && defined __mips64))
  {
    long double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p > a);
  }
#endif
}

/* Check the FE_DOWNWARD rounding direction.  */
_GL_UNUSED static void
test_downward ()
{
  {
    float volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);
  }
  {
    double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);
  }
#if !(((defined __FreeBSD__ || defined __OpenBSD__ || defined __ANDROID__) && defined __aarch64__) || (defined __NetBSD__ && defined __sparcv9 /* sparc64 */) || (defined __OpenBSD__ && defined __mips64))
  {
    long double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p < a);
  }
#endif
}

/* Check the FE_TONEAREST rounding direction.  */
static void
test_tonearest ()
{
  {
    float volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);
  }
  {
    double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);
  }
  {
    long double volatile a, b, q, p;

    a = 1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);

    a = -1; b = 3; q = a / b; p = b * q;
    ASSERT (p == a);
  }
}

int
main ()
{
  /* Check that the default rounding direction is FE_TONEAREST.  */
  test_tonearest ();

/* On alpha, this test works only when compiled with the GCC option
   '-mfp-rounding-mode=d'.  */
#if !((defined __arm__ && defined __SOFTFP__) || defined __alpha)
# if defined FE_TOWARDZERO
  ASSERT (fesetround (FE_TOWARDZERO) == 0);
  test_towardzero ();
  ASSERT (fegetround () == FE_TOWARDZERO);
# endif

# if defined FE_UPWARD
  ASSERT (fesetround (FE_UPWARD) == 0);
  test_upward ();
  ASSERT (fegetround () == FE_UPWARD);
# endif

# if defined FE_DOWNWARD
  ASSERT (fesetround (FE_DOWNWARD) == 0);
  test_downward ();
  ASSERT (fegetround () == FE_DOWNWARD);
# endif
#endif

  ASSERT (fesetround (FE_TONEAREST) == 0);
  test_tonearest ();
  ASSERT (fegetround () == FE_TONEAREST);

  return test_exit_status;
}
