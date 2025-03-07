/* Formatted output to strings in C locale.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

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
#include "c-vasnprintf.h"

#include <string.h>

#include "printf-parse.h"

#define VASNPRINTF c_vasnprintf
#define FCHAR_T char
#define DIRECTIVE char_directive
#define DIRECTIVES char_directives
#define PRINTF_PARSE printf_parse
#define DCHAR_T char
#define DCHAR_CPY memcpy
#define DCHAR_SET memset
#define TCHAR_T char
#define DCHAR_IS_TCHAR 1

#define NEED_PRINTF_DOUBLE 1
#define NEED_PRINTF_LONG_DOUBLE 1
#define decimal_point_char_defined 1
static char
decimal_point_char (void)
{
  return '.';
}

#include "vasnprintf.c"
