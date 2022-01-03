/* Arabic joining group of Unicode characters.
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

/* Use a string pool, rather than a two-dimensional array, because the strings
   have very different lengths.  */

struct joining_group_stringpool_t
  {
#define ELEM(tag,string) char tag[sizeof (string)];
#include "unictype/joininggroup_name.h"
#undef ELEM
  };

static const struct joining_group_stringpool_t joining_group_stringpool_contents =
  {
#define ELEM(tag,string) string,
#include "unictype/joininggroup_name.h"
#undef ELEM
  };
#define joining_group_stringpool ((const char *) &joining_group_stringpool_contents)

static const int joining_group_index[] =
  {
#define ELEM(tag,string) (int)(size_t)&((struct joining_group_stringpool_t *)0)->tag,
#include "unictype/joininggroup_name.h"
#undef ELEM
  };

const char *
uc_joining_group_name (int joining_group)
{
  if (joining_group >= 0
      && joining_group < sizeof (joining_group_index) / sizeof (joining_group_index[0]))
    return joining_group_stringpool + joining_group_index[joining_group];
  return NULL;
}
