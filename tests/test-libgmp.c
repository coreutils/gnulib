/* Test of libgmp or its mini-gmp substitute.
   Copyright (C) 2020 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include <gmp.h>

#include "macros.h"

int
main ()
{
  /* A simple sanity check that 2 + 2 = 4.  */
  static mp_limb_t const twobody[] = { 2 };
  static mpz_t const two = MPZ_ROINIT_N ((mp_limb_t *) twobody, 1);
  ASSERT (mpz_fits_slong_p (two));
  ASSERT (mpz_get_si (two) == 2);

  mpz_t four;
  mpz_init (four);
  mpz_add (four, two, two);
  ASSERT (mpz_fits_slong_p (four));
  ASSERT (mpz_get_si (four) == 4);
  mpz_clear (four);

  return 0;
}
