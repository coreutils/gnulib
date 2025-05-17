/* Test of sha224_buffer() function.
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
    "\x8a\x7d\x25\x22\x11\x13\x61\x35\x7a\x61\x6e\x4a\xf5\xbd\x0a\x17"
    "\x5b\xdf\x7e\x27\xa1\xb7\x00\xf4\x62\x89\x41\x84";
  char buf[SHA224_DIGEST_SIZE];

  if (memcmp (sha224_buffer (in1, strlen (in1), buf),
              out1, SHA224_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA224_DIGEST_SIZE; i++)
        printf ("%02x ", out1[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA224_DIGEST_SIZE; i++)
        printf ("%02x ", buf[i] & 0xFFu);
      printf ("\n");
      return 1;
    }

  return 0;
}
