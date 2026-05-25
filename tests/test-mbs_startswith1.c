/* Test of mbs_startswith() function in the "C" locale.
   Copyright (C) 2025-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

#include <string.h>

#include <stdlib.h>

#include "macros.h"

int
main ()
{
  /* This test is executed in the "C" locale.  */

  ASSERT (mbs_startswith ("", ""));
  ASSERT (mbs_startswith ("abc", ""));

  ASSERT (!mbs_startswith ("", "a"));
  ASSERT (!mbs_startswith ("x", "a"));
  ASSERT (mbs_startswith ("a", "a"));
  ASSERT (mbs_startswith ("abc", "a"));

  ASSERT (!mbs_startswith ("", "xyz"));
  ASSERT (!mbs_startswith ("x", "xyz"));
  ASSERT (!mbs_startswith ("a", "xyz"));
  ASSERT (!mbs_startswith ("abc", "xyz"));
  ASSERT (mbs_startswith ("xyz", "xyz"));
  ASSERT (mbs_startswith ("xyzzy", "xyz"));

  return test_exit_status;
}
