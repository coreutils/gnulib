/* Tests of statat and lstatat.
   Copyright (C) 2009-2025 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>

#include "openat.h"

/* This program tests deprecated functions 'statat' and 'lstatat'.  */
#if _GL_GNUC_PREREQ (4, 3)
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "signature.h"
SIGNATURE_CHECK (statat, int, (int, char const *, struct stat *));
SIGNATURE_CHECK (lstatat, int, (int, char const *, struct stat *));

#define BASE "test-statat.t"
#define TEST_STATAT
#include "test-fstatat.c"
