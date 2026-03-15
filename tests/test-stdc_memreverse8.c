/* Test of stdc_memreverse8 function.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

/* Specification.  */
#include <stdbit.h>

#include "macros.h"

static unsigned char data[10] =
  { 0x12, 0x31, 0x47, 0xBF, 0xC4, 0xD3, 0xEE, 0xF1, 0x90, 0x44 };

static unsigned char *volatile ptr;

int
main (void)
{
  ptr = data;

  stdc_memreverse8 (0, ptr + 5);
  /* Expect: 0x12, 0x31, 0x47, 0xBF, 0xC4, 0xD3, 0xEE, 0xF1, 0x90, 0x44  */
  ASSERT (ptr[4] == 0xC4);
  ASSERT (ptr[5] == 0xD3);

  stdc_memreverse8 (1, ptr + 5);
  /* Expect: 0x12, 0x31, 0x47, 0xBF, 0xC4, 0xD3, 0xEE, 0xF1, 0x90, 0x44  */
  ASSERT (ptr[4] == 0xC4);
  ASSERT (ptr[5] == 0xD3);
  ASSERT (ptr[6] == 0xEE);

  stdc_memreverse8 (2, ptr + 5);
  /* Expect: 0x12, 0x31, 0x47, 0xBF, 0xC4, 0xEE, 0xD3, 0xF1, 0x90, 0x44  */
  ASSERT (ptr[4] == 0xC4);
  ASSERT (ptr[5] == 0xEE);
  ASSERT (ptr[6] == 0xD3);
  ASSERT (ptr[7] == 0xF1);

  stdc_memreverse8 (4, ptr + 3);
  /* Expect: 0x12, 0x31, 0x47, 0xD3, 0xEE, 0xC4, 0xBF, 0xF1, 0x90, 0x44  */
  ASSERT (ptr[2] == 0x47);
  ASSERT (ptr[3] == 0xD3);
  ASSERT (ptr[4] == 0xEE);
  ASSERT (ptr[5] == 0xC4);
  ASSERT (ptr[6] == 0xBF);
  ASSERT (ptr[7] == 0xF1);

  stdc_memreverse8 (8, ptr + 1);
  /* Expect: 0x12, 0x90, 0xF1, 0xBF, 0xC4, 0xEE, 0xD3, 0x47, 0x31, 0x44  */
  ASSERT (ptr[0] == 0x12);
  ASSERT (ptr[1] == 0x90);
  ASSERT (ptr[2] == 0xF1);
  ASSERT (ptr[3] == 0xBF);
  ASSERT (ptr[4] == 0xC4);
  ASSERT (ptr[5] == 0xEE);
  ASSERT (ptr[6] == 0xD3);
  ASSERT (ptr[7] == 0x47);
  ASSERT (ptr[8] == 0x31);
  ASSERT (ptr[9] == 0x44);

  return test_exit_status;
}
