/* Test of character handling in C locale.
   Copyright (C) 2005, 2007-2015 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2005.  */

#include <config.h>

#include "c-ctype.h"

#include <ctype.h>
#include <limits.h>
#include <locale.h>

#include "macros.h"

enum { NCHARS = UCHAR_MAX + 1 };

static char
to_char (int c)
{
  if (CHAR_MIN < 0 && CHAR_MAX < c)
    return c - CHAR_MAX - 1 + CHAR_MIN;
  return c;
}

static void
test_agree_with_C_locale (void)
{
  int c;

  for (c = 0; c <= UCHAR_MAX; c++)
    {
      ASSERT (c_isascii (c) == (isascii (c) != 0));
      if (c_isascii (c))
        {
          ASSERT (c_isalnum (c) == (isalnum (c) != 0));
          ASSERT (c_isalpha (c) == (isalpha (c) != 0));
          ASSERT (c_isblank (c) == (isblank (c) != 0));
          ASSERT (c_iscntrl (c) == (iscntrl (c) != 0));
          ASSERT (c_isdigit (c) == (isdigit (c) != 0));
          ASSERT (c_islower (c) == (islower (c) != 0));
          ASSERT (c_isgraph (c) == (isgraph (c) != 0));
          ASSERT (c_isprint (c) == (isprint (c) != 0));
          ASSERT (c_ispunct (c) == (ispunct (c) != 0));
          ASSERT (c_isspace (c) == (isspace (c) != 0));
          ASSERT (c_isupper (c) == (isupper (c) != 0));
          ASSERT (c_isxdigit (c) == (isxdigit (c) != 0));
          ASSERT (c_tolower (c) == tolower (c));
          ASSERT (c_toupper (c) == toupper (c));
        }
    }
}

static void
test_all (void)
{
  int c;
  int n_isascii = 0;

  for (c = CHAR_MIN; c <= UCHAR_MAX; c++)
    {
      if (c < 0)
        {
          ASSERT (c_isascii (c) == c_isascii (c + NCHARS));
          ASSERT (c_isalnum (c) == c_isalnum (c + NCHARS));
          ASSERT (c_isalpha (c) == c_isalpha (c + NCHARS));
          ASSERT (c_isblank (c) == c_isblank (c + NCHARS));
          ASSERT (c_iscntrl (c) == c_iscntrl (c + NCHARS));
          ASSERT (c_isdigit (c) == c_isdigit (c + NCHARS));
          ASSERT (c_islower (c) == c_islower (c + NCHARS));
          ASSERT (c_isgraph (c) == c_isgraph (c + NCHARS));
          ASSERT (c_isprint (c) == c_isprint (c + NCHARS));
          ASSERT (c_ispunct (c) == c_ispunct (c + NCHARS));
          ASSERT (c_isspace (c) == c_isspace (c + NCHARS));
          ASSERT (c_isupper (c) == c_isupper (c + NCHARS));
          ASSERT (c_isxdigit (c) == c_isxdigit (c + NCHARS));
          ASSERT (to_char (c_tolower (c)) == to_char (c_tolower (c + NCHARS)));
          ASSERT (to_char (c_toupper (c)) == to_char (c_toupper (c + NCHARS)));
        }

      if (0 <= c)
        n_isascii += c_isascii (c);

      ASSERT (c_isascii (c) == (c_isprint (c) || c_iscntrl (c)));

      ASSERT (c_isalnum (c) == (c_isalpha (c) || c_isdigit (c)));

      ASSERT (c_isalpha (c) == (c_islower (c) || c_isupper (c)));

      switch (to_char (c))
        {
        case '\t': case ' ':
          ASSERT (c_isblank (c) == 1);
          break;
        default:
          ASSERT (c_isblank (c) == 0);
          break;
        }

#ifdef C_CTYPE_ASCII
      ASSERT (c_iscntrl (c) == ((c >= 0 && c < 0x20) || c == 0x7f));
#endif

      ASSERT (! (c_iscntrl (c) && c_isprint (c)));

      switch (to_char (c))
        {
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
          ASSERT (c_isdigit (c) == 1);
          break;
        default:
          ASSERT (c_isdigit (c) == 0);
          break;
        }

      switch (to_char (c))
        {
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
          ASSERT (c_islower (c) == 1);
          break;
        default:
          ASSERT (c_islower (c) == 0);
          break;
        }

#ifdef C_CTYPE_ASCII
      ASSERT (c_isgraph (c) == ((c >= 0x20 && c < 0x7f) && c != ' '));

      ASSERT (c_isprint (c) == (c >= 0x20 && c < 0x7f));
#endif

      ASSERT (c_isgraph (c) == (c_isalnum (c) || c_ispunct (c)));

      ASSERT (c_isprint (c) == (c_isgraph (c) || c == ' '));

      switch (to_char (c))
        {
        case '!': case '"': case '#': case '$': case '%': case '&': case '\'':
        case '(': case ')': case '*': case '+': case ',': case '-': case '.':
        case '/': case ':': case ';': case '<': case '=': case '>': case '?':
        case '@': case '[': case'\\': case ']': case '^': case '_': case '`':
        case '{': case '|': case '}': case '~':
          ASSERT (c_ispunct (c) == 1);
          break;
        default:
          ASSERT (c_ispunct (c) == 0);
          break;
        }

      switch (to_char (c))
        {
        case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
          ASSERT (c_isspace (c) == 1);
          break;
        default:
          ASSERT (c_isspace (c) == 0);
          break;
        }

      switch (to_char (c))
        {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z':
          ASSERT (c_isupper (c) == 1);
          break;
        default:
          ASSERT (c_isupper (c) == 0);
          break;
        }

      switch (to_char (c))
        {
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
          ASSERT (c_isxdigit (c) == 1);
          break;
        default:
          ASSERT (c_isxdigit (c) == 0);
          break;
        }

      switch (to_char (c))
        {
        case 'A': ASSERT (to_char (c_tolower (c)) == 'a'); break;
        case 'B': ASSERT (to_char (c_tolower (c)) == 'b'); break;
        case 'C': ASSERT (to_char (c_tolower (c)) == 'c'); break;
        case 'D': ASSERT (to_char (c_tolower (c)) == 'd'); break;
        case 'E': ASSERT (to_char (c_tolower (c)) == 'e'); break;
        case 'F': ASSERT (to_char (c_tolower (c)) == 'f'); break;
        case 'G': ASSERT (to_char (c_tolower (c)) == 'g'); break;
        case 'H': ASSERT (to_char (c_tolower (c)) == 'h'); break;
        case 'I': ASSERT (to_char (c_tolower (c)) == 'i'); break;
        case 'J': ASSERT (to_char (c_tolower (c)) == 'j'); break;
        case 'K': ASSERT (to_char (c_tolower (c)) == 'k'); break;
        case 'L': ASSERT (to_char (c_tolower (c)) == 'l'); break;
        case 'M': ASSERT (to_char (c_tolower (c)) == 'm'); break;
        case 'N': ASSERT (to_char (c_tolower (c)) == 'n'); break;
        case 'O': ASSERT (to_char (c_tolower (c)) == 'o'); break;
        case 'P': ASSERT (to_char (c_tolower (c)) == 'p'); break;
        case 'Q': ASSERT (to_char (c_tolower (c)) == 'q'); break;
        case 'R': ASSERT (to_char (c_tolower (c)) == 'r'); break;
        case 'S': ASSERT (to_char (c_tolower (c)) == 's'); break;
        case 'T': ASSERT (to_char (c_tolower (c)) == 't'); break;
        case 'U': ASSERT (to_char (c_tolower (c)) == 'u'); break;
        case 'V': ASSERT (to_char (c_tolower (c)) == 'v'); break;
        case 'W': ASSERT (to_char (c_tolower (c)) == 'w'); break;
        case 'X': ASSERT (to_char (c_tolower (c)) == 'x'); break;
        case 'Y': ASSERT (to_char (c_tolower (c)) == 'y'); break;
        case 'Z': ASSERT (to_char (c_tolower (c)) == 'z'); break;
        default: ASSERT (c_tolower (c) == c); break;
        }

      switch (to_char (c))
        {
        case 'a': ASSERT (to_char (c_toupper (c)) == 'A'); break;
        case 'b': ASSERT (to_char (c_toupper (c)) == 'B'); break;
        case 'c': ASSERT (to_char (c_toupper (c)) == 'C'); break;
        case 'd': ASSERT (to_char (c_toupper (c)) == 'D'); break;
        case 'e': ASSERT (to_char (c_toupper (c)) == 'E'); break;
        case 'f': ASSERT (to_char (c_toupper (c)) == 'F'); break;
        case 'g': ASSERT (to_char (c_toupper (c)) == 'G'); break;
        case 'h': ASSERT (to_char (c_toupper (c)) == 'H'); break;
        case 'i': ASSERT (to_char (c_toupper (c)) == 'I'); break;
        case 'j': ASSERT (to_char (c_toupper (c)) == 'J'); break;
        case 'k': ASSERT (to_char (c_toupper (c)) == 'K'); break;
        case 'l': ASSERT (to_char (c_toupper (c)) == 'L'); break;
        case 'm': ASSERT (to_char (c_toupper (c)) == 'M'); break;
        case 'n': ASSERT (to_char (c_toupper (c)) == 'N'); break;
        case 'o': ASSERT (to_char (c_toupper (c)) == 'O'); break;
        case 'p': ASSERT (to_char (c_toupper (c)) == 'P'); break;
        case 'q': ASSERT (to_char (c_toupper (c)) == 'Q'); break;
        case 'r': ASSERT (to_char (c_toupper (c)) == 'R'); break;
        case 's': ASSERT (to_char (c_toupper (c)) == 'S'); break;
        case 't': ASSERT (to_char (c_toupper (c)) == 'T'); break;
        case 'u': ASSERT (to_char (c_toupper (c)) == 'U'); break;
        case 'v': ASSERT (to_char (c_toupper (c)) == 'V'); break;
        case 'w': ASSERT (to_char (c_toupper (c)) == 'W'); break;
        case 'x': ASSERT (to_char (c_toupper (c)) == 'X'); break;
        case 'y': ASSERT (to_char (c_toupper (c)) == 'Y'); break;
        case 'z': ASSERT (to_char (c_toupper (c)) == 'Z'); break;
        default: ASSERT (c_toupper (c) == c); break;
        }
    }

  ASSERT (n_isascii == 128);
}

int
main ()
{
  test_agree_with_C_locale ();

  test_all ();

  setlocale (LC_ALL, "de_DE");
  test_all ();

  setlocale (LC_ALL, "ja_JP.EUC-JP");
  test_all ();

  return 0;
}
