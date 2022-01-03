/* Declarations for sinl, cosl, tanl internal functions.  -*- coding: utf-8 -*-
   Contributed by Paolo Bonzini

   Copyright 2002-2003, 2009-2022 Free Software Foundation, Inc.

   This file is part of gnulib.

   This file is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* Decompose x into x = k * π/2 + r
   where k is an integer and abs(r) <= π/4.
   Store r in y[0] and y[1] (main part in y[0], small additional part in
   y[1], r = y[0] + y[1]).
   Return k.  */
extern int ieee754_rem_pio2l (long double x, long double *y);

/* Compute and return sinl (x + y), where x is the main part and y is the
   small additional part of a floating-point number.
   iy is 0 when y is known to be 0.0, otherwise iy is 1.  */
extern long double kernel_sinl (long double x, long double y, int iy);

/* Compute and return cosl (x + y), where x is the main part and y is the
   small additional part of a floating-point number.  */
extern long double kernel_cosl (long double x, long double y);
