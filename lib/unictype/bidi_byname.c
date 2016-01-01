/* Bidi classes of Unicode characters.
   Copyright (C) 2002, 2006, 2011-2016 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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

#include <config.h>

/* Specification.  */
#include "unictype.h"

#include <string.h>

#include "unictype/bidi_byname.h"

int
uc_bidi_class_byname (const char *bidi_class_name)
{
  size_t len;

  len = strlen (bidi_class_name);
  if (len <= MAX_WORD_LENGTH)
    {
      char buf[MAX_WORD_LENGTH + 1];
      const struct named_bidi_class *found;

      /* Copy bidi_class_name into buf, converting '_' and '-' to ' '.  */
      {
        const char *p = bidi_class_name;
        char *q = buf;

        for (;; p++, q++)
          {
            char c = *p;

            if (c == '_' || c == '-')
              c = ' ';
            *q = c;
            if (c == '\0')
              break;
          }
      }
      /* Here q == buf + len.  */

      /* Do a hash table lookup, with case-insensitive comparison.  */
      found = uc_bidi_class_lookup (buf, len);
      if (found != NULL)
        return found->bidi_class;
    }
  /* Invalid bidi class name.  */
  return -1;
}

int
uc_bidi_category_byname (const char *category_name)
{
  return uc_bidi_class_byname (category_name);
}
