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

    { 0x0030, 0x0039 },
    { 0x0660, 0x0669 },
    { 0x06F0, 0x06F9 },
    { 0x07C0, 0x07C9 },
    { 0x0966, 0x096F },
    { 0x09E6, 0x09EF },
    { 0x0A66, 0x0A6F },
    { 0x0AE6, 0x0AEF },
    { 0x0B66, 0x0B6F },
    { 0x0BE6, 0x0BEF },
    { 0x0C66, 0x0C6F },
    { 0x0CE6, 0x0CEF },
    { 0x0D66, 0x0D6F },
    { 0x0E50, 0x0E59 },
    { 0x0ED0, 0x0ED9 },
    { 0x0F20, 0x0F29 },
    { 0x1040, 0x1049 },
    { 0x1090, 0x1099 },
    { 0x17E0, 0x17E9 },
    { 0x1810, 0x1819 },
    { 0x1946, 0x194F },
    { 0x19D0, 0x19D9 },
    { 0x1B50, 0x1B59 },
    { 0x1BB0, 0x1BB9 },
    { 0x1C40, 0x1C49 },
    { 0x1C50, 0x1C59 },
    { 0xA620, 0xA629 },
    { 0xA8D0, 0xA8D9 },
    { 0xA900, 0xA909 },
    { 0xAA50, 0xAA59 },
    { 0xFF10, 0xFF19 },
    { 0x104A0, 0x104A9 },
    { 0x1D7CE, 0x1D7FF }

#define PREDICATE(c) uc_is_property_decimal_digit (c)
#include "test-predicate-part2.h"
