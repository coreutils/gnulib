/*
 * Copyright (C) 2005 Free Software Foundation
 * Written by Simon Josefsson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include "gc.h"

int
main (int argc, char *argv[])
{
  Gc_rc rc;

  rc = gc_init ();
  if (rc != GC_OK)
    {
      printf ("gc_init() failed\n");
      return 1;
    }

  {
    char *in = "abcdefgh";
    size_t inlen = strlen (in);
    char *expect = "\x42\x5a\xf1\x2a\x07\x43\x50\x2b"
      "\x32\x2e\x93\xa0\x15\xbc\xf8\x68\xe3\x24\xd5\x6a";
    char out[20];

    if (gc_sha1 (in, inlen, out) != 0)
      {
	printf ("gc_sha1 call failed\n");
	return 1;
      }

    if (memcmp (out, expect, 20) != 0)
      {
	size_t i;
	printf ("sha1 missmatch. expected:\n");
	for (i = 0; i < 16; i++)
	  printf ("%02x ", expect[i] & 0xFF);
	printf ("\ncomputed:\n");
	for (i = 0; i < 16; i++)
	  printf ("%02x ", out[i] & 0xFF);
	printf ("\n");
	return 1;
      }

    rc = gc_hash_buffer (GC_SHA1, "abcdefgh", 8, out);
    if (rc != GC_OK)
      {
	printf ("gc_hash_buffer(sha1) call failed: %d\n", rc);
	return 1;
      }

    if (memcmp (out, expect, 20) != 0)
      {
	size_t i;
	printf ("sha1' missmatch. expected:\n");
	for (i = 0; i < 16; i++)
	  printf ("%02x ", expect[i] & 0xFF);
	printf ("\ncomputed:\n");
	for (i = 0; i < 16; i++)
	  printf ("%02x ", out[i] & 0xFF);
	printf ("\n");
	return 1;
      }
  }

  gc_done ();

  return 0;
}
