/* Test of getting descriptor for a 32-bit wide character mapping.
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

#include <uchar.h>

#include "signature.h"
SIGNATURE_CHECK (c32_get_mapping, c32_mapping_t, (const char *));

#include "macros.h"

int
main (int argc, char *argv[])
{
  c32_mapping_t desc;

  desc = c32_get_mapping ("any");
  ASSERT (desc == (c32_mapping_t) 0);

  desc = c32_get_mapping ("tolower");
  ASSERT (desc != (c32_mapping_t) 0);

  desc = c32_get_mapping ("toupper");
  ASSERT (desc != (c32_mapping_t) 0);

  return test_exit_status;
}
