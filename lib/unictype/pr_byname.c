/* Properties of Unicode characters.
   Copyright (C) 2007, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2007.

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

/* Get gperf generated lookup function.  */
#include "unictype/pr_byname.h"

static const uc_property_t UC_PROPERTY_NONE = { NULL };

uc_property_t
uc_property_byname (const char *property_name)
{
  char buf[MAX_WORD_LENGTH + 1];
  const char *cp;
  char *bp;
  unsigned int count;
  const struct named_property *found;

  for (cp = property_name, bp = buf, count = MAX_WORD_LENGTH + 1; ; cp++, bp++)
    {
      unsigned char c = (unsigned char) *cp;
      if (c >= 0x80)
        goto invalid;
      if (c >= 'A' && c <= 'Z')
        c += 'a' - 'A';
      else if (c == ' ' || c == '-')
        c = '_';
      *bp = c;
      if (c == '\0')
        break;
      if (--count == 0)
        goto invalid;
    }
  found = uc_property_lookup (buf, bp - buf);
  if (found != NULL)
    return found->property;
 invalid:
  return UC_PROPERTY_NONE;
}
