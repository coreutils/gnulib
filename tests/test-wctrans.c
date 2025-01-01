/* Test of getting descriptor for a wide character mapping.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include <wctype.h>

#include "signature.h"
SIGNATURE_CHECK (wctrans, wctrans_t, (const char *));

#include "macros.h"

int
main (int argc, char *argv[])
{
  wctrans_t desc;

  desc = wctrans ("any");
  ASSERT (desc == (wctrans_t) 0);

  desc = wctrans ("tolower");
  ASSERT (desc != (wctrans_t) 0);

  desc = wctrans ("toupper");
  ASSERT (desc != (wctrans_t) 0);

  return test_exit_status;
}
