/* Test of isupper_l() function.
   Copyright (C) 2020-2025 Free Software Foundation, Inc.

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

#include <config.h>

#include <ctype.h>

#include "signature.h"
SIGNATURE_CHECK (isupper_l, int, (int, locale_t));

#include <locale.h>
#include <stdio.h>

#include "macros.h"

static void
test_single_locale_common (locale_t locale)
{
  int is;

  /* Test EOF.  */
  is = isupper_l (EOF, locale);
  ASSERT (is == 0);

  /* POSIX specifies in
       <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap07.html>
     that
       - in all locales, the uppercase characters include the A ... Z
         characters,
       - in the "POSIX" locale (which is usually the same as the "C" locale),
         the uppercase characters include only the ASCII A ... Z characters.
   */
  {
    int c;

    for (c = 0; c < 0x100; c++)
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
          is = isupper_l ((unsigned char) c, locale);
          switch (c)
            {
            case 'A': case 'B': case 'C': case 'D': case 'E':
            case 'F': case 'G': case 'H': case 'I': case 'J':
            case 'K': case 'L': case 'M': case 'N': case 'O':
            case 'P': case 'Q': case 'R': case 'S': case 'T':
            case 'U': case 'V': case 'W': case 'X': case 'Y':
            case 'Z':
              ASSERT (is != 0);
              break;
            default:
              ASSERT (is == 0);
              break;
            }
          break;
        }
  }
}

int
main ()
{
  {
    locale_t locale = newlocale (LC_ALL_MASK, "C", NULL);
    ASSERT (locale != NULL);

    test_single_locale_common (locale);

    freelocale (locale);
  }
#if !MUSL_LIBC /* musl libc has no unibyte locales */
  {
# if defined _WIN32 && !defined __CYGWIN__
    locale_t locale = newlocale (LC_ALL_MASK, "French_France.1252", NULL);
# else
    locale_t locale = newlocale (LC_ALL_MASK, "fr_FR.ISO-8859-1", NULL);
    if (locale == NULL)
      locale = newlocale (LC_ALL_MASK, "fr_FR.ISO8859-1", NULL);
# endif
    if (locale != NULL)
      {
        test_single_locale_common (locale);

        /* Locale encoding is ISO-8859-1 or ISO-8859-15.  */
        int is;

        /* U+00B2 SUPERSCRIPT TWO */
        is = isupper_l ((unsigned char) '\262', locale);
        ASSERT (is == 0);
        /* U+00B5 MICRO SIGN */
        is = isupper_l ((unsigned char) '\265', locale);
        ASSERT (is == 0);
        /* U+00C9 LATIN CAPITAL LETTER E WITH ACUTE */
        is = isupper_l ((unsigned char) '\311', locale);
        ASSERT (is != 0);
      #if !defined __hpux
        /* U+00DF LATIN SMALL LETTER SHARP S */
        is = isupper_l ((unsigned char) '\337', locale);
        ASSERT (is == 0);
      #endif
        /* U+00E9 LATIN SMALL LETTER E WITH ACUTE */
        is = isupper_l ((unsigned char) '\351', locale);
        ASSERT (is == 0);
        /* U+00FF LATIN SMALL LETTER Y WITH DIAERESIS */
        is = isupper_l ((unsigned char) '\377', locale);
        ASSERT (is == 0);

        freelocale (locale);
      }
  }
#endif

  return test_exit_status;
}
