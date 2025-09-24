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

    { 0x0654, 0x0655 },
    { 0x0658, 0x0658 },
    { 0x06DC, 0x06DC },
    { 0x06E3, 0x06E3 },
    { 0x06E7, 0x06E8 },
    { 0x08CA, 0x08CB },
    { 0x08CD, 0x08CF },
    { 0x08D3, 0x08D3 },
    { 0x08F3, 0x08F3 }

#define PREDICATE(c) uc_is_property_modifier_combining_mark (c)
#include "test-predicate-part2.h"
