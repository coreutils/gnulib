/*
 * Copyright (C) 2004, 2007 Free Software Foundation
 * Written by Simon Josefsson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

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
