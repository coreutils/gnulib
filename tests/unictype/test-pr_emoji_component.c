/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* Test the Unicode character type functions.
   Copyright (C) 2007-2022 Free Software Foundation, Inc.

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

    { 0x0023, 0x0023 },
    { 0x002A, 0x002A },
    { 0x0030, 0x0039 },
    { 0x200D, 0x200D },
    { 0x20E3, 0x20E3 },
    { 0xFE0F, 0xFE0F },
    { 0x1F1E6, 0x1F1FF },
    { 0x1F3FB, 0x1F3FF },
    { 0x1F9B0, 0x1F9B3 },
    { 0xE0020, 0xE007F }

#define PREDICATE(c) uc_is_property_emoji_component (c)
#include "test-predicate-part2.h"
