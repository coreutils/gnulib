/* A string or file based input stream, that keeps track of a line number.
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

/* Written by Bruno Haible <bruno@clisp.org>, 2024.  */

#include <config.h>

/* Specification.  */
#include "sfl-istream.h"

void
sfl_istream_init_from_file (sfl_istream_t *stream, FILE *fp)
{
  sf_istream_init_from_file (&stream->istream, fp);
  stream->line_number = 1;
}

void
sfl_istream_init_from_string (sfl_istream_t *stream, const char *input)
{
  sf_istream_init_from_string (&stream->istream, input);
  stream->line_number = 1;
}

void
sfl_istream_init_from_string_desc (sfl_istream_t *stream, string_desc_t input)
{
  sf_istream_init_from_string_desc (&stream->istream, input);
  stream->line_number = 1;
}

void
sfl_set_line_number (sfl_istream_t *stream, size_t line_number)
{
  stream->line_number = line_number;
}

size_t
sfl_get_line_number (sfl_istream_t *stream)
{
  return stream->line_number;
}

int
sfl_getc (sfl_istream_t *stream)
{
  int c = sf_getc (&stream->istream);
  if (c == '\n')
    stream->line_number++;
  return c;
}

int
sfl_ferror (sfl_istream_t *stream)
{
  return sf_ferror (&stream->istream);
}

void
sfl_ungetc (sfl_istream_t *stream, int c)
{
  if (c == '\n')
    stream->line_number--;
  sf_ungetc (&stream->istream, c);
}

void
sfl_free (sfl_istream_t *stream)
{
  sf_free (&stream->istream);
}
