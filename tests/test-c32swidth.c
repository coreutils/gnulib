/* Test of c32swidth() function.
   Copyright (C) 2023-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include <uchar.h>

#include "signature.h"
SIGNATURE_CHECK (c32swidth, int, (const char32_t *, size_t));

#include <locale.h>
#include <string.h>

#include "localcharset.h"
#include "macros.h"

int
main ()
{
  int w;

  /* Switch to an UTF-8 locale.  */
  if (!((setlocale (LC_ALL, "fr_FR.UTF-8") != NULL
         || setlocale (LC_ALL, "de_DE.UTF-8") != NULL
         || setlocale (LC_ALL, "es_ES.UTF-8") != NULL
         || setlocale (LC_ALL, "en_US.UTF-8") != NULL)
        /* Check whether it's really an UTF-8 locale.
           On native Windows, these setlocale calls succeed but the encoding
           of this locale is CP1252, not UTF-8.  */
        && strcmp (locale_charset (), "UTF-8") == 0))
    {
      fprintf (stderr, "Skipping test: No common Unicode locale is installed\n");
      return 77;
    }

  {
    char32_t s[] = { 'f', 'p', 0, 'x' };
    w = c32swidth (s, 0);
    ASSERT (w == 0);
    w = c32swidth (s, 1);
    ASSERT (w == 1);
    w = c32swidth (s, 2);
    ASSERT (w == 2);
    w = c32swidth (s, 3);
    ASSERT (w == 2);
    w = c32swidth (s, 4);
    ASSERT (w == 2);
    w = c32swidth (s, (size_t)(-1));
    ASSERT (w == 2);
  }

  {
    char32_t s[] = { 'f', 'p', '\n', 'x' };
    w = c32swidth (s, 0);
    ASSERT (w == 0);
    w = c32swidth (s, 1);
    ASSERT (w == 1);
    w = c32swidth (s, 2);
    ASSERT (w == 2);
    w = c32swidth (s, 3);
    ASSERT (w == -1);
    w = c32swidth (s, 4);
    ASSERT (w == -1);
  }

  /* Test width of some non-spacing characters.  */
  {
    char32_t s[] = { 'a', 0x0301 };
    w = c32swidth (s, 2);
    ASSERT (w == 1);
  }

  /* Test width of some zero width characters.  */
  {
    char32_t s[] = { 'i', 0x200B, 'j' };
    w = c32swidth (s, 3);
    ASSERT (w == 2);
  }

  /* Test width of some math symbols.
     U+2202 is marked as having ambiguous width (A) in EastAsianWidth.txt
     (see <https://www.unicode.org/Public/12.0.0/ucd/EastAsianWidth.txt>).
     The Unicode Standard Annex 11
     <https://www.unicode.org/reports/tr11/tr11-36.html>
     says
       "Ambiguous characters behave like wide or narrow characters
        depending on the context (language tag, script identification,
        associated font, source of data, or explicit markup; all can
        provide the context). If the context cannot be established
        reliably, they should be treated as narrow characters by default."
     For c32width(), the only available context information is the locale.
     The chosen locale above is a Western locale, not an East Asian locale,
     therefore U+2202 should be treated like a narrow character.  */
  {
    char32_t s[] = { 0x2202 };
    w = c32swidth (s, 1);
    ASSERT (w == 1);
  }

  /* Test width of some CJK characters.  */
  {
    char32_t s[] = { 0x4E2D, 0x6587 };
    w = c32swidth (s, 2);
    ASSERT (w == 4);
  }
  {
    char32_t s[] = { 0x20369, 0x2F876 };
    w = c32swidth (s, 2);
    ASSERT (w == 4);
  }

  return test_exit_status;
}
