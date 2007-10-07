/*
 * Copyright (C) 2004, 2007 Free Software Foundation
 * Written by Simon Josefsson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <stdio.h>
#include <string.h>

int
main (int argc, char *argv[])
{
  const char *big = "foo-bar-baz";
  char *p;

  p = memmem (big, "foo", strlen (big));
  if (p != big)
    {
      fprintf (stderr, "memmem FAILURE 1\n");
      return 1;
    }

  p = memmem (big, "baz", strlen (big));
  if (p != big + strlen (big) - 3)
    {
      fprintf (stderr, "memmem FAILURE 2\n");
      return 1;

  p = memmem (big, "-", strlen (big));
  if (p != big + 3)
    {
      fprintf (stderr, "memmem FAILURE 3\n");
      return 1;
    }

  p = memmem (big, "baz", strlen (big) - 1);
  if (p != NULL)
    {
      fprintf (stderr, "memmem FAILURE 4\n");
      return 1;
    }

  return 0;
}
