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
      "\x87\xaa\x7c\xde\xa5\xef\x61\x9d\x4f\xf0\xb4\x24\x1a\x1d\x6c\xb0\x23\x79\xf4\xe2\xce\x4e\xc2\x78\x7a\xd0\xb3\x05\x45\xe1\x7c\xde\xda\xa8\x33\xb7\xd6\xb8\xa7\x02\x03\x8b\x27\x4e\xae\xa3\xf4\xe4\xbe\x9d\x91\x4e\xeb\x61\xf1\x70\x2e\x69\x6c\x20\x3a\x12\x68\x54";
    char out[64];

    if (hmac_sha512 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 64) != 0)
      {
        size_t i;
        printf ("hash 1 mismatch. expected:\n");
        for (i = 0; i < 64; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 64; i++)
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
      "\x16\x4b\x7a\x7b\xfc\xf8\x19\xe2\xe3\x95\xfb\xe7\x3b\x56\xe0\xa3\x87\xbd\x64\x22\x2e\x83\x1f\xd6\x10\x27\x0c\xd7\xea\x25\x05\x54\x97\x58\xbf\x75\xc0\x5a\x99\x4a\x6d\x03\x4f\x65\xf8\xf0\xe6\xfd\xca\xea\xb1\xa3\x4d\x4a\x6b\x4b\x63\x6e\x07\x0a\x38\xbc\xe7\x37";
    char out[64];

    if (hmac_sha512 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 64) != 0)
      {
        size_t i;
        printf ("hash 2 mismatch. expected:\n");
        for (i = 0; i < 64; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 64; i++)
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
      "\xfa\x73\xb0\x08\x9d\x56\xa2\x84\xef\xb0\xf0\x75\x6c\x89\x0b\xe9\xb1\xb5\xdb\xdd\x8e\xe8\x1a\x36\x55\xf8\x3e\x33\xb2\x27\x9d\x39\xbf\x3e\x84\x82\x79\xa7\x22\xc8\x06\xb4\x85\xa4\x7e\x67\xc8\x07\xb9\x46\xa3\x37\xbe\xe8\x94\x26\x74\x27\x88\x59\xe1\x32\x92\xfb";
    char out[64];

    if (hmac_sha512 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 64) != 0)
      {
        size_t i;
        printf ("hash 3 mismatch. expected:\n");
        for (i = 0; i < 64; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 64; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  return 0;
}
