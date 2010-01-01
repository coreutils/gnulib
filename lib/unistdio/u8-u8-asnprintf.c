/* Formatted output to strings.
   Copyright (C) 1999, 2002, 2005-2007, 2009-2010 Free Software Foundation,
   Inc.

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
#include "unistdio.h"

#include <stdarg.h>

#define ASNPRINTF u8_u8_asnprintf
#define VASNPRINTF u8_u8_vasnprintf
#define FCHAR_T uint8_t
#define DCHAR_T uint8_t
#include "u-asnprintf.h"
