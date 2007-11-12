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

    { 0x0000, 0x0008 },
    { 0x000E, 0x001F },
    { 0x007F, 0x0084 },
    { 0x0086, 0x009F },
    { 0x00AD, 0x00AD },
    { 0x034F, 0x034F },
    { 0x0600, 0x0603 },
    { 0x06DD, 0x06DD },
    { 0x070F, 0x070F },
    { 0x115F, 0x1160 },
    { 0x17B4, 0x17B5 },
    { 0x180B, 0x180D },
    { 0x200B, 0x200F },
    { 0x202A, 0x202E },
    { 0x2060, 0x206F },
    { 0x3164, 0x3164 },
    { 0xD800, 0xDFFF },
    { 0xFDD0, 0xFDEF },
    { 0xFE00, 0xFE0F },
    { 0xFEFF, 0xFEFF },
    { 0xFFA0, 0xFFA0 },
    { 0xFFF0, 0xFFF8 },
    { 0xFFFE, 0xFFFF },
    { 0x1D173, 0x1D17A },
    { 0x1FFFE, 0x1FFFF },
    { 0x2FFFE, 0x2FFFF },
    { 0x3FFFE, 0x3FFFF },
    { 0x4FFFE, 0x4FFFF },
    { 0x5FFFE, 0x5FFFF },
    { 0x6FFFE, 0x6FFFF },
    { 0x7FFFE, 0x7FFFF },
    { 0x8FFFE, 0x8FFFF },
    { 0x9FFFE, 0x9FFFF },
    { 0xAFFFE, 0xAFFFF },
    { 0xBFFFE, 0xBFFFF },
    { 0xCFFFE, 0xCFFFF },
    { 0xDFFFE, 0xE0FFF },
    { 0xEFFFE, 0xEFFFF },
    { 0xFFFFE, 0xFFFFF },
    { 0x10FFFE, 0x10FFFF }

#define PREDICATE(c) uc_is_property_default_ignorable_code_point (c)
#include "test-predicate-part2.h"
