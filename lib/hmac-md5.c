/* hmac-md5.c -- hashed message authentication codes
   Copyright (C) 2018-2021 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "hmac.h"

#include "md5.h"

#define GL_HMAC_NAME 5
#define GL_HMAC_BLOCKSIZE 64
#define GL_HMAC_HASHSIZE 16
#include "hmac.c"
