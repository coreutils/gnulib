/* Arabic joining type of Unicode characters.
   Copyright (C) 2011 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2011.

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

int
uc_joining_type_byname (const char *joining_type_name)
{
  if (joining_type_name[0] != '\0' && joining_type_name[1] == '\0')
    switch (joining_type_name[0])
      {
      case 'C':
        return UC_JOINING_TYPE_C;
      case 'D':
        return UC_JOINING_TYPE_D;
      case 'L':
        return UC_JOINING_TYPE_L;
      case 'R':
        return UC_JOINING_TYPE_R;
      case 'T':
        return UC_JOINING_TYPE_T;
      case 'U':
        return UC_JOINING_TYPE_U;
      }
  /* Invalid joining type name.  */
  return -1;
}
