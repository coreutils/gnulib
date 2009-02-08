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

    { 0x00B2, 0x00B3 },
    { 0x00B9, 0x00B9 },
    { 0x00BC, 0x00BE },
    { 0x09F4, 0x09F9 },
    { 0x0BF0, 0x0BF2 },
    { 0x0C78, 0x0C7E },
    { 0x0D70, 0x0D75 },
    { 0x0F2A, 0x0F33 },
    { 0x1369, 0x137C },
    { 0x17F0, 0x17F9 },
    { 0x2070, 0x2070 },
    { 0x2074, 0x2079 },
    { 0x2080, 0x2089 },
    { 0x2153, 0x215F },
    { 0x2460, 0x249B },
    { 0x24EA, 0x24FF },
    { 0x2776, 0x2793 },
    { 0x2CFD, 0x2CFD },
    { 0x3192, 0x3195 },
    { 0x3220, 0x3229 },
    { 0x3251, 0x325F },
    { 0x3280, 0x3289 },
    { 0x32B1, 0x32BF },
    { 0x10107, 0x10133 },
    { 0x10175, 0x10178 },
    { 0x1018A, 0x1018A },
    { 0x10320, 0x10323 },
    { 0x10916, 0x10919 },
    { 0x10A40, 0x10A47 },
    { 0x1D360, 0x1D371 }

#define PREDICATE(c) uc_is_general_category (c, UC_CATEGORY_No)
#include "test-predicate-part2.h"
