/* Test of <bitrotate.h> substitute.
   Copyright (C) 2007-2008 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson <simon@josefsson.org>, 2008.  */

#include <config.h>

#include "bitrotate.h"

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr)							\
  do									\
    {									\
      if (!(expr))							\
	{								\
	  fprintf (stderr, "%s:%d: assertion failed\n",			\
		   __FILE__, __LINE__);					\
	  fflush (stderr);						\
	  abort ();							\
	}								\
    }									\
  while (0)

int
main (void)
{
  ASSERT (rotl16 (43981, 1) == 22427);
  ASSERT (rotl16 (43981, 2) == 44854);
  ASSERT (rotl16 (43981, 3) == 24173);
  ASSERT (rotl16 (43981, 4) == 48346);
  ASSERT (rotl16 (43981, 5) == 31157);
  ASSERT (rotl16 (43981, 6) == 62314);
  ASSERT (rotl16 (43981, 7) == 59093);
  ASSERT (rotl16 (43981, 8) == 52651);
  ASSERT (rotl16 (43981, 9) == 39767);
  ASSERT (rotl16 (43981, 10) == 13999);
  ASSERT (rotl16 (43981, 11) == 27998);
  ASSERT (rotl16 (43981, 12) == 55996);
  ASSERT (rotl16 (43981, 13) == 46457);
  ASSERT (rotl16 (43981, 14) == 27379);
  ASSERT (rotl16 (43981, 15) == 54758);

  ASSERT (rotl32 (2309737967U, 1) == 324508639U);
  ASSERT (rotl32 (2309737967U, 2) == 649017278U);
  ASSERT (rotl32 (2309737967U, 3) == 1298034556U);
  ASSERT (rotl32 (2309737967U, 4) == 2596069112U);
  ASSERT (rotl32 (2309737967U, 5) == 897170929U);
  ASSERT (rotl32 (2309737967U, 6) == 1794341858U);
  ASSERT (rotl32 (2309737967U, 7) == 3588683716U);
  ASSERT (rotl32 (2309737967U, 8) == 2882400137U);
  ASSERT (rotl32 (2309737967U, 9) == 1469832979U);
  ASSERT (rotl32 (2309737967U, 10) == 2939665958U);
  ASSERT (rotl32 (2309737967U, 11) == 1584364621U);
  ASSERT (rotl32 (2309737967U, 12) == 3168729242U);
  ASSERT (rotl32 (2309737967U, 13) == 2042491189U);
  ASSERT (rotl32 (2309737967U, 14) == 4084982378U);
  ASSERT (rotl32 (2309737967U, 15) == 3874997461U);
  ASSERT (rotl32 (2309737967U, 16) == 3455027627U);
  ASSERT (rotl32 (2309737967U, 17) == 2615087959U);
  ASSERT (rotl32 (2309737967U, 18) == 935208623U);
  ASSERT (rotl32 (2309737967U, 19) == 1870417246U);
  ASSERT (rotl32 (2309737967U, 20) == 3740834492U);
  ASSERT (rotl32 (2309737967U, 21) == 3186701689U);
  ASSERT (rotl32 (2309737967U, 22) == 2078436083U);
  ASSERT (rotl32 (2309737967U, 23) == 4156872166U);
  ASSERT (rotl32 (2309737967U, 24) == 4018777037U);
  ASSERT (rotl32 (2309737967U, 25) == 3742586779U);
  ASSERT (rotl32 (2309737967U, 26) == 3190206263U);
  ASSERT (rotl32 (2309737967U, 27) == 2085445231U);
  ASSERT (rotl32 (2309737967U, 28) == 4170890462U);
  ASSERT (rotl32 (2309737967U, 29) == 4046813629U);
  ASSERT (rotl32 (2309737967U, 30) == 3798659963U);
  ASSERT (rotl32 (2309737967U, 31) == 3302352631U);

  return 0;
}
