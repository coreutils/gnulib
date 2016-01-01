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

/* Test vectors from RFC 2104. */

int
main (int argc, char *argv[])
{
  {
    /*
       key =         0x0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b
       key_len =     16 bytes
       data =        "Hi There"
       data_len =    8  bytes
       digest =      0x9294727a3638bb1c13f48ef8158bfc9d
     */
    char *key =
      "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
    size_t key_len = 16;
    char *data = "Hi There";
    size_t data_len = 8;
    char *digest =
      "\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d";
    char out[16];

    if (hmac_md5 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 16) != 0)
      {
        size_t i;
        printf ("hash 1 mismatch. expected:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  {
    /*
       key =         "Jefe"
       data =        "what do ya want for nothing?"
       data_len =    28 bytes
       digest =      0x750c783e6ab0b503eaa86e310a5db738
     */
    char *key = "Jefe";
    size_t key_len = 4;
    char *data = "what do ya want for nothing?";
    size_t data_len = 28;
    char *digest =
      "\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7\x38";
    char out[16];

    if (hmac_md5 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 16) != 0)
      {
        size_t i;
        printf ("hash 2 mismatch. expected:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  {
    /*
       key =         0xAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
       key_len       16 bytes
       data =        0xDDDDDDDDDDDDDDDDDDDD...
       ..DDDDDDDDDDDDDDDDDDDD...
       ..DDDDDDDDDDDDDDDDDDDD...
       ..DDDDDDDDDDDDDDDDDDDD...
       ..DDDDDDDDDDDDDDDDDDDD
       data_len =    50 bytes
       digest =      0x56be34521d144c88dbb8c733f0e8b3f6
     */
    char *key =
      "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA";
    size_t key_len = 16;
    char *data = "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
      "\xDD\xDD";
    size_t data_len = 50;
    char *digest =
      "\x56\xbe\x34\x52\x1d\x14\x4c\x88\xdb\xb8\xc7\x33\xf0\xe8\xb3\xf6";
    char out[16];

    if (hmac_md5 (key, key_len, data, data_len, out) != 0)
      {
        printf ("call failure\n");
        return 1;
      }

    if (memcmp (digest, out, 16) != 0)
      {
        size_t i;
        printf ("hash 3 mismatch. expected:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", digest[i] & 0xFF);
        printf ("\ncomputed:\n");
        for (i = 0; i < 16; i++)
          printf ("%02x ", out[i] & 0xFF);
        printf ("\n");
        return 1;
      }
  }

  return 0;
}
