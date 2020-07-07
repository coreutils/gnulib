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
