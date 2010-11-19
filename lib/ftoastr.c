/* floating point to accurate string

   Copyright (C) 2010 Free Software Foundation, Inc.

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

#include "ftoastr.h"

#include "intprops.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#if LENGTH == 3
# define FLOAT long double
# define FLOAT_DIG LDBL_DIG
# define FLOAT_MIN LDBL_MIN
# define FLOAT_PREC_BOUND _GL_LDBL_PREC_BOUND
# define FTOASTR ldtoastr
# define STRTOF strtold
#elif LENGTH == 2
# define FLOAT double
# define FLOAT_DIG DBL_DIG
# define FLOAT_MIN DBL_MIN
# define FLOAT_PREC_BOUND _GL_DBL_PREC_BOUND
# define FTOASTR dtoastr
# define STRTOF strtod
#else
# define LENGTH 1
# define FLOAT float
# define FLOAT_DIG FLT_DIG
# define FLOAT_MIN FLT_MIN
# define FLOAT_PREC_BOUND _GL_FLT_PREC_BOUND
# define FTOASTR ftoastr
# define STRTOF strtof
#endif

/* On pre-C99 hosts, approximate strtof and strtold with strtod.  This
   may generate one or two extra digits, but that's better than not
   working at all.  Assume that strtof works if strtold does.  */
#if LENGTH != 2 && ! HAVE_C99_STRTOLD
# undef STRTOF
# define STRTOF strtod
#endif

int
FTOASTR (char *buf, size_t bufsize, int flags, int width, FLOAT x)
{
  /* The following method is simple but slow.
     For ideas about speeding things up, please see:

     Florian Loitsch, Printing floating-point numbers quickly and accurately
     with integers.  ACM SIGPLAN notices 46, 6 (June 2010), 233-243
     <http://dx.doi.org/10.1145/1809028.1806623>.  */

  char format[sizeof "%-+ 0*.*Lg"];
  FLOAT abs_x = x < 0 ? -x : x;
  int prec;

  char *p = format;
  *p++ = '%';

  /* Support flags that generate output parsable by strtof.  */
  *p = '-'; p += (flags & FTOASTR_LEFT_JUSTIFY  ) != 0;
  *p = '+'; p += (flags & FTOASTR_ALWAYS_SIGNED ) != 0;
  *p = ' '; p += (flags & FTOASTR_SPACE_POSITIVE) != 0;
  *p = '0'; p += (flags & FTOASTR_ZERO_PAD      ) != 0;

  *p++ = '*';
  *p++ = '.';
  *p++ = '*';
  *p = 'L'; p += 2 < LENGTH;
  *p++ = flags & FTOASTR_UPPER_E ? 'G' : 'g';
  *p = '\0';

  for (prec = abs_x < FLOAT_MIN ? 1 : FLOAT_DIG; ; prec++)
    {
      int n = snprintf (buf, bufsize, format, width, prec, x);
      if (n < 0
          || FLOAT_PREC_BOUND <= prec
          || (n < bufsize && STRTOF (buf, NULL) == x))
        return n;
    }
}
