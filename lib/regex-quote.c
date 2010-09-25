/* Construct a regular expression from a literal string.
   Copyright (C) 1995, 2010 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2010.

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

#include <config.h>

/* Specification.  */
#include "regex-quote.h"

#include <string.h>

#include "mbuiter.h"
#include "xalloc.h"

/* Characters that are special in a BRE.  */
static const char bre_special[] = "$^.*[]\\";

/* Characters that are special in an ERE.  */
static const char ere_special[] = "$^.*[]\\+?()";

size_t
regex_quote_length (const char *string, int cflags)
{
  const char *special = (cflags != 0 ? ere_special : bre_special);
  size_t length;
  mbui_iterator_t iter;

  length = 0;
  for (mbui_init (iter, string); mbui_avail (iter); mbui_advance (iter))
    {
      /* We know that special contains only ASCII characters.  */
      if (mb_len (mbui_cur (iter)) == 1
          && strchr (special, * mbui_cur_ptr (iter)))
        length += 1;
      length += mb_len (mbui_cur (iter));
    }
  return length;
}

/* Copies the quoted string to p and returns the incremented p.
   There must be room for regex_quote_length (string, cflags) + 1 bytes at p.
 */
char *
regex_quote_copy (char *p, const char *string, int cflags)
{
  const char *special = (cflags != 0 ? ere_special : bre_special);
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
  return p;
}

/* Returns the freshly allocated quoted string.  */
char *
regex_quote (const char *string, int cflags)
{
  size_t length = regex_quote_length (string, cflags);
  char *result = XNMALLOC (length + 1, char);
  char *p;

  p = result;
  p = regex_quote_copy (p, string, cflags);
  *p = '\0';
  return result;
}
