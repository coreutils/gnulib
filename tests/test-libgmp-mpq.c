/* Test of libgmp or its mini-mpq substitute.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

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
#include <gmp.h>

#include <limits.h>
#include <string.h>

#include "macros.h"

int
main ()
{
  /* A simple sanity check that 2/3 + 2/3 = 4/3.  */
  static mp_limb_t const twobody[] = { 2 };
  static mp_limb_t const threebody[] = { 3 };
  static mpz_t const two = MPZ_ROINIT_N ((mp_limb_t *) twobody, 1);
  static mpz_t const three = MPZ_ROINIT_N ((mp_limb_t *) threebody, 1);
  ASSERT (mpz_fits_slong_p (two));
  ASSERT (mpz_get_si (two) == 2);
  ASSERT (mpz_fits_slong_p (three));
  ASSERT (mpz_get_si (three) == 3);

  mpq_t q;
  mpq_init (q);
  mpz_set (mpq_numref (q), two);
  mpz_set (mpq_denref (q), three);
  mpq_add (q, q, q);
  ASSERT (mpz_fits_slong_p (mpq_numref (q)));
  ASSERT (mpz_get_si (mpq_numref (q)) == 4);
  ASSERT (mpz_fits_slong_p (mpq_denref (q)));
  ASSERT (mpz_get_si (mpq_denref (q)) == 3);
  mpq_clear (q);

  return test_exit_status;
}
