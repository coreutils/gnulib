/* Test of mbs_endswith() function.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

#include <string.h>

#include <stdlib.h>

#include "macros.h"

int
main ()
{
  /* This test is executed in the C locale.  */

  ASSERT (mbs_endswith ("", ""));
  ASSERT (mbs_endswith ("abc", ""));

  ASSERT (!mbs_endswith ("", "c"));
  ASSERT (!mbs_endswith ("x", "c"));
  ASSERT (mbs_endswith ("c", "c"));
  ASSERT (mbs_endswith ("abc", "c"));

  ASSERT (!mbs_endswith ("", "xyz"));
  ASSERT (!mbs_endswith ("x", "xyz"));
  ASSERT (!mbs_endswith ("a", "xyz"));
  ASSERT (!mbs_endswith ("abc", "xyz"));
  ASSERT (mbs_endswith ("xyz", "xyz"));
  ASSERT (mbs_endswith ("yxxyz", "xyz"));

  return test_exit_status;
}
