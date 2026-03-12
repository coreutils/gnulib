/* Test of stdc_load8_aligned_* functions.
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

#include <stdbit.h>

#include "macros.h"

static void
test_stdc_load8_aligned_beu8 (void)
{
  static unsigned char data[1] = { 0xD5 };
  uint8_t result = stdc_load8_aligned_beu8 (data);
  ASSERT (result == 0xD5);
}

static void
test_stdc_load8_aligned_beu16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data =
    { { 0xD5, 0xE6 } };
  uint16_t result = stdc_load8_aligned_beu16 (data.bytes);
  ASSERT (result == 0xD5E6);
}

static void
test_stdc_load8_aligned_beu32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4 } };
  uint32_t result = stdc_load8_aligned_beu32 (data.bytes);
  ASSERT (result == 0xD5E6C2F4U);
}

static void
test_stdc_load8_aligned_beu64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4, 0xF1, 0xC9, 0xEA, 0xDF } };
  uint64_t result = stdc_load8_aligned_beu64 (data.bytes);
  ASSERT (result == 0xD5E6C2F4F1C9EADFULL);
}

static void
test_stdc_load8_aligned_leu8 (void)
{
  static unsigned char data[1] = { 0xD5 };
  uint8_t result = stdc_load8_aligned_leu8 (data);
  ASSERT (result == 0xD5);
}

static void
test_stdc_load8_aligned_leu16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data =
    { { 0xD5, 0xE6 } };
  uint16_t result = stdc_load8_aligned_leu16 (data.bytes);
  ASSERT (result == 0xE6D5);
}

static void
test_stdc_load8_aligned_leu32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4 } };
  uint32_t result = stdc_load8_aligned_leu32 (data.bytes);
  ASSERT (result == 0xF4C2E6D5U);
}

static void
test_stdc_load8_aligned_leu64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4, 0xF1, 0xC9, 0xEA, 0xDF } };
  uint64_t result = stdc_load8_aligned_leu64 (data.bytes);
  ASSERT (result == 0xDFEAC9F1F4C2E6D5ULL);
}

static void
test_stdc_load8_aligned_bes8 (void)
{
  static unsigned char data[1] = { 0xD5 };
  int8_t result = stdc_load8_aligned_bes8 (data);
  ASSERT (result == (int8_t) 0xD5);
}

static void
test_stdc_load8_aligned_bes16 (void)
{
  static union { unsigned char bytes[2]; int16_t align; } data =
    { { 0xD5, 0xE6 } };
  int16_t result = stdc_load8_aligned_bes16 (data.bytes);
  ASSERT (result == (int16_t) 0xD5E6);
}

static void
test_stdc_load8_aligned_bes32 (void)
{
  static union { unsigned char bytes[4]; int32_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4 } };
  int32_t result = stdc_load8_aligned_bes32 (data.bytes);
  ASSERT (result == 0xD5E6C2F4);
}

static void
test_stdc_load8_aligned_bes64 (void)
{
  static union { unsigned char bytes[8]; int64_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4, 0xF1, 0xC9, 0xEA, 0xDF } };
  int64_t result = stdc_load8_aligned_bes64 (data.bytes);
  ASSERT (result == 0xD5E6C2F4F1C9EADFLL);
}

static void
test_stdc_load8_aligned_les8 (void)
{
  static unsigned char data[1] = { 0xD5 };
  int8_t result = stdc_load8_aligned_les8 (data);
  ASSERT (result == (int8_t) 0xD5);
}

static void
test_stdc_load8_aligned_les16 (void)
{
  static union { unsigned char bytes[2]; int16_t align; } data =
    { { 0xD5, 0xE6 } };
  int16_t result = stdc_load8_aligned_les16 (data.bytes);
  ASSERT (result == (int16_t) 0xE6D5);
}

static void
test_stdc_load8_aligned_les32 (void)
{
  static union { unsigned char bytes[4]; int32_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4 } };
  int32_t result = stdc_load8_aligned_les32 (data.bytes);
  ASSERT (result == 0xF4C2E6D5);
}

static void
test_stdc_load8_aligned_les64 (void)
{
  static union { unsigned char bytes[8]; int64_t align; } data =
    { { 0xD5, 0xE6, 0xC2, 0xF4, 0xF1, 0xC9, 0xEA, 0xDF } };
  int64_t result = stdc_load8_aligned_les64 (data.bytes);
  ASSERT (result == 0xDFEAC9F1F4C2E6D5LL);
}

int
main ()
{
  test_stdc_load8_aligned_beu8 ();
  test_stdc_load8_aligned_beu16 ();
  test_stdc_load8_aligned_beu32 ();
  test_stdc_load8_aligned_beu64 ();
  test_stdc_load8_aligned_leu8 ();
  test_stdc_load8_aligned_leu16 ();
  test_stdc_load8_aligned_leu32 ();
  test_stdc_load8_aligned_leu64 ();
  test_stdc_load8_aligned_bes8 ();
  test_stdc_load8_aligned_bes16 ();
  test_stdc_load8_aligned_bes32 ();
  test_stdc_load8_aligned_bes64 ();
  test_stdc_load8_aligned_les8 ();
  test_stdc_load8_aligned_les16 ();
  test_stdc_load8_aligned_les32 ();
  test_stdc_load8_aligned_les64 ();

  return test_exit_status;
}
