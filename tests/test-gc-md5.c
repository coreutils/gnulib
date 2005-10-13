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

  /* Test vectors from RFC 1321. */

  {
    char *in = "abcdefghijklmnopqrstuvwxyz";
    size_t inlen = strlen (in);
    char *expect =
      "\xc3\xfc\xd3\xd7\x61\x92\xe4\x00\x7d\xfb\x49\x6c\xca\x67\xe1\x3b";
    char out[16];

    /* MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b */

    if (gc_md5 (in, inlen, out) != 0)
      {
	printf ("gc_md5 call failed\n");
	return 1;
      }

    if (memcmp (out, expect, 16) != 0)
      {
	size_t i;
	printf ("md5 1 missmatch. expected:\n");
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
