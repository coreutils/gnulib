/* Copy UTF-8 string.
   Copyright (C) 2002, 2006, 2009-2010 Free Software Foundation, Inc.
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

#include <config.h>

/* Specification.  */
#include "unistr.h"

#if HAVE_STRDUP

# include <string.h>

uint8_t *
u8_strdup (const uint8_t *s)
{
  return (uint8_t *) strdup ((const char *) s);
}

#else

# define FUNC u8_strdup
# define UNIT uint8_t
# define U_STRLEN u8_strlen
# include "u-strdup.h"

#endif
