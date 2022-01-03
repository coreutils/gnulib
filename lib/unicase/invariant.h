/* Internal functions for Unicode character case mappings.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.
   Written by Bruno Haible <bruno@clisp.org>, 2009.

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

#include <stdbool.h>
#include <stddef.h>

#include "unitypes.h"
#include "uninorm.h"

/* Return true if mapping NFD(S) to via MAPPING is a no-op.  */
extern int
       u8_is_invariant (const uint8_t *s, size_t n,
                        uint8_t * (*mapping) (const uint8_t *s, size_t n, const char *iso639_language,
                                              uninorm_t nf,
                                              uint8_t *resultbuf, size_t *lengthp),
                        const char *iso639_language,
                        bool *resultp);
extern int
       u16_is_invariant (const uint16_t *s, size_t n,
                         uint16_t * (*mapping) (const uint16_t *s, size_t n, const char *iso639_language,
                                                uninorm_t nf,
                                                uint16_t *resultbuf, size_t *lengthp),
                         const char *iso639_language,
                         bool *resultp);
extern int
       u32_is_invariant (const uint32_t *s, size_t n,
                         uint32_t * (*mapping) (const uint32_t *s, size_t n, const char *iso639_language,
                                                uninorm_t nf,
                                                uint32_t *resultbuf, size_t *lengthp),
                         const char *iso639_language,
                         bool *resultp);
