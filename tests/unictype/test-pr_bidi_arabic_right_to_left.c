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

    { 0x0604, 0x0605 },
    { 0x0608, 0x0608 },
    { 0x060B, 0x060B },
    { 0x060D, 0x060D },
    { 0x061B, 0x064A },
    { 0x065F, 0x065F },
    { 0x066D, 0x066F },
    { 0x0671, 0x06D5 },
    { 0x06E5, 0x06E6 },
    { 0x06EE, 0x06EF },
    { 0x06FA, 0x070E },
    { 0x0710, 0x0710 },
    { 0x0712, 0x072F },
    { 0x074B, 0x07A5 },
    { 0x07B1, 0x07BF },
    { 0x2065, 0x2069 },
    { 0xFB50, 0xFD3D },
    { 0xFD40, 0xFDCF },
    { 0xFDF0, 0xFDFC },
    { 0xFDFE, 0xFDFF },
    { 0xFE1A, 0xFE1F },
    { 0xFE27, 0xFE2F },
    { 0xFE53, 0xFE53 },
    { 0xFE67, 0xFE67 },
    { 0xFE6C, 0xFEFE }

#define PREDICATE(c) uc_is_property_bidi_arabic_right_to_left (c)
#include "test-predicate-part2.h"
