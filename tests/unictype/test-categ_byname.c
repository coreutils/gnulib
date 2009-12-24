/* Test the Unicode character type functions.
   Copyright (C) 2007-2009 Free Software Foundation, Inc.

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

#include "unictype.h"

#include <string.h>

#include "macros.h"

int
main ()
{
  uc_general_category_byname ("L");
  uc_general_category_byname ("Nl");

  {
    uc_general_category_t ct = uc_general_category_byname ("Nd");
    unsigned int c;

    for (c = 0x30; c <= 0x39; c++)
      ASSERT (uc_is_general_category (c, ct));
    for (c = 0x40; c < 0x80; c++)
      ASSERT (!uc_is_general_category (c, ct));
  }

  {
    uc_general_category_t ct = uc_general_category_byname ("Foo");
    unsigned int c;

    for (c = 0; c < 0x110000; c++)
      ASSERT (!uc_is_general_category (c, ct));
  }

  return 0;
}
