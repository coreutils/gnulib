/* Character handling in C locale.

   Copyright 2000-2003, 2006, 2009-2015 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#define NO_C_CTYPE_MACROS
#include "c-ctype.h"

#include <limits.h>
#include "verify.h"

#ifndef C_CTYPE_ASCII
enum { C_CTYPE_ASCII = false };
#endif
#ifndef C_CTYPE_CONSECUTIVE_DIGITS
enum { C_CTYPE_CONSECUTIVE_DIGITS = false };
#endif
#ifndef C_CTYPE_CONSECUTIVE_LOWERCASE
enum { C_CTYPE_CONSECUTIVE_LOWERCASE = false };
#endif
#ifndef C_CTYPE_CONSECUTIVE_UPPERCASE
enum { C_CTYPE_CONSECUTIVE_UPPERCASE = false };
#endif

/* Convert an int, which may be promoted from either an unsigned or a
   signed char, to the corresponding char.  */

static char
to_char (int c)
{
  enum { nchars = CHAR_MAX - CHAR_MIN + 1 };
  if (CHAR_MIN < 0 && CHAR_MAX < c && c < nchars)
    return c - nchars;
  return c;
}

/* The function isascii is not locale dependent. Its use in EBCDIC is
   questionable. */
bool
c_isascii (int c)
{
  return (c >= 0x00 && c <= 0x7f);
}

bool
c_isalnum (int c)
{
  if (C_CTYPE_CONSECUTIVE_DIGITS
      && C_CTYPE_CONSECUTIVE_UPPERCASE
      && C_CTYPE_CONSECUTIVE_LOWERCASE)
    {
      if (C_CTYPE_ASCII)
        return (('0' <= c && c <= '9')
                || ('A' <= (c & ~0x20) && (c & ~0x20) <= 'Z'));
      else
        return (('0' <= c && c <= '9')
                || ('A' <= c && c <= 'Z')
                || ('a' <= c && c <= 'z'));
    }

  switch (to_char (c))
    {
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
      return true;
    default:
      return false;
    }
}

bool
c_isalpha (int c)
{
  if (C_CTYPE_CONSECUTIVE_UPPERCASE && C_CTYPE_CONSECUTIVE_LOWERCASE)
    {
      if (C_CTYPE_ASCII)
        return 'A' <= (c & ~0x20) && (c & ~0x20) <= 'Z';
      else
        return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
    }

  switch (to_char (c))
    {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
      return true;
    default:
      return false;
    }
}

bool
c_isblank (int c)
{
  return (c == ' ' || c == '\t');
}

bool
c_iscntrl (int c)
{
  enum { C_CTYPE_EBCDIC = (' ' == '\x40' && '0' == '\xf0'
                           && 'A' == '\xc1' && 'J' == '\xd1' && 'S' == '\xe2'
                           && 'a' == '\x81' && 'j' == '\x91' && 's' == '\xa2') };
  if (C_CTYPE_ASCII)
    return (0 <= c && c < ' ') || c == 0x7f;
  else
    {
      /* Return true if C corresponds to an ASCII control character.
         Assume EBCDIC code page 1047, and verify that the compiler
         agrees with this.  */
      verify (C_CTYPE_ASCII
              || (C_CTYPE_EBCDIC
                  && '!' == '\x5a' && '#' == '\x7b' && '$' == '\x5b'
                  && '@' == '\x7c' && '[' == '\xad' && '\\' == '\xe0'
                  && ']' == '\xbd' && '^' == '\x5f' && '_' == '\x6d'
                  && '`' == '\x79'));
      switch (c)
        {
        case '\x00': case '\x01': case '\x02': case '\x03': case '\x05':
        case '\x0b': case '\x0c': case '\x0d': case '\x0e': case '\x0f':
        case '\x10': case '\x11': case '\x12': case '\x13': case '\x15':
        case '\x16': case '\x18': case '\x19': case '\x1c': case '\x1d':
        case '\x1e': case '\x1f': case '\x26': case '\x27': case '\x2d':
        case '\x2e': case '\x2f': case '\x32': case '\x37': case '\x3c':
        case '\x3d': case '\x3f': case '\xff':
        case '\xff' < 0 ? 0xff : -1:
          return true;
        default:
          return false;
        }
    }
}

bool
c_isdigit (int c)
{
  if (C_CTYPE_ASCII)
    return '0' <= c && c <= '9';

  c = to_char (c);
  if (C_CTYPE_CONSECUTIVE_DIGITS)
    return '0' <= c && c <= '9';

  switch (c)
    {
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
      return true;
    default:
      return false;
    }
}

bool
c_islower (int c)
{
  if (C_CTYPE_CONSECUTIVE_LOWERCASE)
    return 'a' <= c && c <= 'z';

  switch (to_char (c))
    {
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
      return true;
    default:
      return false;
    }
}

bool
c_isgraph (int c)
{
  if (C_CTYPE_ASCII)
    return '!' <= c && c <= '~';

  switch (to_char (c))
    {
    case '!': case '"': case '#': case '$': case '%': case '&':
    case '\'': case '(': case ')': case '*': case '+': case ',':
    case '-': case '.': case '/':
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
    case ':': case ';': case '<': case '=': case '>': case '?':
    case '@':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '[': case '\\': case ']': case '^': case '_': case '`':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case '{': case '|': case '}': case '~':
      return true;
    default:
      return false;
    }
}

bool
c_isprint (int c)
{
  if (C_CTYPE_ASCII)
    return ' ' <= c && c <= '~';

  switch (to_char (c))
    {
    case ' ': case '!': case '"': case '#': case '$': case '%':
    case '&': case '\'': case '(': case ')': case '*': case '+':
    case ',': case '-': case '.': case '/':
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
    case ':': case ';': case '<': case '=': case '>': case '?':
    case '@':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
    case '[': case '\\': case ']': case '^': case '_': case '`':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case '{': case '|': case '}': case '~':
      return true;
    default:
      return false;
    }
}

bool
c_ispunct (int c)
{
  if (C_CTYPE_ASCII)
    return (('!' <= c && c <= '~')
            && !(('0' <= c && c <= '9')
                 || ('A' <= (c & ~0x20) && (c & ~0x20) <= 'Z')));

  switch (to_char (c))
    {
    case '!': case '"': case '#': case '$': case '%': case '&':
    case '\'': case '(': case ')': case '*': case '+': case ',':
    case '-': case '.': case '/':
    case ':': case ';': case '<': case '=': case '>': case '?':
    case '@':
    case '[': case '\\': case ']': case '^': case '_': case '`':
    case '{': case '|': case '}': case '~':
      return true;
    default:
      return false;
    }
}

bool
c_isspace (int c)
{
  return (c == ' ' || c == '\t'
          || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

bool
c_isupper (int c)
{
  if (C_CTYPE_CONSECUTIVE_UPPERCASE)
    return 'A' <= c && c <= 'Z';

  switch (to_char (c))
    {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z':
      return true;
    default:
      return false;
    }
}

bool
c_isxdigit (int c)
{
  if (C_CTYPE_CONSECUTIVE_DIGITS
      && C_CTYPE_CONSECUTIVE_UPPERCASE
      && C_CTYPE_CONSECUTIVE_LOWERCASE)
    {
      if ('0' <= c && c <= '9')
        return true;
      if (C_CTYPE_ASCII)
        return 'A' <= (c & ~0x20) && (c & ~0x20) <= 'F';
      return (('A' <= c && c <= 'F')
              || ('a' <= c && c <= 'f'));
    }

  switch (to_char (c))
    {
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return true;
    default:
      return false;
    }
}

int
c_tolower (int c)
{
  if (C_CTYPE_CONSECUTIVE_UPPERCASE && C_CTYPE_CONSECUTIVE_LOWERCASE)
    return c_isupper (c) ? c - 'A' + 'a' : c;

  switch (to_char (c))
    {
    case 'A': return 'a';
    case 'B': return 'b';
    case 'C': return 'c';
    case 'D': return 'd';
    case 'E': return 'e';
    case 'F': return 'f';
    case 'G': return 'g';
    case 'H': return 'h';
    case 'I': return 'i';
    case 'J': return 'j';
    case 'K': return 'k';
    case 'L': return 'l';
    case 'M': return 'm';
    case 'N': return 'n';
    case 'O': return 'o';
    case 'P': return 'p';
    case 'Q': return 'q';
    case 'R': return 'r';
    case 'S': return 's';
    case 'T': return 't';
    case 'U': return 'u';
    case 'V': return 'v';
    case 'W': return 'w';
    case 'X': return 'x';
    case 'Y': return 'y';
    case 'Z': return 'z';
    default: return c;
    }
}

int
c_toupper (int c)
{
  if (C_CTYPE_CONSECUTIVE_UPPERCASE && C_CTYPE_CONSECUTIVE_LOWERCASE)
    return c_islower (c) ? c - 'a' + 'A' : c;

  switch (to_char (c))
    {
    case 'a': return 'A';
    case 'b': return 'B';
    case 'c': return 'C';
    case 'd': return 'D';
    case 'e': return 'E';
    case 'f': return 'F';
    case 'g': return 'G';
    case 'h': return 'H';
    case 'i': return 'I';
    case 'j': return 'J';
    case 'k': return 'K';
    case 'l': return 'L';
    case 'm': return 'M';
    case 'n': return 'N';
    case 'o': return 'O';
    case 'p': return 'P';
    case 'q': return 'Q';
    case 'r': return 'R';
    case 's': return 'S';
    case 't': return 'T';
    case 'u': return 'U';
    case 'v': return 'V';
    case 'w': return 'W';
    case 'x': return 'X';
    case 'y': return 'Y';
    case 'z': return 'Z';
    default: return c;
    }
}
