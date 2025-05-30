/* Uppercase mapping for UTF-8 strings (locale dependent).
   Copyright (C) 2009-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unicase.h"

#include <stddef.h>

#include "unicase/unicasemap.h"
#include "unicase/special-casing.h"

uint8_t *
u8_toupper (const uint8_t *s, size_t n, const char *iso639_language,
            uninorm_t nf,
            uint8_t *resultbuf, size_t *lengthp)
{
  return u8_casemap (s, n,
                     unicase_empty_prefix_context, unicase_empty_suffix_context,
                     iso639_language,
                     uc_toupper, offsetof (struct special_casing_rule, upper[0]),
                     nf,
                     resultbuf, lengthp);
}


#ifdef TEST

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Read the contents of an input stream, and return it, terminated with a NUL
   byte. */
char *
read_file (FILE *stream)
{
#define BUFSIZE 4096
  char *buf = NULL;
  int alloc = 0;
  int size = 0;
  int count;

  while (! feof (stream))
    {
      if (size + BUFSIZE > alloc)
        {
          alloc = alloc + alloc / 2;
          if (alloc < size + BUFSIZE)
            alloc = size + BUFSIZE;
          buf = realloc (buf, alloc);
          if (buf == NULL)
            {
              fprintf (stderr, "out of memory\n");
              exit (1);
            }
        }
      count = fread (buf + size, 1, BUFSIZE, stream);
      if (count == 0)
        {
          if (ferror (stream))
            {
              perror ("fread");
              exit (1);
            }
        }
      else
        size += count;
    }
  buf = realloc (buf, size + 1);
  if (buf == NULL)
    {
      fprintf (stderr, "out of memory\n");
      exit (1);
    }
  buf[size] = '\0';
  return buf;
#undef BUFSIZE
}

int
main (int argc, char * argv[])
{
  setlocale (LC_ALL, "");
  if (argc == 1)
    {
      /* Display the upper case of the input string.  */
      char *input = read_file (stdin);
      int length = strlen (input);
      size_t output_length;
      uint8_t *output =
        u8_toupper ((uint8_t *) input, length, uc_locale_language (),
                    NULL,
                    NULL, &output_length);

      fwrite (output, 1, output_length, stdout);

      return 0;
    }
  else
    return 1;
}

#endif /* TEST */
