/* Test of isnanl() substitute.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2007.  */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
        {								     \
          fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
          abort ();							     \
        }								     \
    }									     \
  while (0)

int
main ()
{
  /* Finite values.  */
  ASSERT (!isnanl (3.141L));
  ASSERT (!isnanl (3.141e30L));
  ASSERT (!isnanl (3.141e-30L));
  ASSERT (!isnanl (-2.718L));
  ASSERT (!isnanl (-2.718e30L));
  ASSERT (!isnanl (-2.718e-30L));
  /* Infinite values.  */
  ASSERT (!isnanl (1.0L / 0.0L));
  ASSERT (!isnanl (-1.0L / 0.0L));
  /* Quiet NaN.  */
  ASSERT (isnanl (0.0L / 0.0L));
#if defined LDBL_EXPBIT0_WORD && defined LDBL_EXPBIT0_BIT
  /* Signalling NaN.  */
  {
    #define NWORDS \
      ((sizeof (long double) + sizeof (unsigned int) - 1) / sizeof (unsigned int))
    typedef union { long double value; unsigned int word[NWORDS]; }
            memory_long_double;
    memory_long_double m;
    m.value = 0.0L / 0.0L;
# if LDBL_EXPBIT0_BIT > 0
    m.word[LDBL_EXPBIT0_WORD] ^= (unsigned int) 1 << (LDBL_EXPBIT0_BIT - 1);
# else
    m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      ^= (unsigned int) 1 << (sizeof (unsigned int) * CHAR_BIT - 1);
# endif
    m.word[LDBL_EXPBIT0_WORD + (LDBL_EXPBIT0_WORD < NWORDS / 2 ? 1 : - 1)]
      |= (unsigned int) 1 << LDBL_EXPBIT0_BIT;
    ASSERT (isnanl (m.value));
  }
#endif
  return 0;
}
