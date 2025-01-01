/* Test of wide character mappings.
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
SIGNATURE_CHECK (towctrans, wint_t, (wint_t, wctrans_t));

#include "macros.h"

int
main (int argc, char *argv[])
{
  wctrans_t desc;

  desc = wctrans ("any");
  ASSERT (desc == (wctrans_t) 0);

  desc = wctrans ("tolower");
  ASSERT (desc != (wctrans_t) 0);
  ASSERT (towctrans (L'a', desc) == 'a');
  ASSERT (towctrans (L'z', desc) == 'z');
  ASSERT (towctrans (L'A', desc) == 'a');
  ASSERT (towctrans (L'Z', desc) == 'z');
  ASSERT (towctrans (L'1', desc) == '1');
  ASSERT (towctrans (L'_', desc) == '_');
  ASSERT (towctrans (L'_', desc) == '_');
  ASSERT (towctrans ((wchar_t) 0, desc) == 0);
  ASSERT (towctrans (WEOF, desc) == WEOF);

  desc = wctrans ("toupper");
  ASSERT (desc != (wctrans_t) 0);
  ASSERT (towctrans (L'A', desc) == 'A');
  ASSERT (towctrans (L'Z', desc) == 'Z');
  ASSERT (towctrans (L'a', desc) == 'A');
  ASSERT (towctrans (L'z', desc) == 'Z');
  ASSERT (towctrans (L'1', desc) == '1');
  ASSERT (towctrans (L'_', desc) == '_');
  ASSERT (towctrans ((wchar_t) 0, desc) == 0);
  ASSERT (towctrans (WEOF, desc) == WEOF);

  return test_exit_status;
}
