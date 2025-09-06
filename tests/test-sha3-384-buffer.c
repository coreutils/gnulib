/* Test of the sha3_384_buffer() function.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Collin Funk <collin.funk1@gmail.com>, 2025.  */

#include <config.h>

/* Specification.  */
#include "sha3.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct test_case {
  char const *message;
  char const *digest;
};

static const struct test_case test_cases[] = {
  { "", "\x0c\x63\xa7\x5b\x84\x5e\x4f\x7d\x01\x10\x7d\x85\x2e\x4c\x24\x85\xc5"
        "\x1a\x50\xaa\xaa\x94\xfc\x61\x99\x5e\x71\xbb\xee\x98\x3a\x2a\xc3\x71"
        "\x38\x31\x26\x4a\xdb\x47\xfb\x6b\xd1\xe0\x58\xd5\xf0\x04" },
  { "abc",
    "\xec\x01\x49\x82\x88\x51\x6f\xc9\x26\x45\x9f\x58\xe2\xc6\xad\x8d\xf9\xb4"
    "\x73\xcb\x0f\xc0\x8c\x25\x96\xda\x7c\xf0\xe4\x9b\xe4\xb2\x98\xd8\x8c\xea"
    "\x92\x7a\xc7\xf5\x39\xf1\xed\xf2\x28\x37\x6d\x25" },
  { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "\x99\x1c\x66\x57\x55\xeb\x3a\x4b\x6b\xbd\xfb\x75\xc7\x8a\x49\x2e\x8c\x56"
    "\xa2\x2c\x5c\x4d\x7e\x42\x9b\xfd\xbc\x32\xb9\xd4\xad\x5a\xa0\x4a\x1f\x07"
    "\x6e\x62\xfe\xa1\x9e\xef\x51\xac\xd0\x65\x7c\x22" },
  { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopj"
    "klmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    "\x79\x40\x7d\x3b\x59\x16\xb5\x9c\x3e\x30\xb0\x98\x22\x97\x47\x91\xc3\x13"
    "\xfb\x9e\xcc\x84\x9e\x40\x6f\x23\x59\x2d\x04\xf6\x25\xdc\x8c\x70\x9b\x98"
    "\xb4\x3b\x38\x52\xb3\x37\x21\x61\x79\xaa\x7f\xc7" }
};

static int
check (char const *message, size_t len, char const *expect)
{
  char buf[SHA3_384_DIGEST_SIZE];
  if (memcmp (sha3_384_buffer (message, len, buf),
              expect, SHA3_384_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA3_384_DIGEST_SIZE; i++)
        printf ("%02x ", expect[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA3_384_DIGEST_SIZE; i++)
        printf ("%02x ", buf[i] & 0xFFu);
      printf ("\n");
      return 1;
    }
  return 0;
}

int
main (void)
{
  /* Check the test vectors.  */
  for (size_t i = 0; i < sizeof test_cases / sizeof *test_cases; ++i)
    {
      char const *message = test_cases[i].message;
      char const *digest = test_cases[i].digest;
      if (check (message, strlen (message), digest) != 0)
        return 1;
    }

  /* Check that a large buffer works.  */
  char *large = malloc (1000000);
  if (large != NULL)
    {
      memset (large, 'a', 1000000);
      char const *expect = ("\xee\xe9\xe2\x4d\x78\xc1\x85\x53\x37\x98\x34\x51"
                            "\xdf\x97\xc8\xad\x9e\xed\xf2\x56\xc6\x33\x4f\x8e"
                            "\x94\x8d\x25\x2d\x5e\x0e\x76\x84\x7a\xa0\x77\x4d"
                            "\xdb\x90\xa8\x42\x19\x0d\x2c\x55\x8b\x4b\x83\x40");
      if (check (large, 1000000, expect) != 0)
        return 1;
      free (large);
    }

  /* Check that sha3_free_ctx can be called multiple times without
     crashing.  */
  {
    struct sha3_ctx ctx;
    if (sha3_384_init_ctx (&ctx))
      {
        sha3_free_ctx (&ctx);
        sha3_free_ctx (&ctx);
      }
  }

  return 0;
}
