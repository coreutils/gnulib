/* Test of u16_prev() function.
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

#include "macros.h"

static int
check (const uint16_t *input, size_t input_length, ucs4_t *puc)
{
  ucs4_t uc;

  /* Test recognition when at the beginning of the string.  */
  if (u16_prev (&uc, input + input_length, input) != input)
    return 1;

  /* Test recognition when preceded by a 1-unit character.  */
  {
    uint16_t buf[100];
    uint16_t *ptr;
    size_t i;
    ucs4_t uc1;

    ptr = buf;
    *ptr++ = 0x2102;
    for (i = 0; i < input_length; i++)
      ptr[i] = input[i];

    if (u16_prev (&uc1, ptr + input_length, buf) != ptr)
      return 2;
    if (uc1 != uc)
      return 3;
  }

  /* Test recognition when preceded by a 2-unit character.  */
  {
    uint16_t buf[100];
    uint16_t *ptr;
    size_t i;
    ucs4_t uc1;

    ptr = buf;
    *ptr++ = 0xD835;
    *ptr++ = 0xDD1E;
    for (i = 0; i < input_length; i++)
      ptr[i] = input[i];

    if (u16_prev (&uc1, ptr + input_length, buf) != ptr)
      return 4;
    if (uc1 != uc)
      return 5;
  }

  *puc = uc;
  return 0;
}

static int
check_invalid (const uint16_t *input, size_t input_length)
{
  ucs4_t uc;

  /* Test recognition when at the beginning of the string.  */
  uc = 0xBADFACE;
  if (u16_prev (&uc, input + input_length, input) != NULL)
    return 1;
  if (uc != 0xBADFACE)
    return 2;

  /* Test recognition when preceded by a 1-unit character.  */
  {
    uint16_t buf[100];
    uint16_t *ptr;
    size_t i;

    ptr = buf;
    *ptr++ = 0x2102;
    for (i = 0; i < input_length; i++)
      ptr[i] = input[i];

    uc = 0xBADFACE;
    if (u16_prev (&uc, ptr + input_length, buf) != NULL)
      return 3;
    if (uc != 0xBADFACE)
      return 4;
  }

  /* Test recognition when preceded by a 2-unit character.  */
  {
    uint16_t buf[100];
    uint16_t *ptr;
    size_t i;

    ptr = buf;
    *ptr++ = 0xD835;
    *ptr++ = 0xDD1E;
    for (i = 0; i < input_length; i++)
      ptr[i] = input[i];

    uc = 0xBADFACE;
    if (u16_prev (&uc, ptr + input_length, buf) != NULL)
      return 5;
    if (uc != 0xBADFACE)
      return 6;
  }

  return 0;
}

int
main ()
{
  ucs4_t uc;

  /* Test ISO 646 unit input.  */
  {
    ucs4_t c;
    uint16_t buf[1];

    for (c = 0; c < 0x80; c++)
      {
        buf[0] = c;
        uc = 0xBADFACE;
        ASSERT (check (buf, 1, &uc) == 0);
        ASSERT (uc == c);
      }
  }

  /* Test BMP unit input.  */
  {
    static const uint16_t input[] = { 0x20AC };
    uc = 0xBADFACE;
    ASSERT (check (input, SIZEOF (input), &uc) == 0);
    ASSERT (uc == 0x20AC);
  }

  /* Test 2-units character input.  */
  {
    static const uint16_t input[] = { 0xD835, 0xDD1F };
    uc = 0xBADFACE;
    ASSERT (check (input, SIZEOF (input), &uc) == 0);
    ASSERT (uc == 0x1D51F);
  }

  /* Test incomplete/invalid 1-unit input.  */
  {
    static const uint16_t input[] = { 0xD835 };
    ASSERT (check_invalid (input, SIZEOF (input)) == 0);
  }
  {
    static const uint16_t input[] = { 0xDD1F };
    ASSERT (check_invalid (input, SIZEOF (input)) == 0);
  }

  return test_exit_status;
}
