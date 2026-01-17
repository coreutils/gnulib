/* Test of <selinux/context.h> substitute.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2026.  */

#include <config.h>

#include <selinux/context.h>

/* Check that context_t is defined.  */
context_t t1;

int
main ()
{
  context_t c = context_new ("system_u:system_r:init_t:s0");
  context_type_set (c, "bar");
  context_range_set (c, "bar");
  context_role_set (c, "bar");
  context_user_set (c, "bar");
  (void) context_type_get (c);
  (void) context_range_get (c);
  (void) context_role_get (c);
  (void) context_user_get (c);
  (void) context_str (c);
  context_free (c);

  return 0;
}
