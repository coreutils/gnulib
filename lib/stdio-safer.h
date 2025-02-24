/* Invoke stdio functions, but avoid some glitches.

   Copyright (C) 2001, 2003, 2006, 2009-2025 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

/* This file uses _GL_ATTRIBUTE_DEALLOC.  */
#if !_GL_CONFIG_H_INCLUDED
 #error "Please include config.h first."
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#if GNULIB_FOPEN_SAFER
FILE *fopen_safer (char const *, char const *)
  _GL_ARG_NONNULL ((1, 2)) _GL_ATTRIBUTE_DEALLOC (fclose, 1);
#endif

#if GNULIB_FREOPEN_SAFER
FILE *freopen_safer (char const *, char const *, FILE *)
  _GL_ARG_NONNULL ((2, 3));
#endif

#if GNULIB_POPEN_SAFER
FILE *popen_safer (char const *, char const *)
  _GL_ARG_NONNULL ((1, 2)) _GL_ATTRIBUTE_DEALLOC (pclose, 1);
#endif

#if GNULIB_TMPFILE_SAFER
FILE *tmpfile_safer (void)
  _GL_ATTRIBUTE_DEALLOC (fclose, 1);
#endif


#ifdef __cplusplus
}
#endif
