/* crc-generate-table.c -- cyclic redundancy check table generator
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Sam Russell. */

#include <stdio.h>
#include <stdlib.h>

/*
 * The following function was extracted from RFC 1952 by Sam
 * Russell.  It was modified to remove the reference to the
 * crc_table_computed flag, to extend it to accept a variable number
 * of bits (described below), and reformatted to follow GNU
 * formatting guidelines.
 *
 * make_crc_table() takes a number of bits and generates a lookup
 * table for the CRC32 algorithm. Usage is as follows:
 *
 * make_crc_table(8) : generate for CRC32(0x00) to CRC32(0xFF)
 * make_crc_table(16) : generate for CRC32(0x0000) to CRC32 (0xFF00)
 * in increments of 0x100
 *
 * This is used for the Sarwate algorithm specified in RFC 1952
 * which uses a single lookup table of make_crc_table(8), and for
 * the slice-by-8 algorithm which uses 8 tables from in 8-bit
 * increments from make_crc_table(8) to make_crc_table(64)
 */
static unsigned long crc_table[256];

static void
make_crc_table (int bits)
{
  unsigned long c;

  int n, k;
  for (n = 0; n < 256; n++)
    {
      c = (unsigned long) n;
      for (k = 0; k < bits; k++)
        {
          if (c & 1)
            c = 0xedb88320L ^ (c >> 1);
          else
            c = c >> 1;
        }
      crc_table[n] = c;
    }
}

static void
print_crc_table (FILE * stream, int bits)
{
  make_crc_table (bits);

  fprintf (stream, "  {");

  for (int i = 0; i < 256; i++)
    {
      if ((i % 6) == 0)
        fprintf (stream, "\n   ");
      if (i != 255)
        fprintf (stream, " 0x%08lX,", crc_table[i]);
      else
        fprintf (stream, " 0x%08lX\n", crc_table[i]);
    }

  fprintf (stream, "  }");
}

static void
print_header (FILE * stream)
{
  fprintf (stream, "/* Slice-by-8 lookup tables */\n");
  fprintf (stream, "static const uint32_t crc32_sliceby8_table[][256] = {\n");
  for (int i = 8; i <= 64; i += 8)
    {
      print_crc_table (stream, i);
      if (i < 64)
        fprintf (stream, ",");
      fprintf (stream, "\n");
    }
  fprintf (stream, "};\n");
}

static void
print_copyright_notice (FILE * stream)
{
  fprintf (stream, "/* DO NOT EDIT! GENERATED AUTOMATICALLY! */\n");
  fprintf (stream, "/* crc.c -- cyclic redundancy checks\n");
  fprintf (stream,
           "Copyright (C) 2005-2006, 2009-2025 Free Software Foundation, Inc.\n");
  fprintf (stream, "\n");
  fprintf (stream,
           "This file is free software: you can redistribute it and/or modify\n");
  fprintf (stream,
           "it under the terms of the GNU Lesser General Public License as\n");
  fprintf (stream,
           "published by the Free Software Foundation, either version 3 of the\n");
  fprintf (stream, "License, or (at your option) any later version.\n");
  fprintf (stream, "\n");
  fprintf (stream,
           "This file is distributed in the hope that it will be useful,\n");
  fprintf (stream,
           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  fprintf (stream,
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
  fprintf (stream, "GNU Lesser General Public License for more details.\n");
  fprintf (stream, "\n");
  fprintf (stream,
           "You should have received a copy of the GNU Lesser General Public License\n");
  fprintf (stream,
           "along with this program.  If not, see <https://www.gnu.org/licenses/>.  */\n\n");
}

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, " Usage: %s crc-sliceby8.h\n", argv[0]);
      exit (1);
    }

  const char *filename = argv[1];
  FILE *stream = fopen (filename, "w");
  if (stream == NULL)
    {
      fprintf (stderr, "cannot open '%s' for writing\n", filename);
      exit (1);
    }

  print_copyright_notice (stream);
  print_header (stream);

  if (ferror (stream) || fclose (stream))
    {
      fprintf (stderr, "error writing to '%s'\n", filename);
      exit (1);
    }

  return 0;
}
