/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Test the Unicode character type functions.
   Copyright (C) 2007 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "test-predicate-part1.h"

    { 0x02B0, 0x02B8 },
    { 0x02C0, 0x02C1 },
    { 0x02E0, 0x02E4 },
    { 0x0345, 0x0345 },
    { 0x037A, 0x037A },
    { 0x1D2C, 0x1D61 },
    { 0x1D78, 0x1D78 },
    { 0x1D9B, 0x1DBF },
    { 0x2090, 0x2094 },
    { 0x2170, 0x217F },
    { 0x24D0, 0x24E9 },
    { 0x2C7D, 0x2C7D },
    { 0xA770, 0xA770 }

#define PREDICATE(c) uc_is_property_other_lowercase (c)
#include "test-predicate-part2.h"
