/* Copy UTF-8 string.
   Copyright (C) 1999, 2002, 2006, 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Ensure stpcpy() gets declared.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

#include <config.h>

/* Specification.  */
#include "unistr.h"

#if HAVE_STPCPY

# include <string.h>

uint8_t *
u8_stpcpy (uint8_t *dest, const uint8_t *src)
{
  return (uint8_t *) stpcpy ((char *) dest, (const char *) src);
}

#else

# define FUNC u8_stpcpy
# define UNIT uint8_t
# include "u-stpcpy.h"

#endif
