/* Test of the sha3_224_buffer() function.
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
  { "", "\x6b\x4e\x03\x42\x36\x67\xdb\xb7\x3b\x6e\x15\x45\x4f\x0e\xb1\xab\xd4"
        "\x59\x7f\x9a\x1b\x07\x8e\x3f\x5b\x5a\x6b\xc7" },
  { "abc", "\xe6\x42\x82\x4c\x3f\x8c\xf2\x4a\xd0\x92\x34\xee\x7d\x3c\x76\x6f"
           "\xc9\xa3\xa5\x16\x8d\x0c\x94\xad\x73\xb4\x6f\xdf" },
  { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "\x8a\x24\x10\x8b\x15\x4a\xda\x21\xc9\xfd\x55\x74\x49\x44\x79\xba\x5c\x7e"
    "\x7a\xb7\x6e\xf2\x64\xea\xd0\xfc\xce\x33" },
  { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopj"
    "klmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    "\x54\x3e\x68\x68\xe1\x66\x6c\x1a\x64\x36\x30\xdf\x77\x36\x7a\xe5\xa6\x2a"
    "\x85\x07\x0a\x51\xc1\x4c\xbf\x66\x5c\xbc" },
};

static int
check (char const *message, size_t len, char const *expect)
{
  char buf[SHA3_224_DIGEST_SIZE];
  if (memcmp (sha3_224_buffer (message, len, buf),
              expect, SHA3_224_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA3_224_DIGEST_SIZE; i++)
        printf ("%02x ", expect[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA3_224_DIGEST_SIZE; i++)
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
      char const *expect = ("\xd6\x93\x35\xb9\x33\x25\x19\x2e\x51\x6a\x91\x2e"
                            "\x6d\x19\xa1\x5c\xb5\x1c\x6e\xd5\xc1\x52\x43\xe7"
                            "\xa7\xfd\x65\x3c");
      if (check (large, 1000000, expect) != 0)
        return 1;
      free (large);
    }

  /* Check that sha3_free_ctx can be called multiple times without
     crashing.  */
  {
    struct sha3_ctx ctx;
    if (sha3_224_init_ctx (&ctx))
      {
        sha3_free_ctx (&ctx);
        sha3_free_ctx (&ctx);
      }
  }

  return 0;
}
