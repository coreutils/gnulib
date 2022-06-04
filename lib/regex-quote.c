/* Construct a regular expression from a literal string.
   Copyright (C) 1995, 2010-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2010.

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

/* Specification.  */
#include "regex-quote.h"

#include <string.h>

#include "mbuiter.h"
#include "xalloc.h"

/* Characters that are special in a BRE.  */
static const char bre_special[] = "$^.*[\\";

/* Characters that are special in an ERE.  */
static const char ere_special[] = "$^.*[\\+?{()|";

struct regex_quote_spec
regex_quote_spec_posix (int cflags, bool anchored)
{
  struct regex_quote_spec result;

  strcpy (result.special, cflags != 0 ? ere_special : bre_special);
  result.multibyte = true;
  result.anchored = anchored;

  return result;
}

/* Syntax bit values, defined in GNU <regex.h>.  We don't include it here,
   otherwise this module would need to depend on gnulib module 'regex'.  */
#define RE_BK_PLUS_QM    0x00000002
#define RE_INTERVALS     0x00000200
#define RE_LIMITED_OPS   0x00000400
#define RE_NEWLINE_ALT   0x00000800
#define RE_NO_BK_BRACES  0x00001000
#define RE_NO_BK_PARENS  0x00002000
#define RE_NO_BK_VBAR    0x00008000

struct regex_quote_spec
regex_quote_spec_gnu (unsigned long /*reg_syntax_t*/ syntax, bool anchored)
{
  struct regex_quote_spec result;
  char *p;

  p = result.special;
  memcpy (p, bre_special, sizeof (bre_special) - 1);
  p += sizeof (bre_special) - 1;
  if ((syntax & RE_LIMITED_OPS) == 0 && (syntax & RE_BK_PLUS_QM) == 0)
    {
      *p++ = '+';
      *p++ = '?';
    }
  if ((syntax & RE_INTERVALS) != 0 && (syntax & RE_NO_BK_BRACES) != 0)
    {
      *p++ = '{';
      *p++ = '}';
    }
  if ((syntax & RE_NO_BK_PARENS) != 0)
    {
      *p++ = '(';
      *p++ = ')';
    }
  if ((syntax & RE_LIMITED_OPS) == 0 && (syntax & RE_NO_BK_VBAR) != 0)
    *p++ = '|';
  if ((syntax & RE_NEWLINE_ALT) != 0)
    *p++ = '\n';
  *p = '\0';

  result.multibyte = true;
  result.anchored = anchored;

  return result;
}

/* Characters that are special in a PCRE.  */
static const char pcre_special[] = "$^.*[]\\+?{}()|";

/* Options bit values, defined in <pcre.h>.  We don't include it here, because
   it is not a standard header.  */
#define PCRE_ANCHORED 0x00000010
#define PCRE_EXTENDED 0x00000008

struct regex_quote_spec
regex_quote_spec_pcre (int options, bool anchored)
{
  struct regex_quote_spec result;
  char *p;

  p = result.special;
  memcpy (p, pcre_special, sizeof (pcre_special) - 1);
  p += sizeof (pcre_special) - 1;
  if (options & PCRE_EXTENDED)
    {
      *p++ = ' ';
      *p++ = '\t';
      *p++ = '\n';
      *p++ = '\v';
      *p++ = '\f';
      *p++ = '\r';
      *p++ = '#';
    }
  *p = '\0';

  /* PCRE regular expressions consist of UTF-8 characters of options contains
     PCRE_UTF8 and of single bytes otherwise.  */
  result.multibyte = false;
  /* If options contains PCRE_ANCHORED, the anchoring is implicit.  */
  result.anchored = (options & PCRE_ANCHORED ? 0 : anchored);

  return result;
}

size_t
regex_quote_length (const char *string, const struct regex_quote_spec *spec)
{
  const char *special = spec->special;
  size_t length;

  length = 0;
  if (spec->anchored)
    length += 2; /* for '^' at the beginning and '$' at the end */
  if (spec->multibyte)
    {
      mbui_iterator_t iter;

      for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
        {
          /* We know that special contains only ASCII characters.  */
          if (mb_len (mbui_cur (iter)) == 1
              && strchr (special, * mbui_cur_ptr (iter)))
            length += 1;
          length += mb_len (mbui_cur (iter));
        }
    }
  else
    {
      const char *iter;

      for (iter = string; *iter != '\0'; iter++)
        {
          if (strchr (special, *iter))
            length += 1;
          length += 1;
        }
    }

  return length;
}

char *
regex_quote_copy (char *p, const char *string, const struct regex_quote_spec *spec)
{
  const char *special = spec->special;

  if (spec->anchored)
    *p++ = '^';
  if (spec->multibyte)
    {
      mbui_iterator_t iter;

      for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
        {
          /* We know that special contains only ASCII characters.  */
          if (mb_len (mbui_cur (iter)) == 1
              && strchr (special, * mbui_cur_ptr (iter)))
            *p++ = '\\';
          memcpy (p, mbui_cur_ptr (iter), mb_len (mbui_cur (iter)));
          p += mb_len (mbui_cur (iter));
        }
    }
  else
    {
      const char *iter;

      for (iter = string; *iter != '\0'; iter++)
        {
          if (strchr (special, *iter))
            *p++ = '\\';
          *p++ = *iter++;
        }
    }
  if (spec->anchored)
    *p++ = '$';

  return p;
}

char *
regex_quote (const char *string, const struct regex_quote_spec *spec)
{
  size_t length = regex_quote_length (string, spec);
  char *result = XNMALLOC (length + 1, char);
  char *p;

  p = result;
  p = regex_quote_copy (p, string, spec);
  *p = '\0';
  return result;
}
