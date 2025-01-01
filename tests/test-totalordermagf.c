/* Test totalordermagf.
   Copyright 2023-2025 Free Software Foundation, Inc.

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

/* Specification.  */
#include <math.h>

#include "signature.h"
SIGNATURE_CHECK (totalordermagf, int, (const float *, const float *));

#define TOTALORDER totalordermagf
#define TOTALORDER_TYPE memory_float
#define TOTALORDER_INF Infinityf
#define TOTALORDER_MINUS_ZERO minus_zerof
#define TOTALORDER_MINUS minus_NaNf
#define TOTALORDER_SETPAYLOAD setpayloadf
#define TOTALORDER_HAVE_SNAN HAVE_SNANF
#define TOTALORDER_POSITIVE_SNAN memory_positive_SNaNf
#define TOTALORDER_NEGATIVE_SNAN memory_negative_SNaNf
#include "test-totalordermag.h"
