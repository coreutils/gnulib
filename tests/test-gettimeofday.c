/*
 * Copyright (C) 2005 Free Software Foundation
 * Written by Jim Meyering.
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

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "gettimeofday.h"

int
main (int argc, char *argv[])
{
  suseconds_t dummy = 0;	/* just to test if this type is available */
  time_t t = 0;
  struct tm *lt;
  struct tm saved_lt;
  struct timeval tv;
  lt = localtime (&t);
  saved_lt = *lt;
  gettimeofday (&tv, NULL);
  if (memcmp (lt, &saved_lt, sizeof (struct tm)) != 0)
    {
      fprintf (stderr, "gettimeofday still clobbers the localtime buffer!\n");
      return 1;
    }
  else
    {
      return 0;
    }
}
