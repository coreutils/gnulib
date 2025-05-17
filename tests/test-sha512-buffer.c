/* Test of sha512_buffer() function.
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
#include "sha512.h"

#include <stdio.h>
#include <string.h>

int
main (void)
{
  const char *in1 =
    "Deldoul has 8647 inhabitants.\n"
    "<https://en.wikipedia.org/wiki/Deldoul,_Timimoun>\n";
  const char *out1 =
    "\xe5\x31\xc6\x89\x31\x84\xe7\xda\xa2\x26\x4c\xfa\xb1\x40\xb7\x3d"
    "\x49\x5c\xce\xfb\x53\xbf\x5a\xe3\x1c\x2b\xad\x5c\x3e\x0b\x82\x9a"
    "\xa5\x3c\x25\xb1\x39\x52\xa7\xca\x73\xe5\xf2\xb8\xc1\xef\xa7\x45"
    "\xac\x8a\xf9\x83\x9c\x35\x92\x94\x2d\xb7\xb4\xb4\x08\x2e\x7f\xe2";
  char buf[SHA512_DIGEST_SIZE];

  if (memcmp (sha512_buffer (in1, strlen (in1), buf),
              out1, SHA512_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA512_DIGEST_SIZE; i++)
        printf ("%02x ", out1[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA512_DIGEST_SIZE; i++)
        printf ("%02x ", buf[i] & 0xFFu);
      printf ("\n");
      return 1;
    }

  return 0;
}
