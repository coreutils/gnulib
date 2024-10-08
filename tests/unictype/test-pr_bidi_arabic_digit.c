/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Test the Unicode character type functions.
   Copyright (C) 2007-2024 Free Software Foundation, Inc.

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

    { 0x0600, 0x0605 },
    { 0x0660, 0x0669 },
    { 0x066B, 0x066C },
    { 0x06DD, 0x06DD },
    { 0x0890, 0x0891 },
    { 0x08E2, 0x08E2 },
    { 0x10D30, 0x10D39 },
    { 0x10D40, 0x10D49 },
    { 0x10E60, 0x10E7E }

#define PREDICATE(c) uc_is_property_bidi_arabic_digit (c)
#include "test-predicate-part2.h"
