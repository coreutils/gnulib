/* Test of xstrtod() in the "C" locale.
   Copyright (C) 2008-2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include "xstrtod.h"

#include <errno.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include "isnand-nolibm.h"
#include "minus-zero.h"
#include "macros.h"

#define UNINIT 0.2614972128476428

int
main ()
{
  /* Subject sequence empty or invalid.  */
  {
    const char input[] = "";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " ";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " +";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " .";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " .e0";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " +.e-0";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " in";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }
  {
    const char input[] = " na";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (ptr == input);
      ASSERT (errno == 0 || errno == EINVAL);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 0.0);
      ASSERT (!signbit (result));
      ASSERT (errno == 0 || errno == EINVAL);
    }
  }

  /* Simple floating point values.  */
  {
    const char input[] = "1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1.";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = ".5";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.5);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = " 1";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (ptr == input + 2);
      ASSERT (errno == 0);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (errno == 0);
    }
  }
  {
    const char input[] = "+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "-1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == -1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1e0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1e+0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1e-0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1e1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "5e-1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.5);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }

  /* Zero.  */
  {
    const char input[] = "0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = ".0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0e0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0e+9999999";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 10);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0e-9999999";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 10);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "-0";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!!signbit (result) == !!signbit (minus_zerod));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }

  /* Suffixes.  */
  {
    const char input[] = "1f";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1.f";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1e";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (ptr == input + 1);
      ASSERT (errno == 0);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 1.0);
      ASSERT (errno == 0);
    }
  }
  {
    const char input[] = "1e+";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (ptr == input + 1);
      ASSERT (errno == 0);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 1.0);
      ASSERT (errno == 0);
    }
  }
  {
    const char input[] = "1e-";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (ptr == input + 1);
      ASSERT (errno == 0);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 1.0);
      ASSERT (errno == 0);
    }
  }
  {
    const char input[] = "1E 2";
    {
      const char *ptr;
      double result = UNINIT;
      bool ok = xstrtod (input, &ptr, &result, strtod);
      ASSERT (ok);
      ASSERT (result == 1.0);
      ASSERT (ptr == input + 1);
      ASSERT (errno == 0);
    }
    {
      double result = UNINIT;
      bool ok = xstrtod (input, NULL, &result, strtod);
      ASSERT (!ok);
      ASSERT (result == 1.0);
      ASSERT (errno == 0);
    }
  }
  {
    const char input[] = "0x";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "00x1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "-0x";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!!signbit (result) == !!signbit (minus_zerod));
    ASSERT (ptr == input + 2);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0xg";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0xp";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0XP";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x.";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0xp+";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0xp+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x.p+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1p+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "1P+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }

  /* Overflow.  */
  {
    const char input[] = "1e500";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (!ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 5);
    ASSERT (errno == ERANGE);
  }
  {
    const char input[] = "1E1000000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (!ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 9);
    ASSERT (errno == ERANGE);
  }
  {
    const char input[] = "-1E1000000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (!ok);
    ASSERT (result == -HUGE_VAL);
    ASSERT (ptr == input + 10);
    ASSERT (errno == ERANGE);
  }

  /* Gradual underflow, resulting in a denormalized number.  */
  {
    const char input[] = "1e-320";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (0.0 < result && result <= DBL_MIN);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "-1e-320";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (-DBL_MIN <= result && result < 0.0);
    ASSERT (ptr == input + 7);
    ASSERT (errno == 0);
  }

  /* Flush-to-zero underflow.  */
  {
    const char input[] = "1E-100000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input + 9);
    ASSERT (errno == ERANGE);
  }
  {
    const char input[] = "-1E-100000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 0.0);
    /* Negative underflow.  Expect a negative sign, although POSIX allows +0.0.
       See also <https://sourceware.org/PR5995>.  */
    ASSERT (!!signbit (result) == !!signbit (minus_zerod));
    ASSERT (ptr == input + 10);
    ASSERT (errno == ERANGE);
  }

  /* Space before the exponent.  */
  {
    const char input[] = "1E 1000000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 1);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1P 1000000";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }

  /* Infinity.  */
  {
    const char input[] = "iNf";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "-InF";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == -HUGE_VAL);
    ASSERT (ptr == input + 4);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "infinite";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "infinitY";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 8);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "infinitY.";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == HUGE_VAL);
    ASSERT (ptr == input + 8);
    ASSERT (errno == 0);
  }

  /* NaN.  Some processors set the sign bit of the default NaN, so all
     we check is that using a sign changes the result.  */
  {
    const char input[] = "-nan";
    const char *ptr1;
    const char *ptr2;
    double result1 = UNINIT;
    double result2 = UNINIT;
    bool ok1 = xstrtod (input, &ptr1, &result1, strtod);
    bool ok2 = xstrtod (input + 1, &ptr2, &result2, strtod);
    ASSERT (ok1);
    ASSERT (ok2);
#if 1 /* All known CPUs support NaNs.  */
    ASSERT (isnand (result1));
    ASSERT (isnand (result2));
# if 0
    /* Sign bits of NaN is a portability sticking point, not worth
       worrying about.  */
    ASSERT (!!signbit (result1) != !!signbit (result2));
# endif
    ASSERT (ptr1 == input + 4);
    ASSERT (ptr2 == input + 4);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    const char input[] = "+nan(";
    const char *ptr1;
    const char *ptr2;
    double result1 = UNINIT;
    double result2 = UNINIT;
    bool ok1 = xstrtod (input, &ptr1, &result1, strtod);
    bool ok2 = xstrtod (input + 1, &ptr2, &result2, strtod);
    ASSERT (ok1);
    ASSERT (ok2);
#if 1 /* All known CPUs support NaNs.  */
    ASSERT (isnand (result1));
    ASSERT (isnand (result2));
    ASSERT (!!signbit (result1) == !!signbit (result2));
    ASSERT (ptr1 == input + 4);
    ASSERT (ptr2 == input + 4);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    const char input[] = "-nan()";
    const char *ptr1;
    const char *ptr2;
    double result1 = UNINIT;
    double result2 = UNINIT;
    bool ok1 = xstrtod (input, &ptr1, &result1, strtod);
    bool ok2 = xstrtod (input + 1, &ptr2, &result2, strtod);
    ASSERT (ok1);
    ASSERT (ok2);
#if 1 /* All known CPUs support NaNs.  */
    ASSERT (isnand (result1));
    ASSERT (isnand (result2));
# if 0
    /* Sign bits of NaN is a portability sticking point, not worth
       worrying about.  */
    ASSERT (!!signbit (result1) != !!signbit (result2));
# endif
    ASSERT (ptr1 == input + 6);
    ASSERT (ptr2 == input + 6);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    const char input[] = " nan().";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
#if 1 /* All known CPUs support NaNs.  */
    ASSERT (isnand (result));
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
#else
    ASSERT (result == 0.0);
    ASSERT (!signbit (result));
    ASSERT (ptr == input);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }
  {
    /* The behavior of nan(0) is implementation-defined, but all
       implementations we know of which handle optional
       n-char-sequences handle nan(0) the same as nan().  */
    const char input[] = "-nan(0).";
    const char *ptr1;
    const char *ptr2;
    double result1 = UNINIT;
    double result2 = UNINIT;
    bool ok1 = xstrtod (input, &ptr1, &result1, strtod);
    bool ok2 = xstrtod (input + 1, &ptr2, &result2, strtod);
    ASSERT (ok1);
    ASSERT (ok2);
#if 1 /* All known CPUs support NaNs.  */
    ASSERT (isnand (result1));
    ASSERT (isnand (result2));
# if 0
    /* Sign bits of NaN is a portability sticking point, not worth
       worrying about.  */
    ASSERT (!!signbit (result1) != !!signbit (result2));
# endif
    ASSERT (ptr1 == input + 7);
    ASSERT (ptr2 == input + 7);
    ASSERT (errno == 0);
#else
    ASSERT (result1 == 0.0);
    ASSERT (result2 == 0.0);
    ASSERT (!signbit (result1));
    ASSERT (!signbit (result2));
    ASSERT (ptr1 == input);
    ASSERT (ptr2 == input + 1);
    ASSERT (errno == 0 || errno == EINVAL);
#endif
  }

  /* Hex.  */
  {
    const char input[] = "0xa";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0XA";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 10.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1p";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1p+";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1P+";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1p+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 2.0);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0X1P+1";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 2.0);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1p+1a";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 2.0);
    ASSERT (ptr == input + 6);
    ASSERT (errno == 0);
  }
  {
    const char input[] = "0x1p 2";
    const char *ptr;
    double result = UNINIT;
    bool ok = xstrtod (input, &ptr, &result, strtod);
    ASSERT (ok);
    ASSERT (result == 1.0);
    ASSERT (ptr == input + 3);
    ASSERT (errno == 0);
  }

  /* Large buffers.  */
  {
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
        const char *ptr;
        double result = UNINIT;
        memset (input, '\t', m - 1);
        input[m - 1] = '1';
        input[m] = '\0';
        bool ok = xstrtod (input, &ptr, &result, strtod);
        ASSERT (ok);
        ASSERT (result == 1.0);
        ASSERT (ptr == input + m);
        ASSERT (errno == 0);
      }
    free (input);
  }
  {
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
        const char *ptr;
        double result = UNINIT;
        memset (input, '0', m - 1);
        input[m - 1] = '1';
        input[m] = '\0';
        bool ok = xstrtod (input, &ptr, &result, strtod);
        ASSERT (ok);
        ASSERT (result == 1.0);
        ASSERT (ptr == input + m);
        ASSERT (errno == 0);
      }
    free (input);
  }
#if 0
  /* Newlib has an artificial limit of 20000 for the exponent.  TODO -
     gnulib should fix this.  */
  {
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
        const char *ptr;
        double result = UNINIT;
        input[0] = '.';
        memset (input + 1, '0', m - 10);
        input[m - 9] = '1';
        input[m - 8] = 'e';
        input[m - 7] = '+';
        input[m - 6] = '9';
        input[m - 5] = '9';
        input[m - 4] = '9';
        input[m - 3] = '9';
        input[m - 2] = '9';
        input[m - 1] = '1';
        input[m] = '\0';
        bool ok = xstrtod (input, &ptr, &result, strtod);
        ASSERT (ok);
        ASSERT (result == 1.0);
        ASSERT (ptr == input + m);
        ASSERT (errno == 0);
      }
    free (input);
  }
  {
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
        const char *ptr;
        double result = UNINIT;
        input[0] = '1';
        memset (input + 1, '0', m - 9);
        input[m - 8] = 'e';
        input[m - 7] = '-';
        input[m - 6] = '9';
        input[m - 5] = '9';
        input[m - 4] = '9';
        input[m - 3] = '9';
        input[m - 2] = '9';
        input[m - 1] = '1';
        input[m] = '\0';
        bool ok = xstrtod (input, &ptr, &result, strtod);
        ASSERT (ok);
        ASSERT (result == 1.0);
        ASSERT (ptr == input + m);
        ASSERT (errno == 0);
      }
    free (input);
  }
#endif
  {
    size_t m = 1000000;
    char *input = malloc (m + 1);
    if (input)
      {
        const char *ptr;
        double result = UNINIT;
        input[0] = '-';
        input[1] = '0';
        input[2] = 'e';
        input[3] = '1';
        memset (input + 4, '0', m - 3);
        input[m] = '\0';
        bool ok = xstrtod (input, &ptr, &result, strtod);
        ASSERT (ok);
        ASSERT (result == 0.0);
        ASSERT (!!signbit (result) == !!signbit (minus_zerod));
        ASSERT (ptr == input + m);
        ASSERT (errno == 0);
      }
    free (input);
  }

  return test_exit_status;
}
