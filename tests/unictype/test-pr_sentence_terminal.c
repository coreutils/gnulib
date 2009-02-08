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

    { 0x0021, 0x0021 },
    { 0x002E, 0x002E },
    { 0x003F, 0x003F },
    { 0x055C, 0x055C },
    { 0x055E, 0x055E },
    { 0x0589, 0x0589 },
    { 0x061F, 0x061F },
    { 0x06D4, 0x06D4 },
    { 0x0700, 0x0702 },
    { 0x07F9, 0x07F9 },
    { 0x0964, 0x0965 },
    { 0x104A, 0x104B },
    { 0x1362, 0x1362 },
    { 0x1367, 0x1368 },
    { 0x166E, 0x166E },
    { 0x1803, 0x1803 },
    { 0x1809, 0x1809 },
    { 0x1944, 0x1945 },
    { 0x1B5A, 0x1B5B },
    { 0x1B5E, 0x1B5F },
    { 0x1C3B, 0x1C3C },
    { 0x1C7E, 0x1C7F },
    { 0x203C, 0x203D },
    { 0x2047, 0x2049 },
    { 0x2E2E, 0x2E2E },
    { 0x3002, 0x3002 },
    { 0xA60E, 0xA60F },
    { 0xA876, 0xA877 },
    { 0xA8CE, 0xA8CF },
    { 0xA92F, 0xA92F },
    { 0xAA5D, 0xAA5F },
    { 0xFE52, 0xFE52 },
    { 0xFE56, 0xFE57 },
    { 0xFF01, 0xFF01 },
    { 0xFF0E, 0xFF0E },
    { 0xFF1F, 0xFF1F },
    { 0xFF61, 0xFF61 }

#define PREDICATE(c) uc_is_property_sentence_terminal (c)
#include "test-predicate-part2.h"
