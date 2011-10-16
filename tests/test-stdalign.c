/* Test of <stdalign.h>.
   Copyright 2009-2011 Free Software Foundation, Inc.

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

/* Written by Paul Eggert, inspired by Bruno Haible's test-alignof.c.  */

#include <config.h>

#include <stdalign.h>

#include <stddef.h>
#include <stdint.h>

#include "verify.h"

typedef long double longdouble;
typedef struct { char a[1]; } struct1;
typedef struct { char a[2]; } struct2;
typedef struct { char a[3]; } struct3;
typedef struct { char a[4]; } struct4;

verify (__alignof_is_defined == 1);
#ifndef alignof
# error "alignof is not a macro"
#endif

#if __alignas_is_defined
verify (__alignas_is_defined == 1);
# ifndef alignas
#  error "alignas is not a macro"
# endif
# define CHECK_ALIGNAS(type) \
    type alignas (1 << 3) type##_alignas; \
    type _Alignas (1 << 3) type##_Alignas;
#else
# define CHECK_ALIGNAS(type)
#endif

#define CHECK(type) \
  typedef struct { char slot1; type slot2; } type##_helper; \
  verify (alignof (type) == offsetof (type##_helper, slot2)); \
  verify (_Alignof (type) == alignof (type)); \
  const int type##_alignment = alignof (type); \
  CHECK_ALIGNAS(type)

CHECK (char)
CHECK (short)
CHECK (int)
CHECK (long)
CHECK (float)
CHECK (double)
CHECK (longdouble)
#ifdef INT64_MAX
CHECK (int64_t)
#endif
CHECK (struct1)
CHECK (struct2)
CHECK (struct3)
CHECK (struct4)

int
main ()
{
  return 0;
}
