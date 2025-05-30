/* Test of u32_cmp() function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2010.  */

#include <config.h>

#include "unistr.h"

#include "zerosize-ptr.h"
#include "macros.h"

#define UNIT uint32_t
#define U_CMP u32_cmp
#define MAGIC 0xBADFACE
#include "test-cmp.h"

int
main ()
{
  test_cmp ();

  /* Test comparison with non-BMP characters.  */
  {
    static const UNIT input1[] = { 0x1D51F };
    static const UNIT input2[] = { 0xFEFF };
    ASSERT (U_CMP (input1, input2, 1) > 0);
    ASSERT (U_CMP (input2, input1, 1) < 0);
  }

  return test_exit_status;
}
