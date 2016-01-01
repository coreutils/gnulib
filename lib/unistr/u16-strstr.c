/* Substring test for UTF-16 strings.
   Copyright (C) 1999, 2002, 2006, 2010-2016 Free Software Foundation, Inc.
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

#include "malloca.h"

/* FIXME: Maybe walking the string via u16_mblen is a win?  */

#define UNIT uint16_t

#define CANON_ELEMENT(c) c
#include "str-kmp.h"

#define FUNC u16_strstr
#define U_STRCHR u16_strchr
#define U_STRMBTOUC u16_strmbtouc
#define U_STRLEN u16_strlen
#define U_STRNLEN u16_strnlen
#include "u-strstr.h"
