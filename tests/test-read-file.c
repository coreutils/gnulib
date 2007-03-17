/*
 * Copyright (C) 2006-2007 Free Software Foundation
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "read-file.h"

#include <stdio.h>
#include <stdlib.h>

#define FILE1 "/etc/resolv.conf"
#define FILE2 "/dev/null"

int
main (void)
{
  {
    size_t len;
    char *out = read_file (FILE1, &len);

    if (!out)
      perror ("Could not read file");

    if (out[len] != '\0')
      perror ("BAD: out[len] not zero");

    printf ("Read %ld from %s...\n", (unsigned long) len, FILE1);

    free (out);
  }

  {
    size_t len;
    char *out = read_file (FILE2, &len);

    if (!out)
      perror ("Could not read file");

    if (out[len] != '\0')
      perror ("BAD: out[len] not zero");

    printf ("Read %ld from %s...\n", (unsigned long) len, FILE2);

    free (out);
  }

  return 0;
}
