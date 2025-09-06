/* Test of the sha3_256_buffer() function.
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
  { "", "\xa7\xff\xc6\xf8\xbf\x1e\xd7\x66\x51\xc1\x47\x56\xa0\x61\xd6\x62\xf5"
        "\x80\xff\x4d\xe4\x3b\x49\xfa\x82\xd8\x0a\x4b\x80\xf8\x43\x4a" },
  { "abc",
    "\x3a\x98\x5d\xa7\x4f\xe2\x25\xb2\x04\x5c\x17\x2d\x6b\xd3\x90\xbd\x85\x5f"
    "\x08\x6e\x3e\x9d\x52\x5b\x46\xbf\xe2\x45\x11\x43\x15\x32" },
  { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    "\x41\xc0\xdb\xa2\xa9\xd6\x24\x08\x49\x10\x03\x76\xa8\x23\x5e\x2c\x82\xe1"
    "\xb9\x99\x8a\x99\x9e\x21\xdb\x32\xdd\x97\x49\x6d\x33\x76" },
  { "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopj"
    "klmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    "\x91\x6f\x60\x61\xfe\x87\x97\x41\xca\x64\x69\xb4\x39\x71\xdf\xdb\x28\xb1"
    "\xa3\x2d\xc3\x6c\xb3\x25\x4e\x81\x2b\xe2\x7a\xad\x1d\x18" },
};

static int
check (char const *message, size_t len, char const *expect)
{
  char buf[SHA3_256_DIGEST_SIZE];
  if (memcmp (sha3_256_buffer (message, len, buf),
              expect, SHA3_256_DIGEST_SIZE) != 0)
    {
      size_t i;
      printf ("expected:\n");
      for (i = 0; i < SHA3_256_DIGEST_SIZE; i++)
        printf ("%02x ", expect[i] & 0xFFu);
      printf ("\ncomputed:\n");
      for (i = 0; i < SHA3_256_DIGEST_SIZE; i++)
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
      char const *expect = ("\x5c\x88\x75\xae\x47\x4a\x36\x34\xba\x4f\xd5\x5e"
                            "\xc8\x5b\xff\xd6\x61\xf3\x2a\xca\x75\xc6\xd6\x99"
                            "\xd0\xcd\xcb\x6c\x11\x58\x91\xc1");
      if (check (large, 1000000, expect) != 0)
        return 1;
      free (large);
    }

  /* Check that sha3_free_ctx can be called multiple times without
     crashing.  */
  {
    struct sha3_ctx ctx;
    if (sha3_256_init_ctx (&ctx))
      {
        sha3_free_ctx (&ctx);
        sha3_free_ctx (&ctx);
      }
  }

  return 0;
}
