/* Test of ordered set data type implementation as a C++ class.
   Copyright (C) 2020 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2020.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "gl_oset.hh"
#include "gl_array_oset.h"

#include <string.h>

#include "macros.h"

static int
reverse_strcmp (const char *str1, const char *str2)
{
  int cmp = strcmp (str1, str2);
  return (cmp > 0 ? -1 : cmp < 0 ? 1 : 0);
}

int
main (int argc, char *argv[])
{
  gl_OSet<const char *> set1;

  set1 = gl_OSet<const char *> (GL_ARRAY_OSET, reverse_strcmp, NULL);
  set1.add ("A");
  set1.add ("C");
  set1.add ("D");
  set1.add ("C");
  ASSERT (set1.size () == 3);

  gl_OSet<const char *>::iterator iter1 = set1.begin ();
  const char *elt;
  ASSERT (iter1.next (elt));
  ASSERT (strcmp (elt, "D") == 0);
  ASSERT (iter1.next (elt));
  ASSERT (strcmp (elt, "C") == 0);
  ASSERT (iter1.next (elt));
  ASSERT (strcmp (elt, "A") == 0);
  ASSERT (!iter1.next (elt));

  set1.free ();

  return 0;
}
