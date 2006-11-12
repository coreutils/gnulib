#include <assert.h>
#include "i-ring.h"
/* Test with this:
   gcc -I. -Wall -W -O i-ring-test.c i-ring.c -L. -lcoreutils && ./a.out
*/

int
main ()
{
  I_ring ir;
  i_ring_init (&ir, -1);
  int o = i_ring_push (&ir, 1);
  assert (o == -1);
  o = i_ring_push (&ir, 2);
  assert (o == -1);
  o = i_ring_push (&ir, 3);
  assert (o == -1);
  o = i_ring_push (&ir, 4);
  assert (o == -1);
  o = i_ring_push (&ir, 5);
  assert (o == 1);
  o = i_ring_push (&ir, 6);
  assert (o == 2);
  o = i_ring_push (&ir, 7);
  assert (o == 3);

  o = i_ring_pop (&ir);
  assert (o == 7);
  o = i_ring_pop (&ir);
  assert (o == 6);
  o = i_ring_pop (&ir);
  assert (o == 5);
  o = i_ring_pop (&ir);
  assert (o == 4);
  assert (i_ring_empty (&ir));

  o = i_ring_push (&ir, 8);
  assert (o == -1);
  o = i_ring_pop (&ir);
  assert (o == 8);
  assert (i_ring_empty (&ir));

  return 0;
}
