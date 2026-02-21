/* Test of strnul() function.
   Copyright (C) 2026 Free Software Foundation, Inc.

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
#include <string.h>

#include "macros.h"

int
main ()
{
  const char ro[] = "foo";
  char rw[] = "foo";

  const char *ro_nul = strnul (ro);
  char *rw_nul = strnul (rw);

  ASSERT (ro_nul - ro == 3);
  ASSERT (rw_nul - rw == 3);

  return test_exit_status;
}
