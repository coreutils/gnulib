/* Grapheme cluster length test.
   Copyright (C) 2010-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Ben Pfaff <blp@cs.stanford.edu>, 2010. */

#include <config.h>

/* Specification. */
#include <unigbrk.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_u32_grapheme_len (size_t len, ...)
{
  uint32_t s[32];
  size_t retval;
  va_list args;
  size_t n;

  va_start (args, len);
  n = 0;
  for (;;)
    {
      int unit = va_arg (args, int);
      if (unit == -1)
        break;
      else if (n >= sizeof s / sizeof *s)
        abort ();

      s[n++] = unit;
    }
  va_end (args);

  retval = u32_grapheme_len (s, n);
  if (retval != len)
    {
      size_t i;

      fprintf (stderr, "u32_grapheme_len counted %zu units, expected %zu:",
               retval, len);
      for (i = 0; i < n; i++)
        fprintf (stderr, " %04x", (unsigned int) s[i]);
      putc ('\n', stderr);
      abort ();
    }
}


int
main (void)
{
  /* Empty string. */
  test_u32_grapheme_len (0, -1);

  /* Standalone 1-unit graphemes.  */
  test_u32_grapheme_len (1, 'a', -1);
  test_u32_grapheme_len (1, 'a', 'b', -1);
  test_u32_grapheme_len (1, 'a', 'b', 'c', -1);

  /* Multi-unit, single code point graphemes. */
#define HIRAGANA_A 0x3042       /* あ: Hiragana letter 'a'. */
  test_u32_grapheme_len (1, HIRAGANA_A, -1);
  test_u32_grapheme_len (1, HIRAGANA_A, 'x', -1);
  test_u32_grapheme_len (1, HIRAGANA_A, HIRAGANA_A, -1);

  /* Combining accents. */
#define GRAVE 0x0300            /* Combining grave accent. */
#define ACUTE 0x0301            /* Combining acute accent. */
  test_u32_grapheme_len (2, 'e', ACUTE, -1);
  test_u32_grapheme_len (3, 'e', ACUTE, GRAVE, -1);
  test_u32_grapheme_len (2, 'e', ACUTE, 'x', -1);
  test_u32_grapheme_len (2, 'e', ACUTE, 'e', ACUTE, -1);

  /* Outside BMP. */
#define NEUTRAL_FACE 0x1f610    /* 😐: neutral face. */
  test_u32_grapheme_len (1, NEUTRAL_FACE, -1);
  test_u32_grapheme_len (2, NEUTRAL_FACE, GRAVE, -1);

  return 0;
}
