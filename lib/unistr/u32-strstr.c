/* Substring test for UTF-32 strings.
   Copyright (C) 1999, 2002, 2006, 2009-2025 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2002.

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

/* Don't use the const-improved function macros in this compilation unit.  */
#define _LIBUNISTRING_NO_CONST_GENERICS

#include <config.h>

/* Specification.  */
#include "unistr.h"

#define UNIT uint32_t
#define FUNC u32_strstr
#define RETURN_TYPE uint32_t *
#define MEMCHR0(s, n) u32_chr (s, n, 0)
#define STRCHR u32_strchr
#define CMP_FUNC u32_cmp
#include "wcsstr-impl.h"
