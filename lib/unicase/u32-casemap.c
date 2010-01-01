/* Case mapping for UTF-32 strings (locale dependent).
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
#include "unicasemap.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "unistr.h"
#include "unictype.h"
#include "uninorm.h"
#include "caseprop.h"
#include "context.h"
#include "special-casing.h"

#define FUNC u32_casemap
#define UNIT uint32_t
#define U_MBTOUC_UNSAFE u32_mbtouc_unsafe
#define U_UCTOMB u32_uctomb
#define U_CPY u32_cpy
#define U_NORMALIZE u32_normalize
#include "u-casemap.h"
