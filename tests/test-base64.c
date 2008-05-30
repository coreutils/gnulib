/* Self tests for base64.
   Copyright (C) 2004, 2008 Free Software Foundation, Inc.
   Written by Simon Josefsson.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "base64.h"

int
main (int argc, char *argv[])
{
  const char *in = "abcdefghijklmnop";
  const char *b64in = "YWJjZGVmZw==";
  char out[255];
  size_t len;
  bool ok;

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 0, out, 0);
  if (out[0] != '\x42')
    fprintf (stderr, "failure\n");

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 1, out, 1);
  if (memcmp (out, "YQ==", 1) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 1, out, 2);
  if (memcmp (out, "YQ==", 2) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 1, out, 3);
  if (memcmp (out, "YQ==", 3) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 1, out, 4);
  if (memcmp (out, "YQ==", 4) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 1, out, 8);
  if (memcmp (out, "YQ==", 4) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 2, out, 4);
  if (memcmp (out, "YWI=", 4) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 3, out, 4);
  if (memcmp (out, "YWJj", 4) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 4, out, 5);
  if (memcmp (out, "YWJjZA==", 5) != 0)
    {
      out[5] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  memset (out, 0x42, sizeof (out));
  base64_encode (in, 4, out, 100);
  if (memcmp (out, "YWJjZA==", 6) != 0)
    {
      out[6] = '\0';
      fprintf (stderr, "failure (%s)\n", out);
    }

  /* Decode. */

  memset (out, 0x42, sizeof (out));
  len = 0;
  ok = base64_decode (b64in, 4, out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 0)
    fprintf (stderr, "failure (%lu)\n", (unsigned long) len);

  memset (out, 0x42, sizeof (out));
  len = 1;
  ok = base64_decode (b64in, 4, out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 1 || memcmp (out, "abcdefg", 1) != 0)
    {
      out[2] = '\0';
      fprintf (stderr, "failure (%lu: %s)\n", (unsigned long) len, out);
    }

  memset (out, 0x42, sizeof (out));
  len = 2;
  ok = base64_decode (b64in, 4, out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 2 || memcmp (out, "abcdefg", 2) != 0)
    {
      out[3] = '\0';
      fprintf (stderr, "failure (%lu: %s)\n", (unsigned long) len, out);
    }

  memset (out, 0x42, sizeof (out));
  len = 3;
  ok = base64_decode (b64in, 4, out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 3 || memcmp (out, "abcdefg", 3) != 0)
    {
      out[4] = '\0';
      fprintf (stderr, "failure (%lu: %s)\n", (unsigned long) len, out);
    }

  memset (out, 0x42, sizeof (out));
  len = 4;
  ok = base64_decode (b64in, 4, out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 3 || memcmp (out, "abcdefg", 3) != 0)
    {
      out[3] = '\0';
      fprintf (stderr, "failure (%lu: %s)\n", (unsigned long) len, out);
    }

  memset (out, 0x42, sizeof (out));
  len = 100;
  ok = base64_decode (b64in, strlen (b64in), out, &len);
  if (!ok)
    fprintf (stderr, "decode failed\n");
  if (len != 7 || memcmp (out, "abcdefg", 7) != 0)
    {
      out[7] = '\0';
      fprintf (stderr, "failure (%lu: %s)\n", (unsigned long) len, out);
    }

  return 0;
}
