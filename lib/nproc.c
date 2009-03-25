/* Detect the number of processors.

   Copyright (C) 2009 Free Software Foundation, Inc.

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

/* Written by Glen Lenker.  */

#include <config.h>
#include "nproc.h"

#include <unistd.h>

/* Return the total number of processors.  The result is guaranteed to
   be at least 1.  */
unsigned long int
num_processors (void)
{
#ifdef _SC_NPROCESSORS_ONLN
  long int nprocs = sysconf (_SC_NPROCESSORS_ONLN);
  if (0 < nprocs)
    return nprocs;
#endif

  return 1;
}
