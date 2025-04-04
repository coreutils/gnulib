/* Test of iswblank() function.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

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

#include <wctype.h>

#include "macros.h"

/* Check that WEOF is defined.  */
wint_t e = WEOF;

int
main (void)
{
  /* Check that the function exist as a function or as a macro.  */
  (void) iswblank (0);
  /* Check that the isw* functions map WEOF to 0.  */
  ASSERT (!iswblank (e));

  /* Sanity check for the iswblank function.  */
  ASSERT (iswblank (L' '));
  ASSERT (iswblank (L'\t'));
  ASSERT (!iswblank (L'\n'));

  return test_exit_status;
}
