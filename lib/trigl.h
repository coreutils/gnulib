/* Declarations for sinl, cosl, tanl internal functions
   Contributed by Paolo Bonzini

   Copyright 2002, 2003 Free Software Foundation, Inc.

   This file is part of gnulib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

extern int ieee754_rem_pio2l (long double x, long double *y);
extern long double kernel_sinl (long double x, long double y, int iy);
extern long double kernel_cosl (long double x, long double y);

