#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "verify.h"

int x;
enum { a, b, c };

verify (x >= 0);                  /* should give ERROR: non-constant expression */
verify (c == 2);                  /* should be ok */
verify (1 + 1 == 3);              /* should give ERROR */
verify (1 == 1); verify (1 == 1); /* should be ok */

int function (int n)
{
  verify (n >= 0);                  /* should give ERROR: non-constant expression */
  verify (c == 2);                  /* should be ok */
  verify (1 + 1 == 3);              /* should give ERROR */
  verify (1 == 1); verify (1 == 1); /* should be ok */

  if (n)
    return (verify_expr (1 == 1), verify_expr (1 == 1), 7); /* should be ok */
  else
    return (verify_expr (1 == 2), 5); /* should give ERROR */
}
