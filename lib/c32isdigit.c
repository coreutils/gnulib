/* Test 32-bit wide character for being a digit.
   Copyright (C) 2020 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

/* Specification.  */
#include <uchar.h>

#define FUNC c32isdigit
#define WCHAR_FUNC iswdigit
#define UCS_FUNC uc_is_digit
#include "c32is-impl.h"
