/* Test of string descriptors.
   Copyright (C) 2023-2024 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2023.  */

#include <config.h>

#include "xstring-desc.h"

#include <stdlib.h>
#include <string.h>

#include "macros.h"

int
main (void)
{
  string_desc_t s0 = string_desc_new_empty ();
  string_desc_t s1 = string_desc_from_c ("Hello world!");
  string_desc_t s2 = string_desc_new_addr (21, "The\0quick\0brown\0\0fox");

  /* Test xstring_desc_new.  */
  string_desc_t s4 = xstring_desc_new (5);
  string_desc_set_char_at (s4, 0, 'H');
  string_desc_set_char_at (s4, 4, 'o');
  string_desc_set_char_at (s4, 1, 'e');
  string_desc_fill (s4, 2, 4, 'l');
  ASSERT (string_desc_length (s4) == 5);
  ASSERT (string_desc_startswith (s1, s4));

  /* Test xstring_desc_new_filled.  */
  string_desc_t s5 = xstring_desc_new_filled (5, 'l');
  string_desc_set_char_at (s5, 0, 'H');
  string_desc_set_char_at (s5, 4, 'o');
  string_desc_set_char_at (s5, 1, 'e');
  ASSERT (string_desc_length (s5) == 5);
  ASSERT (string_desc_startswith (s1, s5));

  /* Test xstring_desc_copy.  */
  {
    string_desc_t s6 = xstring_desc_copy (s0);
    ASSERT (string_desc_is_empty (s6));
    string_desc_free (s6);
  }
  {
    string_desc_t s6 = xstring_desc_copy (s2);
    ASSERT (string_desc_equals (s6, s2));
    string_desc_free (s6);
  }

  /* Test xstring_desc_concat.  */
  {
    string_desc_t s8 =
      xstring_desc_concat (3, string_desc_new_addr (10, "The\0quick"),
                              string_desc_new_addr (7, "brown\0"),
                              string_desc_new_addr (4, "fox"),
                              string_desc_new_addr (7, "unused"));
    ASSERT (string_desc_equals (s8, s2));
    string_desc_free (s8);
  }

  /* Test xstring_desc_c.  */
  {
    char *ptr = xstring_desc_c (s2);
    ASSERT (ptr != NULL);
    ASSERT (memcmp (ptr, "The\0quick\0brown\0\0fox\0", 22) == 0);
    free (ptr);
  }

  return 0;
}
