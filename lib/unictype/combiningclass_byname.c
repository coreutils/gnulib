/* Canonical combining classes of Unicode characters.
   Copyright (C) 2011-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "unictype.h"

#include <string.h>

#include "unictype/combiningclass_byname.h"

int
uc_combining_class_byname (const char *ccc_name)
{
  size_t len;

  len = strlen (ccc_name);
  if (len <= MAX_WORD_LENGTH)
    {
      char buf[MAX_WORD_LENGTH + 1];
      const struct named_combining_class *found;

      /* Copy ccc_name into buf, converting '_' and '-' to ' '.  */
      {
        const char *p = ccc_name;
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
      found = uc_combining_class_lookup (buf, len);
      if (found != NULL)
        return found->combining_class;
    }
  /* Invalid combining class name.  */
  return -1;
}
