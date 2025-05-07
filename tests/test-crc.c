/*
 * Copyright (C) 2005-2007, 2010-2025 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson.  */

#include <config.h>

#include "crc.h"

#include <stdio.h>
#include <string.h>

#include "macros.h"

#define MAX_ALIGNMENT 32

uint32_t trailing_unaligned_byte_hashes[32] = {
  0xDFDD19BE, 0xD2D258E4, 0x3F67D485, 0xFF300DA4, 0xFC46B871,
  0x6B97174C, 0x3C6873C6, 0x3DE4FDA4, 0x783A2D5F, 0x2ECC37AC,
  0xFEFAD741, 0x5FFB72DF, 0xF6E6DD29, 0xCD21E04E, 0x78CAE842,
  0xCFC25F71, 0x7DC85CEE, 0xC971E8DA, 0x541726C1, 0xB15FFE02,
  0xFABE629D, 0x4F254C30, 0x444B083F, 0x049F6D1D, 0x2D0670E0,
  0x719C83F9, 0xEB7FAABC, 0xF0E4ECEE, 0x80958327, 0xD3566178,
  0xD1683190, 0x7ABB3299
};

uint32_t leading_unaligned_byte_hashes[32] = {
  0xA6C6EC55, 0x10BA5C53, 0xE991A1AB, 0xD9279D86, 0xEF21C396,
  0x78A4387C, 0x2F865A77, 0xAEA7587C, 0x9779C9D0, 0x914DD47A,
  0xB42B53CB, 0x47D39201, 0xE53E891F, 0xB4501BA1, 0xE946965F,
  0xF092BC6F, 0xA4D33D75, 0xD602ECA6, 0x107142FB, 0xD83B4AFD,
  0x9B9CE2B9, 0xCF2C3E17, 0xCDE401A1, 0xE686DFE7, 0x5137E14F,
  0xB309A32D, 0x2E1BB47F, 0xECA84001, 0x1F5B5472, 0x5BA21753,
  0x203ACDCA, 0x48B56C22
};

uint32_t variable_alignment_variable_length_hashes[] = {
  0x00000000, 0xB40BBE37, 0x98662B02, 0x062ED334, 0xFD64D7A9, 0xCA47A65D,
  0x70A0B329, 0x191C96C2, 0xFECD07E0, 0x5120CD3D, 0xC886714D, 0x5416D158,
  0xA9E8B882, 0x7B18C316, 0x0A7BB6E4, 0x19664DC7, 0x71A8E3C4, 0xDD7CD42A,
  0x95623656, 0x2C4CFCBD, 0x7046B873, 0x7777762E, 0xAD10FF30, 0x15773DB6,
  0x0515202E, 0xE0D0FC3E, 0x2F8FAC7B, 0x3A9ADE62, 0x13560288, 0x8BADEEE8,
  0x4B8F79A9, 0xBDF67D65
};

int
main (void)
{
  uint32_t p;
  size_t i, j;
  char data[MAX_ALIGNMENT * 3];

  p = crc32_update_no_xor (42, "foo", 3);
  if (p != 0x46e87f05)
    {
      printf ("cunx got %lx\n", (unsigned long) p);
      return 1;
    }

  p = crc32_no_xor ("foo", 3);
  if (p != 0x7332bc33)
    {
      printf ("cnx got %lx\n", (unsigned long) p);
      return 1;
    }

  p = crc32_update (42, "foo", 3);
  if (p != 0xb9a9a617)
    {
      printf ("cu got %lx\n", (unsigned long) p);
      return 1;
    }

  p = crc32 ("foo", 3);
  if (p != 0x8c736521)
    {
      printf ("c got %lx\n", (unsigned long) p);
      return 1;
    }

  /*
   * Tests for new CRC32 implementation
   * The original implementation works on a byte-by-byte basis
   * but new implementations may work on longer alignments.
   * This test will confirm correct operation with non-aligned
   * data.
   */

  /* Test for trailing unaligned bytes */

  for (i = 0; i < MAX_ALIGNMENT; i++)
    {
      p = crc32_update_no_xor (0, randomb, MAX_ALIGNMENT + i);
      if (p != trailing_unaligned_byte_hashes[i])
        {
          printf ("aligned c at %lu length %lu got %lx\n", (size_t) 0,
                  MAX_ALIGNMENT + i, (unsigned long) p);
          return 1;
        }
    }

  /* Test for leading unaligned bytes */

  for (i = 0; i < MAX_ALIGNMENT; i++)
    {
      p = crc32_update_no_xor (0, randomb + i, (MAX_ALIGNMENT * 2) - i);
      if (p != leading_unaligned_byte_hashes[i])
        {
          printf ("aligned c at %lu length %lu got %lx\n", i,
                  (MAX_ALIGNMENT * 2) - i, (unsigned long) p);
          return 1;
        }
    }

  /* Combined test of multiple lengths at multiple alignments */

  for (i = 0; i < MAX_ALIGNMENT; i++)
    for (j = 0; j < MAX_ALIGNMENT; j++)
      {
        memcpy (data + i, randomb, MAX_ALIGNMENT * 2);
        p = crc32_update_no_xor (0, data + i, j);
        if (p != variable_alignment_variable_length_hashes[j])
          {
            printf ("aligned c at %lu length %lu got %lx\n", i, j,
                    (unsigned long) p);
            return 1;
          }
      }


  return 0;
}
