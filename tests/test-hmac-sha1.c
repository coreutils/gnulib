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

/* Written by Simon Josefsson.  */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include "hmac.h"

int
main (int argc, char *argv[])
{
  {
    char *key =
      "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
    size_t key_len = 16;
    char *data = "Hi There";
    size_t data_len = 8;
    char *digest =
      "\x67\x5b\x0b\x3a\x1b\x4d\xdf\x4e\x12\x48\x72\xda\x6c\x2f\x63\x2b\xfe\xd9\x57\xe9";
    char out[20];

    if (hmac_sha1 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 20) != 0)
      {
        size_t i;
        printf ("hash 1 mismatch. expected:\n");
        for (i = 0; i < 20; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 20; i++)
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
      "\xef\xfc\xdf\x6a\xe5\xeb\x2f\xa2\xd2\x74\x16\xd5\xf1\x84\xdf\x9c\x25\x9a\x7c\x79";
    char out[20];

    if (hmac_sha1 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 20) != 0)
      {
        size_t i;
        printf ("hash 2 mismatch. expected:\n");
        for (i = 0; i < 20; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 20; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  {
    char *key =
      "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA";
    size_t key_len = 16;
    char *data = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD";
    size_t data_len = 50;
    char *digest =
      "\xd7\x30\x59\x4d\x16\x7e\x35\xd5\x95\x6f\xd8\x00\x3d\x0d\xb3\xd3\xf4\x6d\xc7\xbb";
    char out[20];

    if (hmac_sha1 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 20) != 0)
      {
        size_t i;
        printf ("hash 3 mismatch. expected:\n");
        for (i = 0; i < 20; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 20; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  return 0;
}
