/* Next grapheme cluster function.
   Copyright (C) 2010-2025 Free Software Foundation, Inc.

   This file is free software.
   It is dual-licensed under "the GNU LGPLv3+ or the GNU GPLv2+".
   You can redistribute it and/or modify it under either
     - the terms of the GNU Lesser General Public License as published
       by the Free Software Foundation, either version 3, or (at your
       option) any later version, or
     - the terms of the GNU General Public License as published by the
       Free Software Foundation; either version 2, or (at your option)
       any later version, or
     - the same dual license "the GNU LGPLv3+ or the GNU GPLv2+".

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public
   License and of the GNU General Public License along with this
   program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Written by Bruno Haible <bruno@clisp.org>, 2025.  */

/* Don't use the const-improved function macros in this compilation unit.  */
#define _LIBUNISTRING_NO_CONST_GENERICS

#include <config.h>

/* Specification.  */
#include "unigbrk.h"

#include "unictype.h"
#include "unistr.h"

#define FUNC u8_grapheme_next
#define UNIT uint8_t
#define U_MBTOUC u8_mbtouc
#include "u-grapheme-next.h"
