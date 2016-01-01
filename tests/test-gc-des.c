/*
 * Copyright (C) 2005, 2010-2016 Free Software Foundation, Inc.
 * Written by Simon Josefsson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include "gc.h"

int
main (int argc, char *argv[])
{
  gc_cipher_handle ctx;
  Gc_rc rc;

  rc = gc_init ();
  if (rc != GC_OK)
    {
      printf ("gc_init() failed\n");
      return 1;
    }

  /*
   * DES Maintenance Test
   */
  {
    int i;
    char key[8] = { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 };
    char input[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    char result[8] = { 0x24, 0x6e, 0x9d, 0xb9, 0xc5, 0x50, 0x38, 0x1a };
    char temp1[8], temp2[8], temp3[8];

    for (i = 0; i < 64; ++i)
      {

        rc = gc_cipher_open (GC_DES, GC_ECB, &ctx);
        if (rc != GC_OK)
          return 1;

        rc = gc_cipher_setkey (ctx, 8, key);
        if (rc != GC_OK)
          return 1;

        memcpy (temp1, input, 8);
        rc = gc_cipher_encrypt_inline (ctx, 8, temp1);
        if (rc != GC_OK)
          return 1;

        memcpy (temp2, temp1, 8);
        rc = gc_cipher_encrypt_inline (ctx, 8, temp2);
        if (rc != GC_OK)
          return 1;

        rc = gc_cipher_setkey (ctx, 8, temp2);
        if (rc != GC_OK)
          return 1;

        memcpy (temp3, temp1, 8);
        rc = gc_cipher_decrypt_inline (ctx, 8, temp3);
        if (rc != GC_OK)
          return 1;

        memcpy (key, temp3, 8);
        memcpy (input, temp1, 8);
      }
    if (memcmp (temp3, result, 8))
      return 1;
  }

  gc_done ();

  return 0;
}
