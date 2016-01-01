/*
 * Copyright (C) 2005, 2010-2016 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Simon Josefsson.  Test vectors from RFC 4231.  */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include "hmac.h"

int
main (int argc, char *argv[])
{
  {
    char *key =
      "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
    size_t key_len = 20;
    char *data = "Hi There";
    size_t data_len = 8;
    char *digest =
      "\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1\x2b\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32\xcf\xf7";
    char out[32];

    if (hmac_sha256 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 32) != 0)
      {
        size_t i;
        printf ("hash 1 mismatch. expected:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  {
    char *key = "Jefe";
    size_t key_len = 4;
    char *data = "what do ya want for nothing?";
    size_t data_len = 28;
    char *digest =
      "\x5b\xdc\xc1\x46\xbf\x60\x75\x4e\x6a\x04\x24\x26\x08\x95\x75\xc7\x5a\x00\x3f\x08\x9d\x27\x39\x83\x9d\xec\x58\xb9\x64\xec\x38\x43";
    char out[32];

    if (hmac_sha256 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 32) != 0)
      {
        size_t i;
        printf ("hash 2 mismatch. expected:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  {
    char *key =
      "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA";
    size_t key_len = 20;
    char *data = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD";
    size_t data_len = 50;
    char *digest =
      "\x77\x3e\xa9\x1e\x36\x80\x0e\x46\x85\x4d\xb8\xeb\xd0\x91\x81\xa7\x29\x59\x09\x8b\x3e\xf8\xc1\x22\xd9\x63\x55\x14\xce\xd5\x65\xfe";
    char out[32];

    if (hmac_sha256 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 32) != 0)
      {
        size_t i;
        printf ("hash 3 mismatch. expected:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 32; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  return 0;
}
