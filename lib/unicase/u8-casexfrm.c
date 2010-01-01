/* Locale dependent transformation for case insensitive comparison of UTF-8
   strings.
   Copyright (C) 2009-2010 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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
#include "unicase.h"

#include <errno.h>
#include <stdlib.h>

#include "localcharset.h"
#include "uniconv.h"
#include "memxfrm.h"

#define FUNC u8_casexfrm
#define UNIT uint8_t
#define U_CASEFOLD u8_casefold
#define U_CONV_TO_ENCODING u8_conv_to_encoding
#include "u-casexfrm.h"
