/* Mapping a single-byte character to uppercase.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

#include <config.h>

/* Specification.  */
#include <ctype.h>

#define FUNC toupper_l
#define GLOBAL_FUNC toupper
#define C_FUNC(c) \
  (c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c)
/* Documentation:
   <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/toupper-toupper-towupper-toupper-l-towupper-l>  */
#define WINDOWS_FUNC _toupper_l
#include "to_l-impl.h"
