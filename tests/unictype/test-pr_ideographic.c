/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Test the Unicode character type functions.
   Copyright (C) 2007-2025 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "test-predicate-part1.h"

    { 0x3006, 0x3007 },
    { 0x3021, 0x3029 },
    { 0x3038, 0x303A },
    { 0x3400, 0x4DBF },
    { 0x4E00, 0x9FFF },
    { 0xF900, 0xFA6D },
    { 0xFA70, 0xFAD9 },
    { 0x16FE4, 0x16FE4 },
    { 0x16FF2, 0x16FF6 },
    { 0x17000, 0x18CD5 },
    { 0x18CFF, 0x18D1E },
    { 0x18D80, 0x18DF2 },
    { 0x1B170, 0x1B2FB },
    { 0x20000, 0x2A6DF },
    { 0x2A700, 0x2B81D },
    { 0x2B820, 0x2CEAD },
    { 0x2CEB0, 0x2EBE0 },
    { 0x2EBF0, 0x2EE5D },
    { 0x2F800, 0x2FA1D },
    { 0x30000, 0x3134A },
    { 0x31350, 0x33479 }

#define PREDICATE(c) uc_is_property_ideographic (c)
#include "test-predicate-part2.h"
