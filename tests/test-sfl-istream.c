/* Test of string or file based input stream with line number.
   Copyright (C) 2024-2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include "sfl-istream.h"

#include <unistd.h>

#include "macros.h"

#define CONTENTS_LEN 9
#define CONTENTS "Hello\377\n\nW"

static void
test_open_stream (sfl_istream_t *stream)
{
  int c;

  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 'H');
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 'e');
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 'l');
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 'l');
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 'o');
  ASSERT (sfl_get_line_number (stream) == 1);
  sfl_ungetc (stream, c);
  c = sfl_getc (stream);
  ASSERT (c == 'o');
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == 0xff);
  ASSERT (sfl_get_line_number (stream) == 1);
  sfl_ungetc (stream, c);
  c = sfl_getc (stream);
  ASSERT (c == 0xff);
  ASSERT (sfl_get_line_number (stream) == 1);
  c = sfl_getc (stream);
  ASSERT (c == '\n');
  ASSERT (sfl_get_line_number (stream) == 2);
  c = sfl_getc (stream);
  ASSERT (c == '\n');
  ASSERT (sfl_get_line_number (stream) == 3);
  sfl_ungetc (stream, c);
  ASSERT (sfl_get_line_number (stream) == 2);
  c = sfl_getc (stream);
  ASSERT (c == '\n');
  ASSERT (sfl_get_line_number (stream) == 3);
  c = sfl_getc (stream);
  ASSERT (c == 'W');
  ASSERT (sfl_get_line_number (stream) == 3);
  c = sfl_getc (stream);
  ASSERT (c == EOF);
  ASSERT (sfl_get_line_number (stream) == 3);
  c = sfl_getc (stream);
  ASSERT (c == EOF);
  ASSERT (sfl_get_line_number (stream) == 3);
  sfl_ungetc (stream, c);
  c = sfl_getc (stream);
  ASSERT (c == EOF);
  ASSERT (sfl_get_line_number (stream) == 3);
  ASSERT (!sfl_ferror (stream));
}

int
main ()
{
  char const contents[CONTENTS_LEN] _GL_ATTRIBUTE_NONSTRING = CONTENTS;

  /* Test reading from a file.  */
  {
    const char *filename = "test-sfl-istream.tmp";
    unlink (filename);
    {
      FILE *fp = fopen (filename, "wb");
      ASSERT (fwrite (contents, 1, CONTENTS_LEN, fp) == CONTENTS_LEN);
      ASSERT (fclose (fp) == 0);
    }
    {
      FILE *fp = fopen (filename, "rb");
      sfl_istream_t stream;
      sfl_istream_init_from_file (&stream, fp);
      test_open_stream (&stream);
      sfl_free (&stream);
    }
    unlink (filename);
  }

  /* Test reading from a string in memory.  */
  {
    sfl_istream_t stream;
    sfl_istream_init_from_string_desc (&stream,
                                       sd_new_addr (CONTENTS_LEN, contents));
    test_open_stream (&stream);
    sfl_free (&stream);
  }

  /* Test reading from a NUL-terminated string in memory.  */
  {
    sfl_istream_t stream;
    sfl_istream_init_from_string (&stream, CONTENTS);
    test_open_stream (&stream);
    sfl_free (&stream);
  }

  return 0;
}
