/* hashcode-string2.c -- compute a hash value from a NUL-terminated string.

   Copyright (C) 2001, 2003, 2006, 2009-2026 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include "hashcode-string2.h"

#include <limits.h>
#include <stdbit.h>

/* A hash function for NUL-terminated char* strings using
   the method described by Bruno Haible.
   See https://www.haible.de/bruno/hashfunc.html.  */

size_t
hash_pjw (const void *x, size_t tablesize)
{
  size_t h = 0;

  for (const char *s = x; *s; s++)
    h = *s + stdc_rotate_left (h, 9);

  return h % tablesize;
}
