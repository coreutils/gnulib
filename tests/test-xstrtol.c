/* Test of xstrtol module.
   Copyright (C) 1995, 1996, 1998, 1999, 2000, 2001, 2003, 2004, 2005,
   2006, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "xstrtol.h"
#include "error.h"

#ifndef __xstrtol
# define __xstrtol xstrtol
# define __strtol_t long int
# define __spec "ld"
#endif

char *program_name;

int
main (int argc, char **argv)
{
  strtol_error s_err;
  int i;

  program_name = argv[0];
  for (i = 1; i < argc; i++)
    {
      char *p;
      __strtol_t val;

      s_err = __xstrtol (argv[i], &p, 0, &val, "bckMw0");
      if (s_err == LONGINT_OK)
	{
	  printf ("%s->%" __spec " (%s)\n", argv[i], val, p);
	}
      else
	{
	  STRTOL_FATAL_ERROR ("arg", argv[i], s_err);
	}
    }
  exit (0);
}
