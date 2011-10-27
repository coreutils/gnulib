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
#include <stdlib.h>

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
# define DECLARE_ALIGNED(type, name) \
    type alignas (1 << 3) name##_alignas; \
    type _Alignas (1 << 3) name##_Alignas;
# define CHECK_ALIGNED(name) \
    (((uintptr_t) &name##_alignas % (1 << 3) ? abort () : (void) 0), \
     ((uintptr_t) &name##_Alignas % (1 << 3) ? abort () : (void) 0))
#else
# define DECLARE_ALIGNED(type, name)
# define CHECK_ALIGNED(name) ((void) 0)
#endif

#define CHECK_STATIC(type) \
  typedef struct { char slot1; type slot2; } type##_helper; \
  verify (alignof (type) == offsetof (type##_helper, slot2)); \
  verify (_Alignof (type) == alignof (type)); \
  const int type##_alignment = alignof (type); \
  DECLARE_ALIGNED(type, static_##type)

#define CHECK_AUTO(type) \
  { \
    DECLARE_ALIGNED(type, auto_##type) \
    CHECK_ALIGNED(static_##type); \
    CHECK_ALIGNED(auto_##type); \
  }

#ifdef INT64_MAX
# define if_INT64_MAX(x) x
#else
# define if_INT64_MAX(x)
#endif

#define CHECK_TYPES(check) \
  check (char) \
  check (short) \
  check (int) \
  check (long) \
  if_INT64_MAX (check (int64_t)) \
  check (float) \
  check (double) \
  check (longdouble) \
  check (struct1) \
  check (struct2) \
  check (struct3) \
  check (struct4)

CHECK_TYPES (CHECK_STATIC)

int
main ()
{
  CHECK_TYPES (CHECK_AUTO)
  return 0;
}
