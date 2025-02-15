/* Test whether a single-byte character is a punctuation or symbol character.
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

#define FUNC ispunct_l
#define GLOBAL_FUNC ispunct
#define C_FUNC(c) \
  ((c >= 0x21 && c <= 0x7e) \
   && !((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
/* Documentation:
   <https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/ispunct-iswpunct-ispunct-l-iswpunct-l>  */
#define WINDOWS_FUNC _ispunct_l
#include "is_l-impl.h"
