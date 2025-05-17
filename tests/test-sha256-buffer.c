/* Test of sha256_buffer() function.
   Copyright (C) 2005-2025 Free Software Foundation, Inc.

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

#include <config.h>

/* Specification.  */
#include "sha256.h"

#include <stdio.h>
#include <string.h>

int
main (void)
{
  const char *in1 =
    "Prejmer has 8647 inhabitants.\n"
    "<https://en.wikipedia.org/wiki/Prejmer>\n";
  const char *out1 =
    "\xd1\x27\xec\x93\xdd\x7f\x60\xaa\x72\x51\x51\x40\xd0\xe2\x5e\xc7"
    "\x89\xaf\xfa\x91\x9c\x8a\x85\xb7\x19\x8d\x8b\x15\xaf\xa3\x4b\x2c";
  char buf[SHA256_DIGEST_SIZE];

  if (memcmp (sha256_buffer (in1, strlen (in1), buf),
              out1, SHA256_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA256_DIGEST_SIZE; i++)
        printf ("%02x ", out1[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA256_DIGEST_SIZE; i++)
        printf ("%02x ", buf[i] & 0xFFu);
      printf ("\n");
      return 1;
    }

  return 0;
}
