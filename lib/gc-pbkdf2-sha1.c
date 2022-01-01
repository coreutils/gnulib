/* gc-pbkdf2-sha1.c --- Password-Based Key Derivation Function a'la PKCS#5
   Copyright (C) 2002-2006, 2009-2022 Free Software Foundation, Inc.

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

/* Written by Simon Josefsson. */

#include <config.h>

#include "gc.h"

#include <stdlib.h>
#include <string.h>

Gc_rc
gc_pbkdf2_sha1 (const char *P, size_t Plen,
                const char *S, size_t Slen,
                unsigned int c,
                char *DK, size_t dkLen)
{
  return gc_pbkdf2_hmac (GC_SHA1, P, Plen, S, Slen, c, DK, dkLen);
}
