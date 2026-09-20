/* Test of conversion of wide character to unibyte character.
   Copyright (C) 2008-2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

#include <wchar.h>

#include "signature.h"
SIGNATURE_CHECK (wctob, int, (wint_t));

#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

/* Check the unibyte character c.  */
static void
check_character (unsigned char c)
{
  wint_t wc = btowc (c);
  ASSERT (wc != WEOF);
  int cc = wctob (wc);
  ASSERT (cc == c);
}

int
main (int argc, char *argv[])
{
  /* configure should already have checked that the locale is supported.  */
  if (setlocale (LC_ALL, "") == NULL)
    return 1;

  /* Test NUL character.  */
  ASSERT (wctob (0) == 0);

  /* Test single bytes.  */
  for (int c = 0; c < 0x100; c++)
    switch (c)
      {
      case '\t': case '\v': case '\f':
      case ' ': case '!': case '"': case '#': case '%':
      case '&': case '\'': case '(': case ')': case '*':
      case '+': case ',': case '-': case '.': case '/':
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case ':': case ';': case '<': case '=': case '>':
      case '?':
      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y':
      case 'Z':
      case '[': case '\\': case ']': case '^': case '_':
      case 'a': case 'b': case 'c': case 'd': case 'e':
      case 'f': case 'g': case 'h': case 'i': case 'j':
      case 'k': case 'l': case 'm': case 'n': case 'o':
      case 'p': case 'q': case 'r': case 's': case 't':
      case 'u': case 'v': case 'w': case 'x': case 'y':
      case 'z': case '{': case '|': case '}': case '~':
        /* c is in the ISO C "basic character set".  */
        check_character (c);
        break;
      }

  if (argc > 1)
    switch (argv[1][0])
      {
      case '1':
        /* C locale; tested above.  */
#if !defined __ANDROID__
        /* On Android ≥ 5.0, the default locale is the "C.UTF-8" locale, not the
           "C" locale.  Furthermore, when you attempt to set the "C" or "POSIX"
           locale via setlocale(), what you get is a "C" locale with UTF-8
           encoding, that is, effectively the "C.UTF-8" locale.  */
        /* Check that wctob does the inverse of btowc, in the C locale.
           Above we have only tested the ISO C "basic character set".  */
        for (int c = 0; c < 0x100; c++)
          check_character (c);
#endif
        return test_exit_status;

      case '2':
        /* Locale encoding is ISO-8859-1 or ISO-8859-15.  */
        return test_exit_status;

      case '3':
        /* Locale encoding is UTF-8.  */
        ASSERT (wctob (0x0091) == EOF);
        ASSERT (wctob (0x00DF) == EOF);
        ASSERT (wctob (0x00FC) == EOF);
        ASSERT (wctob (0x1F60B) == EOF);
        return test_exit_status;

      case '4':
        /* Locale encoding is EUC-JP.  */
        return test_exit_status;

      case '5':
        /* Locale encoding is GB18030.  */
        return test_exit_status;
      }

  return 1;
}
