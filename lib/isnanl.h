/* Test for NaN that does not need libm.
   Copyright (C) 2007 Free Software Foundation, Inc.

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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#if HAVE_ISNANL_IN_LIBC
/* Get declaration of isnan macro or (older) isnanl function.  */
# include <math.h>
# ifdef isnan
#  undef isnanl
#  define isnanl(x) isnan ((long double)(x))
# endif
#else
/* Test whether X is a NaN.  */
# undef isnanl
# define isnanl rpl_isnanl
extern int isnanl (long double x);
#endif
