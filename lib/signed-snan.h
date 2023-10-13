/* Macros for signalling not-a-number.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

#ifndef _SIGNED_SNAN_H
#define _SIGNED_SNAN_H

#include "signed-nan.h"
#include "snan.h"


#if HAVE_SNANF

/* Returns a signalling 'float' NaN with sign bit == 0.  */
_GL_UNUSED static float
positive_SNaNf ()
{
  return construct_SNaNf (positive_NaNf ());
}

/* Returns a signalling 'float' NaN with sign bit == 1.  */
_GL_UNUSED static float
negative_SNaNf ()
{
  return construct_SNaNf (negative_NaNf ());
}

#endif


#if HAVE_SNAND

/* Returns a signalling 'double' NaN with sign bit == 0.  */
_GL_UNUSED static double
positive_SNaNd ()
{
  return construct_SNaNd (positive_NaNd ());
}

/* Returns a signalling 'double' NaN with sign bit == 1.  */
_GL_UNUSED static double
negative_SNaNd ()
{
  return construct_SNaNd (negative_NaNd ());
}

#endif


#if HAVE_SNANL

/* Returns a signalling 'long double' NaN with sign bit == 0.  */
_GL_UNUSED static long double
positive_SNaNl ()
{
  return construct_SNaNl (positive_NaNl ());
}

/* Returns a signalling 'long double' NaN with sign bit == 1.  */
_GL_UNUSED static long double
negative_SNaNl ()
{
  return construct_SNaNl (negative_NaNl ());
}

#endif


#endif /* _SIGNED_SNAN_H */
