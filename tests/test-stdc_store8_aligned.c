/* Test of stdc_store8_aligned_* functions.
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
test_stdc_store8_aligned_beu8 (void)
{
  static unsigned char data[1];
  stdc_store8_aligned_beu8 (0xD5, data);
  ASSERT (data[0] == (unsigned char) 0xD5);
}

static void
test_stdc_store8_aligned_beu16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data;
  stdc_store8_aligned_beu16 (0xD5E6, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
}

static void
test_stdc_store8_aligned_beu32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data;
  stdc_store8_aligned_beu32 (0xD5E6C2F4U, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
}

static void
test_stdc_store8_aligned_beu64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data;
  stdc_store8_aligned_beu64 (0xD5E6C2F4F1C9EADFULL, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
  ASSERT (data.bytes[4] == (unsigned char) 0xF1);
  ASSERT (data.bytes[5] == (unsigned char) 0xC9);
  ASSERT (data.bytes[6] == (unsigned char) 0xEA);
  ASSERT (data.bytes[7] == (unsigned char) 0xDF);
}

static void
test_stdc_store8_aligned_leu8 (void)
{
  static unsigned char data[1];
  stdc_store8_aligned_leu8 (0xD5, data);
  ASSERT (data[0] == (unsigned char) 0xD5);
}

static void
test_stdc_store8_aligned_leu16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data;
  stdc_store8_aligned_leu16 (0xE6D5, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
}

static void
test_stdc_store8_aligned_leu32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data;
  stdc_store8_aligned_leu32 (0xF4C2E6D5U, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
}

static void
test_stdc_store8_aligned_leu64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data;
  stdc_store8_aligned_leu64 (0xDFEAC9F1F4C2E6D5ULL, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
  ASSERT (data.bytes[4] == (unsigned char) 0xF1);
  ASSERT (data.bytes[5] == (unsigned char) 0xC9);
  ASSERT (data.bytes[6] == (unsigned char) 0xEA);
  ASSERT (data.bytes[7] == (unsigned char) 0xDF);
}

static void
test_stdc_store8_aligned_bes8 (void)
{
  static unsigned char data[1];
  stdc_store8_aligned_bes8 ((int8_t) 0xD5, data);
  ASSERT (data[0] == (unsigned char) 0xD5);
}

static void
test_stdc_store8_aligned_bes16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data;
  stdc_store8_aligned_bes16 ((int16_t) 0xD5E6, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
}

static void
test_stdc_store8_aligned_bes32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data;
  stdc_store8_aligned_bes32 (0xD5E6C2F4, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
}

static void
test_stdc_store8_aligned_bes64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data;
  stdc_store8_aligned_bes64 (0xD5E6C2F4F1C9EADFLL, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
  ASSERT (data.bytes[4] == (unsigned char) 0xF1);
  ASSERT (data.bytes[5] == (unsigned char) 0xC9);
  ASSERT (data.bytes[6] == (unsigned char) 0xEA);
  ASSERT (data.bytes[7] == (unsigned char) 0xDF);
}

static void
test_stdc_store8_aligned_les8 (void)
{
  static unsigned char data[1];
  stdc_store8_aligned_les8 ((int8_t) 0xD5, data);
  ASSERT (data[0] == (unsigned char) 0xD5);
}

static void
test_stdc_store8_aligned_les16 (void)
{
  static union { unsigned char bytes[2]; uint16_t align; } data;
  stdc_store8_aligned_les16 ((int16_t) 0xE6D5, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
}

static void
test_stdc_store8_aligned_les32 (void)
{
  static union { unsigned char bytes[4]; uint32_t align; } data;
  stdc_store8_aligned_les32 (0xF4C2E6D5, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
}

static void
test_stdc_store8_aligned_les64 (void)
{
  static union { unsigned char bytes[8]; uint64_t align; } data;
  stdc_store8_aligned_les64 (0xDFEAC9F1F4C2E6D5LL, data.bytes);
  ASSERT (data.bytes[0] == (unsigned char) 0xD5);
  ASSERT (data.bytes[1] == (unsigned char) 0xE6);
  ASSERT (data.bytes[2] == (unsigned char) 0xC2);
  ASSERT (data.bytes[3] == (unsigned char) 0xF4);
  ASSERT (data.bytes[4] == (unsigned char) 0xF1);
  ASSERT (data.bytes[5] == (unsigned char) 0xC9);
  ASSERT (data.bytes[6] == (unsigned char) 0xEA);
  ASSERT (data.bytes[7] == (unsigned char) 0xDF);
}

static void
test_strict_aliasing (void)
{
  unsigned char *buf = malloc (8);
  ASSERT (buf);
  stdc_store8_aligned_leu32 (0x13121110, buf);
  stdc_store8_aligned_leu32 (0x17161514, buf + 4);
  stdc_store8_aligned_leu16 (0xF3F2, buf + 2);
  ASSERT (stdc_load8_aligned_leu32 (buf) == 0xF3F21110);
  ASSERT (stdc_load8_aligned_leu32 (buf + 4) == 0x17161514);
  ASSERT (buf[0] == 0x10);
  ASSERT (buf[1] == 0x11);
  ASSERT (buf[2] == 0xF2);
  ASSERT (buf[3] == 0xF3);
  ASSERT (buf[4] == 0x14);
  ASSERT (buf[5] == 0x15);
  ASSERT (buf[6] == 0x16);
  ASSERT (buf[7] == 0x17);
}

int
main ()
{
  test_stdc_store8_aligned_beu8 ();
  test_stdc_store8_aligned_beu16 ();
  test_stdc_store8_aligned_beu32 ();
  test_stdc_store8_aligned_beu64 ();
  test_stdc_store8_aligned_leu8 ();
  test_stdc_store8_aligned_leu16 ();
  test_stdc_store8_aligned_leu32 ();
  test_stdc_store8_aligned_leu64 ();
  test_stdc_store8_aligned_bes8 ();
  test_stdc_store8_aligned_bes16 ();
  test_stdc_store8_aligned_bes32 ();
  test_stdc_store8_aligned_bes64 ();
  test_stdc_store8_aligned_les8 ();
  test_stdc_store8_aligned_les16 ();
  test_stdc_store8_aligned_les32 ();
  test_stdc_store8_aligned_les64 ();
  test_strict_aliasing ();

  return test_exit_status;
}
