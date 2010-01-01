/* Test the "verify" module.

   Copyright (C) 2005, 2009, 2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible.  */

#include <config.h>

#include "verify.h"

int x;
enum { a, b, c };

verify (x >= 0);                  /* should give ERROR: non-constant expression */
verify (c == 2);                  /* should be ok */
verify (1 + 1 == 3);              /* should give ERROR */
verify (1 == 1); verify (1 == 1); /* should be ok */

enum
{
  item = verify_true (1 == 1) * 0 + 17 /* should be ok */
};

int function (int n)
{
  verify (n >= 0);                  /* should give ERROR: non-constant expression */
  verify (c == 2);                  /* should be ok */
  verify (1 + 1 == 3);              /* should give ERROR */
  verify (1 == 1); verify (1 == 1); /* should be ok */

  if (n)
    return (verify_true (1 == 1), verify_true (1 == 1), 7); /* should be ok */
  else
    return (verify_true (1 == 2), 5); /* should give ERROR */
}
