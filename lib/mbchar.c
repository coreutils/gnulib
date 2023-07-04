/* Copyright (C) 2001, 2006, 2009-2023 Free Software Foundation, Inc.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */


#include <config.h>

#define MBCHAR_INLINE _GL_EXTERN_INLINE

#include <limits.h>

#include "mbchar.h"

#if IS_BASIC_ASCII

/* Bit table of characters in the POSIX "portable character set", which
   POSIX guarantees to be single-byte and in practice are safe to treat
   like the ISO C "basic character set".  */
const unsigned int is_basic_table [UCHAR_MAX / 32 + 1] =
{
  0x00003f81,           /* '\0' '\007' '\010' '\t' '\n' '\v' '\f' '\r' */
  0xffffffff,           /* ' '......'?' */
  0xffffffff,           /* '@' 'A'...'Z' '[' '\\' ']' '^' '_' */
  0x7fffffff            /* '`' 'a'...'z' '{' '|' '}' '~' */
  /* The remaining bits are 0.  */
};

#endif /* IS_BASIC_ASCII */
