/* Declaration for long double functions.
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

#ifndef GNULIB_MATHL_H
#define GNULIB_MATHL_H

extern long double acosl (long double x);
extern long double asinl (long double x);
extern long double atanl (long double x);
extern long double ceill (long double x);
extern long double cosl (long double x);
extern long double expl (long double x);
extern long double floorl (long double x);
extern long double frexpl (long double x, int *exp);
extern long double ldexpl (long double x, int exp);
extern long double logl (long double x);
extern long double sinl (long double x);
extern long double sqrtl (long double x);
extern long double tanl (long double x);

#endif
