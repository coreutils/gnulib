/*
 * Copyright (C) 2005, 2006 Free Software Foundation
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

#include <config.h>

#include <stdio.h>
#include <string.h>
#include "gc.h"

int
main (int argc, char *argv[])
{
  Gc_rc rc;

  rc = gc_init ();
  if (rc != GC_OK)
    {
      printf ("gc_init() failed\n");
      return 1;
    }

  gc_done ();

  return 0;
}
