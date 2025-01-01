/* Test totalorder.
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
SIGNATURE_CHECK (totalorder, int, (const double *, const double *));

#define TOTALORDER totalorder
#define TOTALORDER_TYPE memory_double
#define TOTALORDER_INF Infinityd
#define TOTALORDER_MINUS_ZERO minus_zerod
#define TOTALORDER_MINUS minus_NaNd
#define TOTALORDER_SETPAYLOAD setpayload
#define TOTALORDER_HAVE_SNAN HAVE_SNAND
#define TOTALORDER_POSITIVE_SNAN memory_positive_SNaNd
#define TOTALORDER_NEGATIVE_SNAN memory_negative_SNaNd
#include "test-totalorder.h"
