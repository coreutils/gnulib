/* Test of sha384_buffer() function.
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
    "\x57\x93\xc9\xcf\x15\x18\x89\x99\x3d\xd1\x42\x89\x1e\x0a\xec\xec"
    "\x7f\xb0\x4f\x82\x0e\x3a\x4f\xe0\x69\x8a\x5d\x36\x40\x07\x63\xd3"
    "\x74\x8d\x80\x99\xfb\x05\xb5\x22\xa3\x8f\x29\x1c\xea\xdd\x7a\x82";
  char buf[SHA384_DIGEST_SIZE];

  if (memcmp (sha384_buffer (in1, strlen (in1), buf),
              out1, SHA384_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA384_DIGEST_SIZE; i++)
        printf ("%02x ", out1[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA384_DIGEST_SIZE; i++)
        printf ("%02x ", buf[i] & 0xFFu);
      printf ("\n");
      return 1;
    }

  return 0;
}
