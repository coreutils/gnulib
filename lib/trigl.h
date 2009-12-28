/* Declarations for sinl, cosl, tanl internal functions
   Contributed by Paolo Bonzini

   Copyright 2002, 2003, 2009 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

extern int ieee754_rem_pio2l (long double x, long double *y);
extern long double kernel_sinl (long double x, long double y, int iy);
extern long double kernel_cosl (long double x, long double y);

