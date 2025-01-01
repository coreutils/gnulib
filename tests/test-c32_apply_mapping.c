/* Test of 32-bit wide character mappings.
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
SIGNATURE_CHECK (c32_apply_mapping, wint_t, (wint_t, c32_mapping_t));

#include "macros.h"

int
main (int argc, char *argv[])
{
  c32_mapping_t desc;

  desc = c32_get_mapping ("any");
  ASSERT (desc == (c32_mapping_t) 0);

  desc = c32_get_mapping ("tolower");
  ASSERT (desc != (c32_mapping_t) 0);
  ASSERT (c32_apply_mapping ((char32_t) 'a', desc) == 'a');
  ASSERT (c32_apply_mapping ((char32_t) 'z', desc) == 'z');
  ASSERT (c32_apply_mapping ((char32_t) 'A', desc) == 'a');
  ASSERT (c32_apply_mapping ((char32_t) 'Z', desc) == 'z');
  ASSERT (c32_apply_mapping ((char32_t) '1', desc) == '1');
  ASSERT (c32_apply_mapping ((char32_t) '_', desc) == '_');
  ASSERT (c32_apply_mapping ((char32_t) '_', desc) == '_');
  ASSERT (c32_apply_mapping ((char32_t) 0, desc) == 0);
  ASSERT (c32_apply_mapping (WEOF, desc) == WEOF);

  desc = c32_get_mapping ("toupper");
  ASSERT (desc != (c32_mapping_t) 0);
  ASSERT (c32_apply_mapping ((char32_t) 'A', desc) == 'A');
  ASSERT (c32_apply_mapping ((char32_t) 'Z', desc) == 'Z');
  ASSERT (c32_apply_mapping ((char32_t) 'a', desc) == 'A');
  ASSERT (c32_apply_mapping ((char32_t) 'z', desc) == 'Z');
  ASSERT (c32_apply_mapping ((char32_t) '1', desc) == '1');
  ASSERT (c32_apply_mapping ((char32_t) '_', desc) == '_');
  ASSERT (c32_apply_mapping ((char32_t) 0, desc) == 0);
  ASSERT (c32_apply_mapping (WEOF, desc) == WEOF);

  return test_exit_status;
}
