/* Conversion to UTF-16 from legacy encodings.
   Copyright (C) 2002, 2006-2007, 2009-2010 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>.  */

#include <config.h>

/* Specification.  */
#include "uniconv.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "unistr.h"

#define FUNC u16_strconv_from_encoding
#define UNIT uint16_t
#define U_CONV_FROM_ENCODING u16_conv_from_encoding
#define U_STRLEN u16_strlen
#include "u-strconv-from-enc.h"
