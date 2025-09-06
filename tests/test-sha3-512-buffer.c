/* Test of the sha3_512_buffer() function.
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
  { "", "\xa6\x9f\x73\xcc\xa2\x3a\x9a\xc5\xc8\xb5\x67\xdc\x18\x5a\x75\x6e\x97"
        "\xc9\x82\x16\x4f\xe2\x58\x59\xe0\xd1\xdc\xc1\x47\x5c\x80\xa6\x15\xb2"
        "\x12\x3a\xf1\xf5\xf9\x4c\x11\xe3\xe9\x40\x2c\x3a\xc5\x58\xf5\x00\x19"
        "\x9d\x95\xb6\xd3\xe3\x01\x75\x85\x86\x28\x1d\xcd\x26" },
  { "abc",
    "\xb7\x51\x85\x0b\x1a\x57\x16\x8a\x56\x93\xcd\x92\x4b\x6b\x09\x6e\x08\xf6"
    "\x21\x82\x74\x44\xf7\x0d\x88\x4f\x5d\x02\x40\xd2\x71\x2e\x10\xe1\x16\xe9"
    "\x19\x2a\xf3\xc9\x1a\x7e\xc5\x76\x47\xe3\x93\x40\x57\x34\x0b\x4c\xf4\x08"
    "\xd5\xa5\x65\x92\xf8\x27\x4e\xec\x53\xf0" },
  { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "\x04\xa3\x71\xe8\x4e\xcf\xb5\xb8\xb7\x7c\xb4\x86\x10\xfc\xa8\x18\x2d\xd4"
    "\x57\xce\x6f\x32\x6a\x0f\xd3\xd7\xec\x2f\x1e\x91\x63\x6d\xee\x69\x1f\xbe"
    "\x0c\x98\x53\x02\xba\x1b\x0d\x8d\xc7\x8c\x08\x63\x46\xb5\x33\xb4\x9c\x03"
    "\x0d\x99\xa2\x7d\xaf\x11\x39\xd6\xe7\x5e" },
  { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopj"
    "klmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    "\xaf\xeb\xb2\xef\x54\x2e\x65\x79\xc5\x0c\xad\x06\xd2\xe5\x78\xf9\xf8\xdd"
    "\x68\x81\xd7\xdc\x82\x4d\x26\x36\x0f\xee\xbf\x18\xa4\xfa\x73\xe3\x26\x11"
    "\x22\x94\x8e\xfc\xfd\x49\x2e\x74\xe8\x2e\x21\x89\xed\x0f\xb4\x40\xd1\x87"
    "\xf3\x82\x27\x0c\xb4\x55\xf2\x1d\xd1\x85" }
};

static int
check (char const *message, size_t len, char const *expect)
{
  char buf[SHA3_512_DIGEST_SIZE];
  if (memcmp (sha3_512_buffer (message, len, buf),
              expect, SHA3_512_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA3_512_DIGEST_SIZE; i++)
        printf ("%02x ", expect[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA3_512_DIGEST_SIZE; i++)
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
      char const *expect = ("\x3c\x3a\x87\x6d\xa1\x40\x34\xab\x60\x62\x7c\x07"
                            "\x7b\xb9\x8f\x7e\x12\x0a\x2a\x53\x70\x21\x2d\xff"
                            "\xb3\x38\x5a\x18\xd4\xf3\x88\x59\xed\x31\x1d\x0a"
                            "\x9d\x51\x41\xce\x9c\xc5\xc6\x6e\xe6\x89\xb2\x66"
                            "\xa8\xaa\x18\xac\xe8\x28\x2a\x0e\x0d\xb5\x96\xc9"
                            "\x0b\x0a\x7b\x87");
      if (check (large, 1000000, expect) != 0)
        return 1;
      free (large);
    }

  /* Check that sha3_free_ctx can be called multiple times without
     crashing.  */
  {
    struct sha3_ctx ctx;
    if (sha3_512_init_ctx (&ctx))
      {
        sha3_free_ctx (&ctx);
        sha3_free_ctx (&ctx);
      }
  }

  return 0;
}
